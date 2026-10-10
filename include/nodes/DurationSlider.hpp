#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/binding/EffectGameObject.hpp>

using namespace geode::prelude;

namespace tinker::ui {
	class DurationSlider : public CCNodeRGBA {
	public:
		static DurationSlider* create(EffectGameObject* object);
		bool init(EffectGameObject* object);
	};
}