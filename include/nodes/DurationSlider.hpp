#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tinker::ui {
	class DurationSlider : public CCNodeRGBA {
	public:
		static DurationSlider* create(EffectGameObject* object);
		bool init(EffectGameObject* object);
	};
}