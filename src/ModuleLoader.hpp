#pragma once

#include <Geode/Geode.hpp>
#include "ModuleRegistry.hpp"

using namespace geode::prelude;

template <class T, class Base>
struct ModuleLoader {
    ModuleLoader() {
        ModuleRegistry<Base>::get()->m_modules.push_back([] {
            auto ret = T::create();
            ret->onCreated();
            return ret;
        });
    }
};