#include "SettingsQueueHandler.hpp"

void SettingsQueueHandler::addFeature(const std::string& name) {
    m_features.push_back(std::move(name));
    queueShow();
}

void SettingsQueueHandler::addSetting(const std::string& name) {
    m_settings.push_back(std::move(name));
    queueShow();
}

void SettingsQueueHandler::queueShow() {
    if (!m_queued) {
        queueInMainThread([this] {

            StringBuffer<> content;
            content.append("### <cy>The following settings require the editor to be restarted to take effect.</c>\n");
            
            if (!m_features.empty()) {
                content.append("\n---\n");
                content.append("### Features\n");

                for (const auto& v : m_features) {
                    auto setting = Mod::get()->getSetting(v);
                    if (!setting) continue;
                    auto name = setting->getDisplayName();
                    if (name == "Enabled") {
                        auto split = utils::string::split(v, "-");
                        auto titleName = split[0] + "-title";
                        auto titleSetting = Mod::get()->getSetting(titleName);
                        name = "<unknown>";
                        if (titleSetting) {
                            name = titleSetting->getDisplayName();
                        }
                    }
                    utils::string::replaceIP(name, "Enable ", "");
                    content.append("- <c-dddddd>{}</c>\n", name);
                }
            }

            if (!m_settings.empty()) {
                content.append("\n---\n");
                content.append("### Settings\n");

                for (const auto& v : m_settings) {
                    auto setting = Mod::get()->getSetting(v);
                    if (!setting) continue;
                    auto split = utils::string::split(v, "-");
                    auto titleName = split[0] + "-title";
                    auto titleSetting = Mod::get()->getSetting(titleName);
                    std::string featureName = "unknown";
                    if (titleSetting) {
                        featureName = titleSetting->getDisplayName();
                    }
                    content.append("- <c-dddddd>{}</c> <c-888888>({})</c>\n", setting->getDisplayName(), featureName);
                }
            }

            auto popup = MDPopup::create("Notice", content.str(), "Okay");
            popup->show();

            m_queued = false;
            m_features.clear();
            m_settings.clear();
        });
    }
    m_queued = true;
}