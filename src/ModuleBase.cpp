#include "module/ModuleBase.hpp"
#include "MainHooks.hpp"

ListenerHandle* ModuleBase::addEventListenerInternal(std::string id, ListenerHandle handle) {
    auto wrap = std::make_unique<ListenerHandle>(std::move(handle));
    auto ret = wrap.get();
    m_eventListeners.emplace(std::move(id), std::move(wrap));
    return ret;
}

void ModuleBase::removeEventListener(std::string_view id) {
    auto range = m_eventListeners.equal_range(id);
    m_eventListeners.erase(range.first, range.second);
}

void ModuleBase::removeEventListener(ListenerHandle* handle) {
    std::erase_if(m_eventListeners, [=](auto& l) {
        return l.second.get() == handle;
    });
}

ListenerHandle* ModuleBase::getEventListener(std::string_view id) {
    auto it = m_eventListeners.find(id);
    return it != m_eventListeners.end() ? it->second.get() : nullptr;
}

size_t ModuleBase::getEventListenerCount() {
    return m_eventListeners.size();
}

EditorUI* ModuleBase::getEditor() {
    return MainEditorUI::get();
}

LevelEditorLayer* ModuleBase::getEditorLayer() {
    auto editorUI = MainEditorUI::get();
    if (editorUI) {
        return editorUI->m_editorLayer;
    }
    return nullptr;
}