#pragma once

#include "module/Module.hpp"
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/LevelSettingsLayer.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/ConfigureValuePopup.hpp>

class $module(RepeatingEditorButtons) {
    void onEditor();
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    static void applyRepeatIfExist(SetGroupIDLayer* setGroupIDLayer, ZStringView id);
    static void recursivelySetRepeat(CCNode* node);
	static void setRepeatable(CCMenuItemSpriteExtra* item, bool repeatable);
	static void forceNoRepeat(CCMenuItemSpriteExtra* item, bool force);
};

class $modify(REBCCMenuItemSpriteExtra, CCMenuItemSpriteExtra) {
	$registerHooks(RepeatingEditorButtons)

	struct Fields {
		bool m_isHolding;
		bool m_repeatable;
		bool m_forceNoRepeat;
	};

	void setRepeatable(bool repeatable);
	void forceNoRepeat(bool force);
	void checkHold(float dt);
    void activate();
    void selected();
    void unselected();
};

// not linked to any modules so they always run. These are lightweight and simply mark a flag on the button so they always can be toggled in settings
class $modify(REBCustomizeObjectLayer, CustomizeObjectLayer) {
    bool init(GameObject* object, cocos2d::CCArray* objects);
};

class $modify(REBEditButtonBar, EditButtonBar) {
	void loadFromItems(CCArray* objects, int rows, int columns, bool keepPage);
};

class $modify(REBLevelSettingsLayer, LevelSettingsLayer) {
    bool init(LevelSettingsObject* object, LevelEditorLayer* layer);
};

class $modify(REBConfigureValuePopup, ConfigureValuePopup) {
    bool init(ConfigureValuePopupDelegate* delegate, float value, float minimum, float maximum, gd::string title, gd::string description, int type);
};

class $modify(REBSetupTriggerPopup, SetupTriggerPopup) {
    bool init(EffectGameObject* trigger, cocos2d::CCArray* triggers, float width, float height, int background);
    void createEasingControls(cocos2d::CCPoint position, float scale, int page, int group);
    void createCustomEasingControls(gd::string text, cocos2d::CCPoint position, float scale, int typeProperty, int rateProperty, int page, int group);
    cocos2d::CCArray* createValueControlAdvanced(int property, gd::string label, cocos2d::CCPoint position, float scale, bool noSlider, InputValueType valueType, int length, bool arrows, float sliderMin, float sliderMax, int page, int group, GJInputStyle inputStyle, int decimalPlaces, bool allowDisable);
};