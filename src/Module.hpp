#pragma once

#include "ModuleRegistry.hpp"
#include <Geode/Geode.hpp>
#include "Utils.hpp"

using namespace geode::prelude;

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

public:
    static std::shared_ptr<T> create() {
        auto ret = std::make_shared<T>();

        static constexpr auto enabledKey = tinker::utils::concat<Name, "-enabled">();

        bool moduleEnabled = isEnabled();

        if constexpr (std::is_same_v<Base, GlobalModuleBase>) {

            listenForSettingChanges<bool>(enabledKey.data(), [ret] (bool enabled) {
                bool moduleEnabled = isEnabled();

                for (auto hook : ModuleRegistry<Base>::get()->m_hooks[getName()]) {
                    (void) hook->toggle(moduleEnabled);
                }
                
                if (moduleEnabled) $instance = ret;
                else $instance.reset();
            });

            for (const auto& hook : ModuleRegistry<Base>::get()->m_hooks[getName()]) {
                (void) hook->toggle(moduleEnabled);
            }

            if (moduleEnabled) $instance = ret;
            else $instance.reset();

            return ret;
        }

        if (!moduleEnabled) return nullptr;

        $instance = ret;

        for (const auto& hook : ModuleRegistry<Base>::get()->m_hooks[getName()]) {
            (void) hook->enable();
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
        return settingEnabled && getSetting<bool, "enabled">();
    }

    template <class S, geode::utils::string::ConstexprString key>
    static const S& getSetting() {
        static constexpr auto fullKey = tinker::utils::concat<Name, "-", key>();
        static auto setting = Mod::get()->getSettingValue<S>(fullKey.data());
        static auto listener = listenForSettingChanges<S>(fullKey.data(), [] (S value) {
            setting = std::move(value);
        });
        return setting;
    }
};

struct EditorModuleBase {
    LevelEditorLayer* m_editorLayer = nullptr;
    EditorUI* m_editorUI = nullptr;

    virtual ~EditorModuleBase() = default;

    virtual void onEditor() {}
    virtual void onSave() {}
    virtual void onUpdateButtons() {}
    virtual void onSetGroupIDLayer(SetGroupIDLayer* setGroupIDLayer, GameObject* obj, cocos2d::CCArray* objs) {}
    virtual void onEditorPauseLayer(EditorPauseLayer* editorPauseLayer) {}
    virtual void onObjectChange(float lastObjectX) {}
    virtual void onGameTypeChange(bool isPlatformer) {}
};

struct GlobalModuleBase {
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
