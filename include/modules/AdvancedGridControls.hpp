#pragma once

#include <Geode/ui/SliderNode.hpp>
#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

/*namespace tinker::ui {

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

class GridSizePopup : public geode::Popup {
public:
    static GridSizePopup* create();
protected:
    bool init();
    void updateMode();

    void setupSizePage();
    void setupOffsetPage();

    int m_page = 0;

    geode::Label* m_titleLabel = nullptr;

    geode::NineSlice* m_background = nullptr;
    CCMenuItemSpriteExtra* m_clearBtn = nullptr;
    CCMenuItemSpriteExtra* m_alignBtn = nullptr;
    geode::Label* m_enabledLabel = nullptr;
    CCMenuItemToggler* m_enabledCheckbox = nullptr;
    SizeContainer* m_sizeContainer = nullptr;
    SizeContainer* m_sizeXContainer = nullptr;
    SizeContainer* m_sizeYContainer = nullptr;

    SizeContainer* m_offsetXContainer = nullptr;
    SizeContainer* m_offsetYContainer = nullptr;
};

};

class $module(AdvancedGridControls) {

    void onEditor();
    void updateGrid();
    float getGridMultiplierX();
    float getGridMultiplierY();

    bool m_enabled = false;
    bool m_gridScale = false;
    bool m_separateXY = false;

    float m_gridSize = 0.f;
    float m_gridX = 0.f;
    float m_gridY = 0.f;

    float m_offsetX = 0.f;
    float m_offsetY = 0.f;

    bool m_updatingGridSize = false;
};


class $modify(AGCEditorUI, EditorUI) {
    $registerHooks(AdvancedGridControls)

    struct Fields {
        bool m_regularLimited = false;
    };

    cocos2d::CCPoint offsetForKey(int id);
    GameObject* createObject(int objectID, cocos2d::CCPoint position);
    cocos2d::CCPoint getLimitedPosition(cocos2d::CCPoint position);
    void moveObject(GameObject* object, cocos2d::CCPoint offset);

    void updateGridNodeSize();
};*/