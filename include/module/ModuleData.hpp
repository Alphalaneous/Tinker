#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ModuleData {
public:
    ModuleData();
    ~ModuleData();

    std::vector<std::shared_ptr<Hook>>& getHooks();
    void setOnEditorCallback(geode::Function<void()> callback);
    void setModuleEnabledCallback(geode::Function<bool()> callback);

    void setGlobal(bool global);
    bool isGlobal();

    void onEditor();
    bool moduleEnabled();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};