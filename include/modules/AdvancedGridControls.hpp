#pragma once

#include <Geode/binding/EditorCommandsPopup.hpp>
#include <Geode/ui/SliderNode.hpp>
#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

namespace tinker::ui {

class SizeContainer : public cocos2d::CCNode {
public:
    static SizeContainer* create(geode::ZStringView label, float value, float min, float max, float hardMin, float hardMax, bool compact, geode::Function<void(float size)> sizeCallback);

    void setEnabled(bool enabled);
    void setValue(float value);
    
protected:
    bool init(geode::ZStringView label, float value, float min, float max, float hardMin, float hardMax, bool compact, geode::Function<void(float size)> sizeCallback);

    void updateValue();

    bool m_compact = false;
    geode::Function<void(float)> m_sizeCallback = nullptr;

    float m_value = 0.f;

    float m_min = 0.f;
    float m_max = 0.f;

    geode::SliderNode* m_sizeSlider = nullptr;
    geode::TextInput* m_sizeInput = nullptr;
    bool m_skipSliderCallback = false;
};

class AdvancedGridSettingsLayer : public EditorWindowPopup {
public:
    static AdvancedGridSettingsLayer* create();
    void updateValues();

    virtual void onBuildWindowContent();
    virtual void onClose(cocos2d::CCObject* sender);
protected:
    bool init();
    void updateMode();

    void setupSizePage();
    void setupOffsetPage();

    int m_page = 0;

    cocos2d::CCNode* m_contentContainer = nullptr;
    cocos2d::CCMenu* m_contentMenu = nullptr;

    geode::Label* m_titleLabel = nullptr;

    geode::NineSlice* m_background = nullptr;
    CCMenuItemSpriteExtra* m_clearBtn = nullptr;
    CCMenuItemSpriteExtra* m_alignBtn = nullptr;
    geode::Label* m_enabledLabel = nullptr;
    CCMenuItemToggler* m_enabledCheckbox = nullptr;
    CCMenuItemToggler* m_xyModeCheckbox = nullptr;
    CCMenuItemToggler* m_scaleCheckbox = nullptr;

    SizeContainer* m_sizeContainer = nullptr;
    SizeContainer* m_sizeXContainer = nullptr;
    SizeContainer* m_sizeYContainer = nullptr;

    SizeContainer* m_offsetXContainer = nullptr;
    SizeContainer* m_offsetYContainer = nullptr;
};

};

class $module(AdvancedGridControls) {
    AdvancedGridControls();
    ~AdvancedGridControls();

    bool onToggled(bool state);

    void onEditor();
    void updateGrid();
    void updateWindow();
    float getGridMultiplierX();
    float getGridMultiplierY();

    bool m_gridScale = false;
    bool m_separateXY = false;

    float m_gridSize = 0.f;
    float m_gridX = 0.f;
    float m_gridY = 0.f;

    float m_offsetX = 0.f;
    float m_offsetY = 0.f;

    bool m_updatingGridSize = false;
    bool m_getOrigOffset = false;
    geode::Hook* m_offsetForKeyHook = nullptr;
};

cocos2d::CCPoint offsetForKey_h(int id);

class $modify(AGCEditorUI, EditorUI) {
    $registerHooks(AdvancedGridControls)

    struct Fields {
        bool m_regularLimited = false;
    };

    void toggleWindow(int windowID);
    bool handleAction(int action, bool down, double timestamp);
    void cycleCustomGridPreset(bool p0);
    cocos2d::CCPoint moveForCommand(EditCommand command);

    GameObject* onCreateObject(int id);

    cocos2d::CCPoint getLimitedPosition(cocos2d::CCPoint position);
    void moveObject(GameObject* object, cocos2d::CCPoint offset);

    float getGridMultiplier();
    void updateGridNodeSize();
};