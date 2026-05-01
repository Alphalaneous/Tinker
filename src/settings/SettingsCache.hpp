#pragma once

#include "../Utils.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class SettingsCache : public tinker::utils::Singleton<SettingsCache> {
public:
    struct SettingCategory;

    struct SettingInfo {
        std::string id;
        std::string name;
        SettingCategory* category;
        bool hasEnableIf = false;
    };

    struct SettingCategory {
        std::string name;
        std::string id;
        std::set<std::shared_ptr<SettingInfo>> settings;
    };

    const StringMap<SettingCategory>& getSettings();
    const StringMap<std::shared_ptr<SettingInfo>>& getSettingsList();

    void loadSettings();

protected:
    StringMap<SettingCategory> m_settings;
    StringMap<std::shared_ptr<SettingInfo>> m_settingsList;
};