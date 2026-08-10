#pragma once

#include <Geode/Geode.hpp>
#include "DurationSlider.hpp"
#include "DurationThumb.hpp"

using namespace geode::prelude;

namespace tinker::ui {
	
	class DurationControl : public CCNodeRGBA {
	public:
		static DurationControl* create();
		bool init();
		void addObject(EffectGameObject* object);
		void updateObjects(EditorUI* editorUI);
		void setMenuPosition(const CCPoint& point, const CCPoint& anchor);
		void showMultiButtons();
		void showDurationButtons();
		void hideAllButtons();
		EffectGameObject* getEndObject();
		
		CCMenuItemSpriteExtra* createButton(const std::string& text, cocos2d::SEL_MenuHandler method);
		void onAlign(CCObject* obj);

		std::unordered_map<EffectGameObject*, DurationSlider*> m_objects;
		Ref<DurationThumb> m_multiSlider = nullptr;
		CCMenu* m_buttonsMenu = nullptr;
		CCMenuItemSpriteExtra* m_alignAllButton = nullptr;
		bool m_lockUpdateObjects;
	};
}