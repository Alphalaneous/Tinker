#include "DurationSlider.hpp"
#include "DurationThumb.hpp"
#include "utils/Constants.hpp"

using namespace tinker::ui;

DurationSlider* DurationSlider::create(EffectGameObject* object) {
    auto ret = new DurationSlider();
    if (ret->init(object)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool DurationSlider::init(EffectGameObject* object) {
    auto mainThumb = DurationThumb::create(this, object);
    mainThumb->setID("main-thumb"_spr);
    addChild(mainThumb);
    
    if (object->m_objectID == constants::objects::PulseTrigger) {
        auto fadeInThumb = DurationThumb::create(this, object, ThumbType::FadeIn);
        fadeInThumb->setID("fade-in-thumb"_spr);
        addChild(fadeInThumb);

        auto fadeOutThumb = DurationThumb::create(this, object, ThumbType::FadeOut);
        fadeOutThumb->setID("fade-out-thumb"_spr);
        addChild(fadeOutThumb);
    }

    return true;
}