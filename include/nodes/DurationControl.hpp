#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/cocos/menu_nodes/CCMenu.h>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <string>
#include <unordered_map>
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