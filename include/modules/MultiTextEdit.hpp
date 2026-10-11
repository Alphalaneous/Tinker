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
        std::string m_text = "";
	};

    static void _onModify(auto& self) {
        (void) self.setHookPriorityBeforePost("CustomizeObjectLayer::onClose", "nwo5.text_object_utils");
    }
    
    bool init(GameObject* object, cocos2d::CCArray* objects);
	void onClear(cocos2d::CCObject* sender);
    void textChanged(CCTextInputNode* node);
    void sliderChanged(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    #ifdef GEODE_IS_MACOS
    void keyBackClicked();
    #endif
    void onSelectMode(cocos2d::CCObject* sender);
};