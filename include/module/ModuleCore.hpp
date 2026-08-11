#pragma once

#include <Geode/Geode.hpp>

#include "settings/SettingsCache.hpp"
#include "settings/SettingsQueueHandler.hpp"
#include "utils/Utils.hpp"
#include "module/ModuleLoader.hpp"
#include "Events.hpp"

using namespace geode::prelude;

template <class T, geode::utils::string::ConstexprString Name, bool Global>
struct ModuleCore : public ModuleBase {
private:
    static inline ModuleLoader<T> $apply;
    static inline auto const $force = &$apply;

    static inline std::weak_ptr<T> $instance;
    static inline matjson::Value $queuedSettings;

public:
    ~ModuleCore() {
        if (tinker::utils::getSetting<bool, "Debug-module-logs">()) {
            log::debug("Destroyed: {}", Name.data());
        }
    }

    static constexpr bool isGlobal() {
        return Global;
    }

    static constexpr const char* getName() {
        return Name.data();
    }

    static std::shared_ptr<T> create() {
        auto ret = std::make_shared<T>();

        if (tinker::utils::getSetting<bool, "Debug-module-logs">()) {
            log::debug("Created: {}", Name.data());
        }

        $instance = ret;

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

    static std::shared_ptr<T> get() {
        return $instance.lock();
    }

    static bool isEnabled() {
        static constexpr auto enabledKey =
            tinker::utils::concat<Name, "-enabled">();

        static auto setting = Mod::get()->getSetting(enabledKey.data());
        if (!setting) {
            return false;
        }

        static bool settingEnabled = setting->shouldEnable();

        static auto listener = listenForAllSettingChanges(
            [] (std::string_view, std::shared_ptr<SettingV3> setting) {
                auto settingValue = Mod::get()->getSetting(enabledKey.data());
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

        for (const auto& hook :
             ModuleRegistry::get()->getData(getName()).getHooks()) {
            (void) hook->toggle(enabled);
        }
    }

public:
    template <class S, geode::utils::string::ConstexprString key>
    static const S& getSetting() {
        static constexpr auto fullKey =
            tinker::utils::concat<Name, "-", key>();

        static auto setting =
            Mod::get()->getSettingValue<S>(fullKey.data());

        static auto listener = listenForSettingChanges<S>(
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
                    tinker::utils::equals<
                        fullKey,
                        tinker::utils::concat<Name, "-enabled">()
                    >();

                if constexpr (isEnabledSetting) {
                    bool handled = false;

                    if constexpr (requires {
                        base->onToggled(value);
                    }) {
                        handled = base->onToggled(value);
                    }

                    if (!handled) {
                        $queuedSettings[key.data()] = value;
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
                    base->onSettingChanged(key.data(), value);
                }) {
                    handled = base->onSettingChanged(key.data(), value);
                }

                if (enabled && !handled) {
                    $queuedSettings[key.data()] = value;
                    SettingsQueueHandler::get()
                        ->addSetting(fullKey.data());
                    return;
                }

                setting = value;
            }
        );

        static auto exitListener = EditorExitEvent().listen([] {
            auto value = $queuedSettings.get(key.data());
            if (!value) {
                return;
            }

            auto queued = value.unwrap();

            if (tinker::utils::getSetting<bool, "Debug-module-logs">()) {
                log::debug("[{}]: {}", fullKey.data(), queued.dump(4));
            }

            setting = queued.template as<S>().unwrapOrDefault();

            if constexpr (
                tinker::utils::equals<
                    fullKey,
                    tinker::utils::concat<Name, "-enabled">()
                >()
            ) {
                hookToggle();
            }

            $queuedSettings.erase(key.data());
        });

        return setting;
    }
};