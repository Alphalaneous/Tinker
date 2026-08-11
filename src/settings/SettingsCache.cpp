#include "settings/SettingsCache.hpp"

const StringMap<SettingsCache::SettingCategory>& SettingsCache::getSettings() {
    return m_settings;
}

const StringMap<std::shared_ptr<SettingsCache::SettingInfo>>& SettingsCache::getSettingsMap() {
    return m_settingsMap;
}

const std::vector<std::shared_ptr<SettingsCache::SettingInfo>>& SettingsCache::getSettingsList() {
    return m_settingsVec;
}

const std::vector<SettingsCache::SettingCategory*>& SettingsCache::getCategoryList() {
    return m_categoryVec;
}

void SettingsCache::loadSettings() {

    auto modjson = Mod::get()->getRuntimeInfo();
    auto& settings = modjson["settings"];
    SettingCategory* lastCategory;

    for (auto& [settingKey, v] : settings) {
        auto type = v["type"].asString().unwrapOr("unknown");
        auto displayName = v["name"].asString().unwrapOr(settingKey);

        if (type == "title") {
            lastCategory = &m_settings[settingKey];
            lastCategory->name = displayName;
            lastCategory->id = settingKey;
            m_categoryVec.push_back(lastCategory);
        }

        auto settingPtr = std::make_shared<SettingInfo>(
            settingKey,
            displayName,
            type,
            lastCategory,
            v.contains("enable-if")
        );

        lastCategory->settings.insert(settingPtr);
        m_settingsMap[settingKey] = settingPtr;
        m_settingsVec.push_back(settingPtr);
    }
}

$on_mod(Loaded) {
    SettingsCache::get()->loadSettings();
}