#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/binding/EffectGameObject.hpp>

namespace tinker::ui {

class DurationSlider : public cocos2d::CCNodeRGBA {
public:
	static DurationSlider* create(EffectGameObject* object);
	bool init(EffectGameObject* object);
};

}