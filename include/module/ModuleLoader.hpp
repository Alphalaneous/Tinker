#pragma once

#include <Geode/Geode.hpp>
#include "module/ModuleRegistry.hpp"

using namespace geode::prelude;

template <class T>
struct ModuleLoader {
    ModuleLoader() {
        ModuleRegistry::get()->m_modules[T::getName()] = ([] {
            return T::create();
        });

        auto& data = ModuleRegistry::get()->getData(T::getName());
        data.setGlobal(T::isGlobal());

        if constexpr (requires { T::get()->onEditor(); }) {
            data.setOnEditorCallback([] {
                T::get()->onEditor();
            });
        }

        data.setModuleEnabledCallback([] {
            return T::get()->isEnabled();
        });
    }
};