#include "ObjectItem.hpp"
#include <Geode/ui/NineSlice.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "utils/Constants.hpp"

using namespace tinker::constants::objects;
using namespace tinker::ui;

bool ObjectItem::init(int objectID, int count) {
    if (!CCNode::init()) return false;

    auto arr = CCArray::create();

    CCNode* object;

    if (objectID == Particle) {
        object = CCSprite::createWithSpriteFrameName("edit_eCParticleBtn_001.png");
    }
    else if (objectID == LinkedTeleportPortal) {
        object = CCNode::create();
        object->setContentSize({100.f, 100.f});
        object->setAnchorPoint({0.5f, 0.5f});
        auto portalBlue = EditorUI::get()->spriteFromObjectString("1,2902", true, false, 1, nullptr, nullptr, nullptr);
        auto portalOrange = EditorUI::get()->spriteFromObjectString("1,2064", true, false, 1, nullptr, nullptr, nullptr);
        portalOrange->setScaleX(-1.f);

        portalBlue->setPosition(object->getContentSize() / 2.f + CCPoint{12, -10.f});
        portalOrange->setPosition(object->getContentSize() / 2.f + CCPoint{-12, 10.f});

        object->addChild(portalBlue);
        object->addChild(portalOrange);
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
    
    setAnchorPoint({0.5f, 0.5f});

    auto countLabel = CCLabelBMFont::create(numToString(count).c_str(), "bigFont.fnt");
    countLabel->setScale(0.3f);
    countLabel->setAnchorPoint({1.f, 0.f});
    countLabel->limitLabelWidth(max + 6.f, 0.3f, 0.1f);

    float scaleX = max / std::max(object->getContentWidth(), 0.001f);
    float scaleY = max / std::max(object->getContentHeight(), 0.001f);

    float scale = std::min(std::min(scaleX, scaleY), 1.f);

    object->setID("object"_spr);
    object->setScale(scale * 0.7f);
    setContentSize({std::max(object->getScaledContentWidth(), max) + labelOffset + gap, max + labelOffset + gap});
    
    object->setPosition({max / 2.f + gap / 2.f, max / 2.f + labelOffset + gap / 2.f});
    object->setZOrder(1);

    auto bg = geode::NineSlice::create("geode.loader/white-square.png");
    bg->setColor({0, 0, 0});
    bg->setOpacity(127);
    bg->setContentSize({max, max});
    bg->setPosition({max / 2.f + gap / 2.f, max / 2.f + labelOffset + gap / 2.f});
    bg->setID("background"_spr);

    utils::random::Generator generator(objectID);

    for (int i = 0; i < 5; i++) {
        generator.next();
    }

    auto bgRotationOffset = generator.generate<float>(-3.f, 3.f);
    bg->setRotation(bgRotationOffset);

    auto objectRotationOffset = generator.generate<float>(-3.f, 3.f);
    object->setRotation(-bgRotationOffset + objectRotationOffset);

    countLabel->setPosition({max + labelOffset + gap / 2.f, gap / 2.f});
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