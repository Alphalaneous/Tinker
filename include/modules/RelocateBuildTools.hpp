#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorPauseLayer.hpp>

class $module(RelocateBuildTools) {
    static geode::utils::StringMap<std::string> s_labelToIcon;

    geode::Ref<EditorPauseLayer> m_pauseLayer = nullptr;

    void onEditor();

    cocos2d::CCSprite* addIcon(cocos2d::CCNode* node, cocos2d::CCLabelBMFont* label);
    void rebuildButtons(std::vector<geode::Ref<cocos2d::CCNode>> nodes);
    EditButtonBar* createBuildTools();
};

class $modify(RBTEditorPauseLayer, EditorPauseLayer){
	$registerHooks(RelocateBuildTools, true)

	struct Fields {
		bool m_noResume = false;
	};
    
	static void _onModify(auto& self) {
        (void) self.setHookPriority("EditorPauseLayer::onResume", -10000); 
    }
    
    void onResume(cocos2d::CCObject* sender);
};