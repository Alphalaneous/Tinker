#pragma once


#include <Geode/binding/EditorPauseLayer.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/SetGroupIDLayer.hpp>
#include <Geode/cocos/cocoa/CCArray.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/Keyboard.hpp>

struct EditorEnterEvent final : geode::Event<EditorEnterEvent, bool(EditorUI* editorUI)> {
    using Event::Event;
};

struct EditorExitEvent final : geode::Event<EditorExitEvent, bool()> {
    using Event::Event;
};

struct ShowUIEvent final : geode::Event<ShowUIEvent, bool(bool show)> {
    using Event::Event;
};

struct ModifierEvent final : geode::Event<ModifierEvent, bool(geode::KeyboardModifier modifier, geode::KeyboardModifier lastModifier)> {
    using Event::Event;
};

struct UpdateButtonsEvent final : geode::Event<UpdateButtonsEvent, bool()> {
    using Event::Event;
};

struct SetGroupIDLayerOpenedEvent final : geode::Event<SetGroupIDLayerOpenedEvent, bool(SetGroupIDLayer* setGroupIDLayer, GameObject* obj, cocos2d::CCArray* objs)> {
    using Event::Event;
};

struct EditorPausedEvent final : geode::Event<EditorPausedEvent, bool(EditorPauseLayer* editorPauseLayer)> {
    using Event::Event;
};

struct EditorUnpausedEvent final : geode::Event<EditorUnpausedEvent, bool()> {
    using Event::Event;
};

struct LevelSavedEvent final : geode::Event<LevelSavedEvent, bool()> {
    using Event::Event;
};

struct LevelTypeChangedEvent final : geode::Event<LevelTypeChangedEvent, bool(bool isPlatformer)> {
    using Event::Event;
};

struct ObjectChangeEvent final : geode::Event<ObjectChangeEvent, bool(float lastObjectX)> {
    using Event::Event;
};

struct SetupCreateMenuEvent final : geode::Event<SetupCreateMenuEvent, bool()> {
    using Event::Event;
};

struct EditorRotationEvent final : geode::Event<EditorRotationEvent, bool(float rotation)> {
    using Event::Event;
};

struct EditorZoomEvent final : geode::Event<EditorZoomEvent, bool(float zoom)> {
    using Event::Event;
};

struct EditorMoveEvent final : geode::Event<EditorMoveEvent, bool(cocos2d::CCPoint position)> {
    using Event::Event;
};

struct UpdateObjectLabel final : geode::Event<UpdateObjectLabel, bool()> {
    using Event::Event;
};

struct ObjectMovedEvent final : geode::Event<ObjectMovedEvent, bool()> {
    using Event::Event;
};

struct ActiveObjectsChangedEvent final : geode::Event<ActiveObjectsChangedEvent, bool(int count)> {
    using Event::Event;
};

struct AlertsActiveEvent final : geode::Event<AlertsActiveEvent, bool()> {
    using Event::Event;
};

struct PreUIScaleUpdated final : geode::Event<PreUIScaleUpdated, bool(float scale, bool scaleToolbars, bool fullReload)> {
    using Event::Event;
};

struct UIScaleUpdated final : geode::Event<UIScaleUpdated, bool(float scale, bool scaleToolbars, bool fullReload)> {
    using Event::Event;
};

struct PauseUIScaleUpdated final : geode::Event<PauseUIScaleUpdated, bool(EditorPauseLayer* pauseLayer, float scale)> {
    using Event::Event;
};

struct LostFocusEvent final : geode::Event<LostFocusEvent, bool()> {
    using Event::Event;
};