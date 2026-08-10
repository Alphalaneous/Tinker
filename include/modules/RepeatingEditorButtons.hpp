#pragma once

#include "module/Module.hpp"
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>

class $module(RepeatingEditorButtons) {
    void onEditor();
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void applyRepeatIfExist(SetGroupIDLayer* setGroupIDLayer, ZStringView id);
    void recursivelySetRepeat(CCNode* node);
};

class $modify(REBCCMenuItemSpriteExtra, CCMenuItemSpriteExtra) {
	$registerHooks(RepeatingEditorButtons)

	struct Fields {
		bool m_isHolding;
		bool m_repeatable;
	};

	void setRepeatable(bool repeatable);
	void checkHold(float dt);
    void activate();
    void selected();
    void unselected();
};

class $modify(REBCustomizeObjectLayer, CustomizeObjectLayer) {
	$registerHooks(RepeatingEditorButtons)

    bool init(GameObject* object, cocos2d::CCArray* objects);
};

class $modify(REBEditButtonBar, EditButtonBar) {
	$registerHooks(RepeatingEditorButtons)

	void loadFromItems(CCArray* objects, int rows, int columns, bool keepPage);
};
