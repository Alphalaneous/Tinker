#pragma once

#include <Geode/Geode.hpp>
#include "settings/SettingsCache.hpp"
#include "settings/SettingsQueueHandler.hpp"
#include "utils/Utils.hpp"
#include "module/ModuleLoader.hpp"
#include "Events.hpp"

using namespace geode::prelude;

class EditorModuleBase;
class GlobalModuleBase;

template <class T, class Base, geode::utils::string::ConstexprString Name>
struct ModuleCore : Base {
private:
    static inline ModuleLoader<T, Base> $apply;
    static inline auto const $force = &$apply;
    static inline std::weak_ptr<T> $instance;

    static inline matjson::Value $queuedSettings;
public:
    virtual ~ModuleCore() {
        log::debug("Destroyed: {}", Name.data());
    }

    static std::shared_ptr<T> create() {
        auto ret = std::make_shared<T>();
        ret->ModuleBase::isEnabled = [] () {
            return isEnabled();
        };

        log::debug("Created: {}", Name.data());


        bool moduleEnabled = isEnabled();

        $instance = ret;

        if constexpr (std::is_same_v<Base, EditorModuleBase>) {
            if (moduleEnabled) {
                for (const auto& hook : ModuleRegistry<Base>::get()->m_hooks[getName()]) {
                    (void) hook->enable();
                }
            }
        }
        else {
            for (const auto& hook : ModuleRegistry<Base>::get()->m_hooks[getName()]) {
                (void) hook->toggle(moduleEnabled);
            }
        }

        return ret;
    }

    static constexpr const char* getName() {
        return Name.data();
    }

    static std::shared_ptr<T> get() {
        return $instance.lock();
    }

    static bool isEnabled() {
        static constexpr auto enabledKey = tinker::utils::concat<Name, "-enabled">();
        static auto setting = Mod::get()->getSetting(enabledKey.data());
        if (!setting) return false;
        static bool settingEnabled = setting->shouldEnable();
        static auto listener = listenForAllSettingChanges([] (std::string_view key, std::shared_ptr<SettingV3> settingA) {
            static auto setting = Mod::get()->getSetting(enabledKey.data());
            if (!setting) return;
            if (SettingsCache::get()->getSettingsList().find(enabledKey.data())->second->hasEnableIf) {
                settingEnabled = setting->shouldEnable();
            }
        });
        return settingEnabled && getSetting<bool, "enabled">();
    }

    static void globalHookToggle() {
        if constexpr (std::is_same_v<Base, GlobalModuleBase>) {
            auto enabled = isEnabled();
            for (const auto& hook : ModuleRegistry<Base>::get()->m_hooks[getName()]) {
                (void) hook->toggle(enabled);
            }
        }
    }

    template <class S, geode::utils::string::ConstexprString key>
    static const S& getSetting() {
        static constexpr auto fullKey = tinker::utils::concat<Name, "-", key>();
        static auto setting = Mod::get()->getSettingValue<S>(fullKey.data());
        static auto listener = listenForSettingChanges<S>(fullKey.data(), [] (S value) {
            if (!EditorUI::get()) {
                setting = value;
                globalHookToggle();
                return;
            }
            auto base = get();
            if (!base) return;

            if constexpr (tinker::utils::equals<fullKey, tinker::utils::concat<Name, "-enabled">()>()) {
                if (!base->onToggled(value)) {
                    $queuedSettings[key.data()] = value;
                    SettingsQueueHandler::get()->addFeature(fullKey.data());
                    return;
                }
                else {
                    setting = value;
                    auto enabled = isEnabled();
                    for (const auto& hook : ModuleRegistry<Base>::get()->m_hooks[getName()]) {
                        (void) hook->toggle(enabled);
                    }
                    return;
                }
            }
            else {
                auto enabled = isEnabled();
                if (enabled && !base->onSettingChanged(key.data(), value)) {
                    $queuedSettings[key.data()] = value;
                    SettingsQueueHandler::get()->addSetting(fullKey.data());
                    return;
                }
            }
            
            setting = value;
        });

        static auto exitListener = EditorExitEvent().listen([] {
            auto valueRes = $queuedSettings.get(key.data());
            if (!valueRes) return;

            matjson::Value s = valueRes.unwrap();
            log::debug("[{}]: {}", fullKey.data(), s.dump(4));

            setting = s.as<S>().unwrapOrDefault();
            if constexpr (tinker::utils::equals<fullKey, tinker::utils::concat<Name, "-enabled">()>()) {
                globalHookToggle();
            }
            $queuedSettings.erase(key.data());
        });

        return setting;
    }
};