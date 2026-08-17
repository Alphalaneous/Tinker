#pragma once

#include "module/Module.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/AppDelegate.hpp>

using namespace alpha::prelude;

namespace tinker::ui {

class ObjectSelectContainer : public CCNode {
public:
    static ObjectSelectContainer* create(CCArray* objects);

    void shiftObject(bool forward);
    GameObject* getCurrentObject();
protected:

    bool init(CCArray* objects);

    int m_index;

    Ref<CCArray> m_objects;

    Ref<geode::NineSlice> m_objectsBG;
    Ref<AdvancedScrollLayer> m_scrollLayer;
    std::unordered_map<Ref<GameObject>, Ref<CCSprite>> m_objectSprites;

    CCSprite* m_selectDot;
};

class HoverObjectNode : public CCNode, public TouchDelegate {
public:
    static HoverObjectNode* create();
    bool hoveringObjects();
    void stopHover();

protected:

    bool init() override;
    bool mouseEntered(TouchEvent* touch) override;
    void mouseMoved(TouchEvent* touch) override;

    void onEnter() override;
    void onExit() override;

    void selectObject(GameObject* object);

    void onHoverObjects(const CCPoint& pos);

    void showObjectList();
    void removeObjectList();
    void shiftObject(bool forward);

    Ref<ObjectSelectContainer> m_activeSelectContainer;
    Ref<CCArray> m_lastObjects;
    CCPoint m_lastPos;
    bool m_active;
    bool m_modifierPressed;
    bool m_stopped;
};

}

class $module(BetterSelect) {

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void onEditor();
    bool hoveringObjects();
    void stopHover();

    tinker::ui::HoverObjectNode* m_hover;
};

class $modify(BSEditorUI, EditorUI) {
    $registerHooks(BetterSelect)

    void deselectAll();
    void keyDown(cocos2d::enumKeyCodes key, double timestamp);
};

class $modify(BSAppDelegate, AppDelegate) {
    $registerHooks(BetterSelect)

    void applicationDidEnterBackground();
};