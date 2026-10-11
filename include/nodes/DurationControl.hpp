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

namespace tinker::ui {
	
class DurationControl : public cocos2d::CCNodeRGBA {
public:
	static DurationControl* create();
	bool init();
	void addObject(EffectGameObject* object);
	void updateObjects(EditorUI* editorUI);
	void setMenuPosition(const cocos2d::CCPoint& point, const cocos2d::CCPoint& anchor);
	void showMultiButtons();
	void showDurationButtons();
	void hideAllButtons();
	EffectGameObject* getEndObject();
	
	CCMenuItemSpriteExtra* createButton(const std::string& text, cocos2d::SEL_MenuHandler method);
	void onAlign(cocos2d::CCObject* obj);

	std::unordered_map<EffectGameObject*, DurationSlider*> m_objects{};
	geode::Ref<DurationThumb> m_multiSlider = nullptr;
	cocos2d::CCMenu* m_buttonsMenu = nullptr;
	CCMenuItemSpriteExtra* m_alignAllButton = nullptr;
	bool m_lockUpdateObjects = false;
};

}