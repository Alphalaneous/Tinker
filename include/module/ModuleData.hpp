#pragma once

#include <Geode/loader/Hook.hpp>
#include <Geode/utils/function.hpp>
#include <memory>
#include <vector>

class ModuleData {
public:
    ModuleData();
    ~ModuleData();

    std::vector<geode::Hook*>& getHooks();
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