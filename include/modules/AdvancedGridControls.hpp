#pragma once

#include <Geode/ui/SliderNode.hpp>
#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

namespace tinker::ui {

class SizeContainer : public CCNode {
public:
    static SizeContainer* create(ZStringView label, float value, float min, float max, float hardMin, float hardMax, bool compact, geode::Function<void(float size)> sizeCallback);

    void setEnabled(bool enabled);
    void setValue(float value);
    
protected:
    bool init(ZStringView label, float value, float min, float max, float hardMin, float hardMax, bool compact, geode::Function<void(float size)> sizeCallback);

    void updateValue();

    bool m_compact;
    geode::Function<void(float)> m_sizeCallback;

    float m_value;

    float m_min;
    float m_max;

    SliderNode* m_sizeSlider;
    geode::TextInput* m_sizeInput;
    bool m_skipSliderCallback;
};

class GridSizePopup : public geode::Popup {
public:
    static GridSizePopup* create();
protected:
    bool init();
    void updateMode();

    void setupSizePage();
    void setupOffsetPage();

    int m_page;

    geode::Label* m_titleLabel;

    geode::NineSlice* m_background;
    CCMenuItemSpriteExtra* m_clearBtn;
    CCMenuItemSpriteExtra* m_alignBtn;
    geode::Label* m_enabledLabel;
    CCMenuItemToggler* m_enabledCheckbox;
    SizeContainer* m_sizeContainer;
    SizeContainer* m_sizeXContainer;
    SizeContainer* m_sizeYContainer;

    SizeContainer* m_offsetXContainer;
    SizeContainer* m_offsetYContainer;
};

};

class $module(AdvancedGridControls) {

    void onEditor();
    void updateGrid();
    float getGridMultiplierX();
    float getGridMultiplierY();

    bool m_enabled;
    bool m_gridScale;
    bool m_separateXY;

    float m_gridSize;
    float m_gridX;
    float m_gridY;

    float m_offsetX;
    float m_offsetY;

    bool m_updatingGridSize;
};


class $modify(AGCEditorUI, EditorUI) {
    $registerHooks(AdvancedGridControls)

    struct Fields {
        bool m_regularLimited = false;
    };

    CCPoint offsetForKey(int id);
    GameObject* createObject(int objectID, CCPoint position);
    CCPoint getLimitedPosition(CCPoint position);
    void moveObject(GameObject* object, CCPoint offset);

    void updateGridNodeSize();
};