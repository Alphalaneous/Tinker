#pragma once

#include <Geode/Geode.hpp>
#include <memory>
#include "Geode/loader/SettingV3.hpp"
#include "utils/Singleton.hpp"

using namespace geode::prelude;

namespace tinker::settings {

class SettingNodeBase;

struct SettingNodeRegistry : public Singleton<SettingNodeRegistry> {
    StringMap<std::function<SettingNodeBase*(float, std::shared_ptr<SettingV3>)>> m_settingNodes;

    static SettingNodeBase* create(ZStringView type, float width, std::shared_ptr<SettingV3> setting) {
        auto& nodes = SettingNodeRegistry::get()->m_settingNodes;
        auto iter = nodes.find(type);
        if (iter == nodes.end()) {
            log::warn("Unknown setting type: \"{}\" for \"{}\"", type, setting->getKey());
            return SettingNodeRegistry::get()->m_settingNodes["mirror"](width, setting);
        }

        log::debug("Loaded setting type: \"{}\" for \"{}\"", type, setting->getKey());
        return iter->second(width, setting);
    }
};

}