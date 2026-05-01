#include "SettingsCache.hpp"

const StringMap<SettingsCache::SettingCategory>& SettingsCache::getSettings() {
    return m_settings;
}

const StringMap<std::shared_ptr<SettingsCache::SettingInfo>>& SettingsCache::getSettingsList() {
    return m_settingsList;
}

void SettingsCache::loadSettings() {

    auto modjson = Mod::get()->getRuntimeInfo();
    auto& settings = modjson["settings"];

    for (auto& [settingKey, v] : settings) {
        auto type = v["type"].asString().unwrapOr("unknown");
        auto displayName = v["name"].asString().unwrapOr(settingKey);

        if (type == "title") {
            auto& category = m_settings[settingKey];
            category.name = displayName;
            category.id = settingKey;
            continue;
        }

        auto split = utils::string::split(settingKey, "-");
        auto titleId = split[0] + "-title";

        auto& category = m_settings[titleId];

        auto settingPtr = std::make_shared<SettingInfo>(
            settingKey,
            displayName,
            &category,
            v.contains("enable-if")
        );

        category.settings.insert(settingPtr);
        m_settingsList[settingKey] = settingPtr;
    }
}

$on_mod(Loaded) {
    SettingsCache::get()->loadSettings();
}