#pragma once

#include <Geode/Geode.hpp>
#include "nodes/ColorChannelSprite.hpp"

using namespace geode::prelude;

namespace tinker::ui {
	class ColorVisualButton : public CCMenuItemSpriteExtra {
	public:
		static ColorVisualButton* create(EditorUI* editorUI);
		bool init(EditorUI* editorUI);
		void setColorData(int id);
		void updateColor();
		void openColorPicker(CCObject* obj);
	protected:

		int m_id;
		EditorUI* m_editorUI;
		tinker::ui::ColorChannelSprite* m_colorChannelSprite;
		ColorAction* m_action;
	};
}
