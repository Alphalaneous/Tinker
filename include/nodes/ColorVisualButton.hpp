#pragma once

#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/ColorAction.hpp>
#include <Geode/binding/EditorUI.hpp>
#include "nodes/ColorChannelSprite.hpp"

namespace tinker::ui {

class ColorVisualButton : public CCMenuItemSpriteExtra {
public:
	static ColorVisualButton* create(EditorUI* editorUI);
	bool init(EditorUI* editorUI);
	void setColorData(int id);
	void updateColor();
	void openColorPicker(CCObject* obj);
protected:

	int m_id = 0;
	EditorUI* m_editorUI = nullptr;
	tinker::ui::ColorChannelSprite* m_colorChannelSprite = nullptr;
	ColorAction* m_action = nullptr;
};

}
