#pragma once

#include <Geode/utils/StringMap.hpp>
#include <Geode/loader/Event.hpp>
#include <memory>

struct ModuleBase {
    virtual ~ModuleBase() = default;

    template <class Event, class Callback>
    geode::ListenerHandle* addEventListener(std::string_view id, Event const& event, Callback&& callback, int priority = 0) {
        auto handle = event.listen(std::forward<Callback>(callback), priority);
        return addEventListenerInternal(std::string(id), std::move(handle));
    }

    template <class Event, class Callback>
    geode::ListenerHandle* addEventListener(Event const& event, Callback&& callback, int priority = 0) {
        return addEventListener("", event, std::forward<Callback>(callback), priority);
    }

    void removeEventListener(std::string_view id);
    void removeEventListener(geode::ListenerHandle* handle);
    geode::ListenerHandle* getEventListener(std::string_view id);
    size_t getEventListenerCount();

    EditorUI* getEditor();
    LevelEditorLayer* getEditorLayer();

protected:
    geode::utils::StringMultimap<std::unique_ptr<geode::ListenerHandle>> m_eventListeners{};
    
    geode::ListenerHandle* addEventListenerInternal(std::string id, geode::ListenerHandle handle);
};