#pragma once

#include "../Module.hpp"
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/EditButtonBar.hpp>

class $editorModule(RepeatingEditorButtons) {
    void onEditor() override;
    bool onToggled(bool state) override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;

    void onSetGroupIDLayer(SetGroupIDLayer* setGroupIDLayer, GameObject* obj, CCArray* objs) override;
    void applyRepeatIfExist(SetGroupIDLayer* setGroupIDLayer, ZStringView id);
    void recursivelySetRepeat(CCNode* node);
};

class $modify(REBCCMenuItemSpriteExtra, CCMenuItemSpriteExtra) {
	$registerEditorHooks(RepeatingEditorButtons)

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

class $modify(REBEditButtonBar, EditButtonBar) {
	$registerEditorHooks(RepeatingEditorButtons)

	void loadFromItems(CCArray* objects, int rows, int columns, bool keepPage);
};
