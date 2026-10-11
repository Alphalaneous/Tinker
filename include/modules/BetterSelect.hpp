#pragma once

#include "module/Module.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/AppDelegate.hpp>

using namespace alpha::prelude;

namespace tinker::ui {

class ObjectSelectContainer : public cocos2d::CCNode {
public:
    static ObjectSelectContainer* create(cocos2d::CCArray* objects);

    void shiftObject(bool forward);
    GameObject* getCurrentObject();

    void refreshSelectedObjects();
    void showInfo();
protected:

    bool init(cocos2d::CCArray* objects);

    int m_index = 0;

    geode::Ref<cocos2d::CCArray> m_objects = nullptr;

    geode::NineSlice* m_objectsBG = nullptr;
    AdvancedScrollLayer* m_scrollLayer = nullptr;
    std::unordered_map<geode::Ref<GameObject>, geode::Ref<cocos2d::CCSprite>> m_objectSprites{};

    geode::NineSlice* m_infoBG = nullptr;

    cocos2d::CCSprite* m_selectDot = nullptr;
};

class HoverObjectNode : public cocos2d::CCNode, public TouchDelegate {
public:
    static HoverObjectNode* create();
    bool hoveringObjects();
    void stopHover();

    static cocos2d::CCPoint rotatedPos(const cocos2d::CCPoint& pos);
protected:

    bool init() override;
    bool mouseEntered(TouchEvent* touch) override;
    void mouseMoved(TouchEvent* touch) override;

    void onEnter() override;
    void onExit() override;

    void quickSelect();
    void selectObject(GameObject* object, bool toggleSelect = false);

    void onHoverObjects(const cocos2d::CCPoint& pos);

    void showObjectList();
    void removeObjectList();
    void shiftObject(bool forward);

    geode::Ref<ObjectSelectContainer> m_activeSelectContainer = nullptr;
    geode::Ref<cocos2d::CCArray> m_lastObjects = nullptr;
    cocos2d::CCPoint m_lastPos{};
    bool m_active = false;
    bool m_modifierPressed = false;
    bool m_stopped = false;
    bool m_skipStop = false;
};

}

class $module(BetterSelect) {

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void onEditor();
    bool hoveringObjects();
    void stopHover();

    tinker::ui::HoverObjectNode* m_hover = nullptr;
    cocos2d::CCNode* m_selectPickerContainer = nullptr;
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