#include "module/ModuleData.hpp"

class ModuleData::Impl final {
public:
    std::vector<std::shared_ptr<Hook>> m_hooks;
    geode::Function<void()> m_onEditor;
    geode::Function<bool()> m_moduleEnabled;

    bool m_isGlobal;
};

ModuleData::ModuleData() : m_impl(std::make_unique<Impl>()) {}

ModuleData::~ModuleData() {}

std::vector<std::shared_ptr<Hook>>& ModuleData::getHooks() {
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