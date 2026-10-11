#pragma once

#include <Geode/utils/ZStringView.hpp>
#include <string>
#include <vector>
#include "utils/Singleton.hpp"

class SettingsQueueHandler : public Singleton<SettingsQueueHandler> {
public:
    void addFeature(geode::ZStringView name);
    void addSetting(geode::ZStringView name);

    void queueShow();

    std::vector<std::string> m_features{};
    std::vector<std::string> m_settings{};
    bool m_queued = false;
};