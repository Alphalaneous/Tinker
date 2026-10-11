#pragma once

#include <Geode/utils/StringMap.hpp>
#include <Geode/utils/ZStringView.hpp>
#include "utils/Singleton.hpp"
#include "module/ModuleData.hpp"
#include "module/ModuleBase.hpp"

struct ModuleRegistry : public Singleton<ModuleRegistry> {
    ModuleData& getData(geode::ZStringView name) {
        return m_data[name];
    }
    geode::utils::StringMap<std::function<std::shared_ptr<ModuleBase>()>>& getModules() {
        return m_modules;
    }

    geode::utils::StringMap<std::function<std::shared_ptr<ModuleBase>()>> m_modules{};
    geode::utils::StringMap<ModuleData> m_data{};
};