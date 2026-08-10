#include "modules/CenteredObjectButtons.hpp"

CreateMenuItem* COBCreateMenuItem::create(cocos2d::CCNode* normal, cocos2d::CCNode* selected, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector) {
    auto ret = CreateMenuItem::create(normal, selected, target, selector);
    if (!ret) return nullptr;

    if (auto buttonSprite = typeinfo_cast<ButtonSprite*>(normal)) {
        buttonSprite->updateSpriteOffset({-0.25f, -1.5f});
    }

    return ret;
}