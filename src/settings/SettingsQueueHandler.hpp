#pragma once

#include <Geode/Geode.hpp>
#include "../Utils.hpp"

using namespace geode::prelude;

class SettingsQueueHandler : public tinker::utils::Singleton<SettingsQueueHandler> {
public:
    void addFeature(const std::string& name);
    void addSetting(const std::string& name);

    void queueShow();

    std::vector<std::string> m_features;
    std::vector<std::string> m_settings;
    bool m_queued;
};