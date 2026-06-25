#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct ModuleBase {
    geode::Function<bool()> isEnabled;

    virtual void onCreated() {}
    virtual bool onSettingChanged(std::string_view key, const matjson::Value& value) { return false; }
    virtual bool onToggled(bool state) { return false; }

    template <class Event, class Callback>
    ListenerHandle* addEventListener(std::string_view id, Event const& event, Callback&& callback, int priority = 0) {
        auto handle = event.listen(std::forward<Callback>(callback), priority);
        return addEventListenerInternal(std::string(id), std::move(handle));
    }

    template <class Event, class Callback>
    ListenerHandle* addEventListener(Event const& event, Callback&& callback, int priority = 0) {
        return addEventListener("", event, std::forward<Callback>(callback), priority);
    }

    void removeEventListener(std::string_view id);
    void removeEventListener(ListenerHandle* handle);
    ListenerHandle* getEventListener(std::string_view id);
    size_t getEventListenerCount();

protected:
    StringMultimap<std::unique_ptr<ListenerHandle>> m_eventListeners;

    ListenerHandle* addEventListenerInternal(std::string id, ListenerHandle handle);
};

struct EditorModuleBase : public ModuleBase {
    LevelEditorLayer* m_editorLayer = nullptr;
    EditorUI* m_editorUI = nullptr;
    EditorPauseLayer* m_pauseLayer = nullptr;

    virtual ~EditorModuleBase() = default;

    virtual void onEditor() {}
};

struct GlobalModuleBase : public ModuleBase {
};