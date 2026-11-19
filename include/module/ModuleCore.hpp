#pragma once

#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <matjson.hpp>
#include "settings/SettingsCache.hpp"
#include "settings/SettingsQueueHandler.hpp"
#include "utils/Settings.hpp"
#include "utils/String.hpp"
#include "utils/Mod.hpp"
#include "module/ModuleLoader.hpp"
#include "Events.hpp"
#include <alphalaneous.level-storage-api/LevelStorageAPI.hpp>
#include <memory>

template <size_t S>
constexpr bool operator==(
    geode::utils::string::ConstexprString<S> const& a,
    std::string_view b
) {
    return a.size() == b.size() &&
           std::equal(a.begin(), a.end(), b.begin());
}

enum class Save {
    Global,
    Level
};

template <class T, geode::utils::string::ConstexprString Name, bool Global>
struct ModuleCore : public ModuleBase {
private:
    static inline ModuleLoader<T> $apply{};
    static inline auto const $force = &$apply;

    static inline T* $instance;
    static inline matjson::Value $queuedSettings{};

public:
    ~ModuleCore() {
        if (tinker::utils::settings::get<bool, "Debug-module-logs">()) {
            geode::log::debug("Destroyed: {}", Name.data());
        }
    }

    static constexpr bool isGlobal() {
        return Global;
    }

    static constexpr const char* getName() {
        return Name.data();
    }

    static std::unique_ptr<T> create() {
        std::unique_ptr<T> ret = std::make_unique<T>();

        if (tinker::utils::settings::get<bool, "Debug-module-logs">()) {
            geode::log::debug("Created: {}", Name.data());
        }

        $instance = ret.get();

        auto& data = ModuleRegistry::get()->getData(getName());
        auto enabled = isEnabled();

        if constexpr (Global) {
            if (enabled) {
                for (const auto& hook : data.getHooks()) {
                    (void) hook->enable();
                }
            }
        }
        else {
            for (const auto& hook : data.getHooks()) {
                (void) hook->toggle(enabled);
            }
        }

        return ret;
    }

    static T* get() {
        return $instance;
    }

    static bool isEnabled() {
        if (!tinker::utils::mod::shouldLoadTinker()) return false;
        if constexpr (Name == "UIScaling") return true;

        static constexpr auto enabledKey = tinker::utils::string::concat<Name, "-enabled">();

        static auto setting = geode::Mod::get()->getSetting(enabledKey.data());
        if (!setting) {
            return false;
        }

        static bool settingEnabled = setting->shouldEnable();

        static auto listener = geode::listenForAllSettingChanges(
            [] (std::string_view, std::shared_ptr<geode::SettingV3> setting) {
                auto settingValue = geode::Mod::get()->getSetting(enabledKey.data());
                if (!settingValue) {
                    return;
                }

                auto cached = SettingsCache::get()
                    ->getSettingsMap()
                    .find(enabledKey.data());

                if (cached->second->hasEnableIf) {
                    settingEnabled = settingValue->shouldEnable();
                }
            }
        );

        return settingEnabled && getSetting<bool, "enabled">();
    }

private:
    static void hookToggle() {
        auto enabled = isEnabled();

        for (const auto& hook : ModuleRegistry::get()->getData(getName()).getHooks()) {
            (void) hook->toggle(enabled);
        }
    }

public:
    template <Save Location, class S, geode::utils::string::ConstexprString Key>
    static S getSaved(S defaultValue = S{}) {
        static constexpr auto fullKey = tinker::utils::string::concat<Name, "/", Key>();

        if constexpr (Location == Save::Global) {
            static auto saved = geode::Mod::get()->getSavedValue<S>(fullKey.data(), defaultValue);

            static auto listener = SaveValueChangedEvent().listen([] (const matjson::Value& value) {
                auto valueRes = value.as<S>();
                if (!valueRes) return;

                saved = valueRes.unwrap();
            }).leak();

            return saved;
        }
        if constexpr (Location == Save::Level) {
            auto container = alpha::level_storage::getSaveContainer(LevelEditorLayer::get(), geode::Mod::get());
            if (!container.contains(fullKey.data())) {
                return defaultValue;
            }

            return alpha::level_storage::getSavedValue<S>(LevelEditorLayer::get(), fullKey.data());
        }
    }

    template <Save Location, geode::utils::string::ConstexprString Key, class S>
    static void setSaved(S value) {
        static constexpr auto fullKey = tinker::utils::string::concat<Name, "/", Key>();
        if constexpr (Location == Save::Global) {
            geode::Mod::get()->setSavedValue<S>(fullKey.data(), value);
            SaveValueChangedEvent().send(value);
        }
        else if constexpr (Location == Save::Level) {
            alpha::level_storage::setSavedValue(LevelEditorLayer::get(), fullKey.data(), value);
        }
    }

    template <class S, geode::utils::string::ConstexprString Key>
    static const S& getSetting() {
        static constexpr auto fullKey = tinker::utils::string::concat<Name, "-", Key>();
        static auto setting = geode::Mod::get()->getSettingValue<S>(fullKey.data());

        static auto listener = geode::listenForSettingChanges<S>(
            fullKey.data(),
            [] (S value) {
                if (!EditorUI::get()) {
                    setting = value;
                    hookToggle();
                    return;
                }

                auto base = get();
                if (!base) {
                    return;
                }

                constexpr bool isEnabledSetting =
                    tinker::utils::string::equals<
                        fullKey,
                        tinker::utils::string::concat<Name, "-enabled">()
                    >();

                if constexpr (isEnabledSetting) {
                    bool handled = false;

                    if constexpr (requires {
                        base->onToggled(value);
                    }) {
                        handled = base->onToggled(value);
                    }

                    if (!handled) {
                        $queuedSettings[Key.data()] = value;
                        SettingsQueueHandler::get()
                            ->addFeature(fullKey.data());
                        return;
                    }

                    setting = value;
                    hookToggle();
                    return;
                }

                auto enabled = isEnabled();
                bool handled = false;

                if constexpr (requires {
                    base->onSettingChanged(Key.data(), value);
                }) {
                    handled = base->onSettingChanged(Key.data(), value);
                }

                if (enabled && !handled) {
                    $queuedSettings[Key.data()] = value;
                    SettingsQueueHandler::get()
                        ->addSetting(fullKey.data());
                    return;
                }

                setting = value;
            }
        );

        static auto exitListener = EditorExitEvent().listen([] {
            auto value = $queuedSettings.get(Key.data());
            if (!value) {
                return;
            }

            auto queued = value.unwrap();

            if (tinker::utils::settings::get<bool, "Debug-module-logs">()) {
                geode::log::debug("[{}]: {}", fullKey.data(), queued.dump(4));
            }

            setting = queued.template as<S>().unwrapOrDefault();

            if constexpr (
                tinker::utils::string::equals<
                    fullKey,
                    tinker::utils::string::concat<Name, "-enabled">()
                >()
            ) {
                hookToggle();
            }

            $queuedSettings.erase(Key.data());
        });

        return setting;
    }
};