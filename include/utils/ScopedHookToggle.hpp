#pragma once

#include <Geode/loader/Hook.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/utils/string.hpp>
#include "utils/Mod.hpp"

namespace tinker::utils {

template<geode::utils::string::ConstexprString ID, class Module = void>
class ScopedHookToggle {
public:
    ScopedHookToggle(const ScopedHookToggle&) = delete;
    ScopedHookToggle& operator=(const ScopedHookToggle&) = delete;
    ScopedHookToggle(ScopedHookToggle&&) = default;
    ScopedHookToggle& operator=(ScopedHookToggle&&) = default;

    template<typename... Hooks>
    requires (std::convertible_to<Hooks, geode::ZStringView> && ...)
    ScopedHookToggle(Hooks&&... hooks) {
        auto mod = tinker::utils::mod::getMod<ID>();
        if (!mod) return;

        bool shouldToggle = true;
        if constexpr (!std::is_void_v<Module>) {
            if (!Module::isEnabled()) {
                shouldToggle = false;
            }
        }

        const std::array<geode::ZStringView, sizeof...(Hooks)> hookNames{std::forward<Hooks>(hooks)...};

        for (geode::Hook* hook : mod->getHooks()) {
            if (!hook->isEnabled()) continue;
            if (std::ranges::find(hookNames, hook->getDisplayName()) != hookNames.end()) {
                m_hooks.push_back(hook);
                if (shouldToggle) {
                    (void) hook->disable();
                }
            }
        }
    }

    void toggle(bool state) {
        auto mod = tinker::utils::mod::getMod<ID>();
        if (!mod) return;

        bool enabled = false;

        for (auto hook : m_hooks) {
            (void) hook->toggle(!state);
        }
    }
    
    ~ScopedHookToggle() {
        for (auto hook : m_hooks) {
            (void) hook->enable();
        }
    }
private:
    std::vector<geode::Hook*> m_hooks;
};

}