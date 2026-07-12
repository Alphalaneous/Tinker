#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

struct EditorEnterEvent final : Event<EditorEnterEvent, bool(EditorUI* editorUI)> {
    using Event::Event;
};

struct EditorExitEvent final : Event<EditorExitEvent, bool()> {
    using Event::Event;
};

struct ShowUIEvent final : Event<ShowUIEvent, bool(bool show)> {
    using Event::Event;
};

struct ModifierEvent final : Event<ModifierEvent, bool(KeyboardModifier modifier, KeyboardModifier lastModifier)> {
    using Event::Event;
};

struct UpdateButtonsEvent final : Event<UpdateButtonsEvent, bool()> {
    using Event::Event;
};

struct SetGroupIDLayerOpenedEvent final : Event<SetGroupIDLayerOpenedEvent, bool(SetGroupIDLayer* setGroupIDLayer, GameObject* obj, cocos2d::CCArray* objs)> {
    using Event::Event;
};

struct EditorPausedEvent final : Event<EditorPausedEvent, bool(EditorPauseLayer* editorPauseLayer)> {
    using Event::Event;
};

struct EditorUnpausedEvent final : Event<EditorUnpausedEvent, bool()> {
    using Event::Event;
};

struct LevelSavedEvent final : Event<LevelSavedEvent, bool()> {
    using Event::Event;
};

struct LevelTypeChangedEvent final : Event<LevelTypeChangedEvent, bool(bool isPlatformer)> {
    using Event::Event;
};

struct ObjectChangeEvent final : Event<ObjectChangeEvent, bool(float lastObjectX)> {
    using Event::Event;
};

struct SetupCreateMenuEvent final : Event<SetupCreateMenuEvent, bool()> {
    using Event::Event;
};

struct EditorRotationEvent final : Event<EditorRotationEvent, bool(float rotation)> {
    using Event::Event;
};

struct EditorZoomEvent final : Event<EditorZoomEvent, bool(float zoom)> {
    using Event::Event;
};