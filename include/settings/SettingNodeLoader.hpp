#pragma once

#include <Geode/loader/SettingV3.hpp>
#include <Geode/utils/string.hpp>
#include <memory>
#include "settings/SettingNodeRegistry.hpp"

using namespace geode::prelude;

namespace tinker::settings {

template <class T, geode::utils::string::ConstexprString Type>
struct SettingNodeLoader {
    SettingNodeLoader() {
        SettingNodeRegistry::get()->m_settingNodes[Type.data()] = [] (float width, std::shared_ptr<SettingV3> setting) {
            return T::create(width, setting);
        };
    }
};

}