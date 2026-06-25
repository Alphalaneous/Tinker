#pragma once

#include <Geode/Geode.hpp>
#include "utils/Singleton.hpp"

using namespace geode::prelude;

class SettingsQueueHandler : public Singleton<SettingsQueueHandler> {
public:
    void addFeature(const std::string& name);
    void addSetting(const std::string& name);

    void queueShow();

    std::vector<std::string> m_features;
    std::vector<std::string> m_settings;
    bool m_queued;
};