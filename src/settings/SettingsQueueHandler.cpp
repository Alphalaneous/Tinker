#include "settings/SettingsQueueHandler.hpp"
#include <Geode/utils/StringBuffer.hpp>
#include "settings/SettingsCache.hpp"

using namespace geode::prelude;

void SettingsQueueHandler::addFeature(geode::ZStringView name) {
    m_features.push_back(name);
    queueShow();
}

void SettingsQueueHandler::addSetting(geode::ZStringView name) {
    m_settings.push_back(name);
    queueShow();
}

void SettingsQueueHandler::queueShow() {
    if (!m_queued) {
        geode::queueInMainThread([this] {
            geode::utils::StringBuffer<> content{};
            content.append("### <cy>The following settings require the editor to be restarted to take effect.</c>\n");
            
            if (!m_features.empty()) {
                content.append("\n---\n");
                content.append("### Features\n");

                for (const auto& v : m_features) {
                    auto& list = SettingsCache::get()->getSettingsMap();
                    auto settingIter = list.find(v);
                    if (settingIter == list.end()) continue;

                    auto setting = settingIter->second;
                    auto name = setting->name;
                    if (name == "Enabled") {
                        name = setting->category->name;
                    }
                    geode::utils::string::replaceIP(name, "Enable ", "");
                    content.append("- <c-dddddd>{}</c>\n", name);
                }
            }

            if (!m_settings.empty()) {
                content.append("\n---\n");
                content.append("### Settings\n");

                for (const auto& v : m_settings) {
                    auto& list = SettingsCache::get()->getSettingsMap();
                    auto settingIter = list.find(v);
                    if (settingIter == list.end()) continue;

                    auto setting = settingIter->second;

                    content.append("- <c-dddddd>{}</c> <c-888888>({})</c>\n", setting->name, setting->category->name);
                }
            }

            auto popup = geode::MDPopup::create("Notice", content.str(), "Okay");
            popup->show();

            m_queued = false;
            m_features.clear();
            m_settings.clear();
        });
    }
    m_queued = true;
}