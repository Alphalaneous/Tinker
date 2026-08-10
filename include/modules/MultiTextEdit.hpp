#pragma once

#include "module/Module.hpp"
#include <Geode/modify/CustomizeObjectLayer.hpp>

class $module(MultiTextEdit) {
    bool onToggled(bool state);
};

class $modify(MTECustomizeObjectLayer, CustomizeObjectLayer) {
    $registerHooks(MultiTextEdit, true);

    struct Fields {
		bool m_textObjects = false;
        bool m_allowChange = false;
        std::string m_text;
	};

    static void _onModify(auto& self) {
        (void) self.setHookPriorityBeforePost("CustomizeObjectLayer::onClose", "nwo5.text_object_utils");
    }
    
    bool init(GameObject* object, CCArray* objects);
	void onClear(CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void sliderChanged(CCObject* sender);
    void onClose(CCObject* sender);
    void onSelectMode(cocos2d::CCObject* sender);
};