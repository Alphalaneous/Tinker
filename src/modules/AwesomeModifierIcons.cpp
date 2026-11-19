#include "modules/AwesomeModifierIcons.hpp"
#include "MainHooks.hpp"
#include "utils/Constants.hpp"

using namespace geode::prelude;

using namespace tinker::constants::objects;

std::unordered_map<int, std::string> AwesomeModifierIcons::s_textureMap = {
    {DragModifier, "d_block.png"_spr},
    {JumpModifier, "j_block.png"_spr},
    {StopModifier, "s_block.png"_spr},
    {HeadModifier, "h_block.png"_spr},
    {FlipModifier, "f_block.png"_spr}
};

void AMIEffectGameObject::customSetup() {
    EffectGameObject::customSetup();
    if (!AwesomeModifierIcons::s_textureMap.contains(m_objectID)) return;

    if (!AwesomeModifierIcons::get()->getSetting<bool, "solid-border">()) {
        if (auto newSpr = CCSprite::createWithSpriteFrameName("edit_eCollisionBlock01_001.png")) {
            setTexture(newSpr->getTexture());
            setTextureRect(newSpr->getTextureRect());
        }
    }
    
    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    auto spr = CCSprite::create(AwesomeModifierIcons::s_textureMap[m_objectID].c_str());
    if (!spr) return;

    spr->setScale(0.9f);
    addChildAtPosition(spr, Anchor::Center);

    // can end up unscheduled if ran on the object
    MainLevelEditorLayer::get()->runAction(CallFuncExt::create([self = WeakRef(this)] {
        auto obj = self.lock();
        if (obj) {
            obj->updateLetters();
        }
    }));
}

void AMIEffectGameObject::updateLetters() {
    auto showLetter = AwesomeModifierIcons::get()->getSetting<bool, "show-letter">();
    auto label = getChildByType<CCLabelBMFont>(0);
    if (!label) return;

    if (showLetter) {
        label->setPosition({2.f, getContentHeight()});
        label->setScale(0.3f);
        label->setAnchorPoint({0.f, 1.f});
    }
    else {
        label->setVisible(false);
    }
}