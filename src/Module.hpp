#pragma once

#include "ModuleRegistry.hpp"
#include <Geode/Geode.hpp>
#include "Utils.hpp"
#include "SettingsQueueHandler.hpp"

using namespace geode::prelude;

class EditorExitEvent : public Event<EditorExitEvent, bool()> {
public:
    using Event::Event;
};

template <class T, class Base>
struct ModuleLoader {
    ModuleLoader() {
        ModuleRegistry<Base>::get()->m_modules.push_back([] {
            return T::create();
        });
    }
    static inline void* force = &force;
};

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
    static std::shared_ptr<T> create() {
        auto ret = std::make_shared<T>();
        ret->ModuleBase::isEnabled = [] () {
            return isEnabled();
        };

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
            settingEnabled = setting->shouldEnable();
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

struct ModuleBase {
    geode::Function<bool()> isEnabled;

    virtual bool onSettingChanged(std::string_view key, const matjson::Value& value) { return false; }
    virtual bool onToggled(bool state) { return false; }
};

struct EditorModuleBase : public ModuleBase {
    LevelEditorLayer* m_editorLayer = nullptr;
    EditorUI* m_editorUI = nullptr;
    EditorPauseLayer* m_pauseLayer = nullptr;

    virtual ~EditorModuleBase() = default;

    virtual void onEditor() {}
    virtual void onSave() {}
    virtual void onUpdateButtons() {}
    virtual void onSetGroupIDLayer(SetGroupIDLayer* setGroupIDLayer, GameObject* obj, cocos2d::CCArray* objs) {}
    virtual void onEditorPauseLayer(EditorPauseLayer* editorPauseLayer) {}
    virtual void onObjectChange(float lastObjectX) {}
    virtual void onGameTypeChange(bool isPlatformer) {}
};

struct GlobalModuleBase : public ModuleBase {
};

template <class T, geode::utils::string::ConstexprString Name>
using EditorModule = ModuleCore<T, EditorModuleBase, Name>;

template <class T, geode::utils::string::ConstexprString Name>
using GlobalModule = ModuleCore<T, GlobalModuleBase, Name>;

#define REGISTERMODULEHOOKSDECLARE(name, base, alt) \
static void onModify(auto& self) { \
    for (const auto& [k, v] : self.m_hooks) { \
        ModuleRegistry<base>::get()->m_hooks[name::getName()].push_back(v); \
        v->setAutoEnable(false); \
    } \
    alt; \
}

#define REGISTERMODULEHOOKS1(name, base) REGISTERMODULEHOOKSDECLARE(name, base, do_nothing(false))
#define REGISTERMODULEHOOKS2(name, alt, base) REGISTERMODULEHOOKSDECLARE(name, base, Self::_onModify(self))

#define $registerEditorHooks(...) \
    GEODE_INVOKE(GEODE_CONCAT(REGISTERMODULEHOOKS, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__, EditorModuleBase)

#define $registerGlobalHooks(...) \
    GEODE_INVOKE(GEODE_CONCAT(REGISTERMODULEHOOKS, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__, GlobalModuleBase)

#define $editorModule(name) \
GEODE_CONCAT(GEODE_CONCAT(name, __LINE__), Dummy); \
struct name : public EditorModule<name, geode::utils::string::ConstexprString(#name)>

#define $globalModule(name) \
GEODE_CONCAT(GEODE_CONCAT(name, __LINE__), Dummy); \
struct name : public GlobalModule<name, geode::utils::string::ConstexprString(#name)>
