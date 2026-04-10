#include "ObjectItem.hpp"
#include <Geode/ui/NineSlice.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace tinker::ui;

bool ObjectItem::init(int objectID, int count) {
    if (!CCNode::init()) return false;

    auto arr = CCArray::create();

    CCSprite* object;

    if (objectID == 2065) {
        object = CCSprite::createWithSpriteFrameName("edit_eCParticleBtn_001.png");
    }
    else {
        object = EditorUI::get()->spriteFromObjectString(fmt::format("1,{}", objectID), true, false, 1, nullptr, nullptr, nullptr);
    }

    if (auto effectGameObject = typeinfo_cast<EffectGameObject*>(object->getChildByType<GameObject>(0))) {
        if (effectGameObject->m_objectLabel) effectGameObject->m_objectLabel->setVisible(false);
    }

    float max = 40.f;
    float labelOffset = 3.f;
    float gap = 10.f;
    
    setAnchorPoint({ 0.5f, 0.5f });

    auto countLabel = CCLabelBMFont::create(numToString(count).c_str(), "bigFont.fnt");
    countLabel->setScale(0.3f);
    countLabel->setAnchorPoint({1.f, 0.f});
    countLabel->limitLabelWidth(max + 6, 0.3f, 0.1f);

    float scaleX = max / std::max(object->getContentWidth(), 0.001f);
    float scaleY = max / std::max(object->getContentHeight(), 0.001f);

    float scale = std::min(std::min(scaleX, scaleY), 1.f);

    object->setID("object"_spr);
    object->setScale(scale * 0.70f);
    setContentSize({std::max(object->getScaledContentWidth(), max) + labelOffset + gap, max + labelOffset + gap});
    
    object->setPosition({max / 2.f + gap / 2, max / 2.f + labelOffset + gap / 2});
    object->setZOrder(1);

    auto bg = geode::NineSlice::create("geode.loader/white-square.png");
    bg->setColor({0, 0, 0});
    bg->setOpacity(127);
    bg->setContentSize({max, max});
    bg->setPosition({max / 2.f + gap / 2, max / 2.f + labelOffset + gap / 2});
    bg->setID("background"_spr);

    auto bgRotationOffset = utils::random::generate<float>(-3, 3);
    bg->setRotation(bgRotationOffset);

    auto objectRotationOffset = utils::random::generate<float>(-3, 3);
    object->setRotation(objectRotationOffset);

    countLabel->setPosition({max + labelOffset + gap / 2, gap / 2});
    countLabel->setZOrder(2);
    countLabel->setID("object-count-label"_spr);

    addChild(object);
    addChild(bg);
    addChild(countLabel);

    return true;
}

ObjectItem* ObjectItem::create(int objectID, int count) {
    auto ret = new ObjectItem();
    if (ret->init(objectID, count)) {
        ret->autorelease();
        return ret;
    }
   
    delete ret;
    return nullptr;
}