#include "module/ModuleData.hpp"

using namespace geode::prelude;

class ModuleData::Impl final {
public:
    std::vector<geode::Hook*> m_hooks{};
    geode::Function<void()> m_onEditor = nullptr;
    geode::Function<bool()> m_moduleEnabled = nullptr;

    bool m_isGlobal = false;
};

ModuleData::ModuleData() : m_impl(std::make_unique<Impl>()) {}

ModuleData::~ModuleData() {}

std::vector<geode::Hook*>& ModuleData::getHooks() {
    return m_impl->m_hooks;
}

void ModuleData::setOnEditorCallback(geode::Function<void()> callback) {
    m_impl->m_onEditor = std::move(callback);
}

void ModuleData::setModuleEnabledCallback(geode::Function<bool()> callback) {
    m_impl->m_moduleEnabled = std::move(callback);
}

void ModuleData::onEditor() {
    if (m_impl->m_onEditor) {
        m_impl->m_onEditor();
    }
}

bool ModuleData::moduleEnabled() {
    if (m_impl->m_moduleEnabled) {
        return m_impl->m_moduleEnabled();
    }
    return false;
}

void ModuleData::setGlobal(bool global) {
    m_impl->m_isGlobal = global;
}

bool ModuleData::isGlobal() {
    return m_impl->m_isGlobal;
}