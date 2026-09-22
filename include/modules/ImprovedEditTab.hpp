#pragma once

#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditButtonBar.hpp>

namespace tinker::ui {

class ButtonContainer : public geode::NineSlice, public alpha::dispatcher::TouchDelegate {
public:
    static ButtonContainer* create();

    void hide();
    void show();
    void setContentSize(cocos2d::CCSize const& size) override;

protected:
    bool init() override;

    void onEnter() override;
    void onExit() override;

	bool clickBegan(alpha::dispatcher::TouchEvent* touch) override;

    bool m_active;
    geode::Button* m_showButton = nullptr;
    Ref<CCAction> m_buttonAction = nullptr;
    Ref<CCAction> m_moveAction = nullptr;

    static constexpr float Offset = 50.f;
};

class EditControls : public CCLayer {
public:
 // I eventually want to add an API
    enum class Direction {
        Up,
        Down,
        Left,
        Right
    };

    using OnDirection = geode::Function<void(Direction direction)>;
    using CreateSprite = geode::Function<CCSprite*(Direction direction)>;

    struct MoveControl {
        std::string id;
        std::string label;
        float dist;
        CreateSprite createSprite = nullptr;
        OnDirection onDirection = nullptr; 
    };

    static EditControls* create(CCArray* buttons, const CCSize& size);
    bool controlActive();
    float getCurrentDist();
    Direction getCurrentDirection();

    void enableButton(ZStringView ID, bool enable);
    void setButtonBG(ZStringView ID, ZStringView bgID);
    void updateButtons();
    
protected:
    bool init(CCArray* buttons, const CCSize& size);

    geode::Button* createGButton(ZStringView sprite, ZStringView ID, float scale, geode::Function<void(geode::Button* sender)> callback, bool withBG);
    CCMenuItemSpriteExtra* createButton(ZStringView sprite, ZStringView ID, float scale, const CCPoint& offset, geode::Function<void(CCMenuItemSpriteExtra* sender)> callback, bool repeat, bool scaleBG);
    CCMenuItemSpriteExtra* createArrowButton(ZStringView sprite, ZStringView ID, Direction direction);

    std::vector<MoveControl> m_controls;
    StringMap<CCMenuItemSpriteExtra*> m_buttons;
    
    float m_moveDist = 30.f;
    float m_moveMod = 1.f;

    float m_curDist;
    bool m_active;
    Direction m_currentDirection;
    ZStringView m_activeID;
    CCNode* m_valueContainer;

    ButtonContainer* m_buttonContainer;
};

}

class $module(ImprovedEditTab) {
    bool onToggled(bool state);

    void onEditor();
    float getOffset();

    bool controlActive();
    float getCurrentDist();
    tinker::ui::EditControls::Direction getCurrentDirection();

    void updateButtons();

    Ref<tinker::ui::EditControls> m_customEditContainer;
};

class $modify(IETEditorUI, EditorUI) {
    $registerHooks(ImprovedEditTab)

    cocos2d::CCPoint moveForCommand(EditCommand command);
    void updateButtons();
};

class $modify(IETEditButtonBar, EditButtonBar) {
    $registerHooks(ImprovedEditTab, true)

    void removeOldButtons(cocos2d::CCArray* objects);
    void loadFromItems(cocos2d::CCArray* objects, int cols, int rows, bool keepPage);

    static void _onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("EditButtonBar::loadFromItems", "alphalaneous.editortab_api");
    }
};