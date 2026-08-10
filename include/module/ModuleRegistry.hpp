#pragma once

#include <Geode/Geode.hpp>
#include "utils/Singleton.hpp"
#include "module/ModuleData.hpp"
#include "module/ModuleBase.hpp"

using namespace geode::prelude;

struct ModuleRegistry : public Singleton<ModuleRegistry> {
    ModuleData& getData(ZStringView name) {
        return m_data[name];
    }
    StringMap<std::function<std::shared_ptr<ModuleBase>()>>& getModules() {
        return m_modules;
    }

    StringMap<std::function<std::shared_ptr<ModuleBase>()>> m_modules;
    StringMap<ModuleData> m_data;
};