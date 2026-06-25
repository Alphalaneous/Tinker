#pragma once

#include <Geode/Geode.hpp>
#include "utils/Singleton.hpp"

using namespace geode::prelude;

template <class T>
struct ModuleRegistry : public Singleton<ModuleRegistry<T>> {
    std::vector<std::function<std::shared_ptr<T>()>> m_modules;
    StringMap<std::vector<std::shared_ptr<Hook>>> m_hooks;
};