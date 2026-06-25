#pragma once

#include "../Module.hpp"
#include <Geode/modify/EditorPauseLayer.hpp>

class $editorModule(RelocateBuildTools) {
    static StringMap<std::string> s_labelToIcon;

    Ref<EditorPauseLayer> m_pauseLayer;

    void onEditor() override;

    CCSprite* addIcon(CCNode* node, CCLabelBMFont* label);
    void rebuildButtons(std::vector<Ref<CCNode>> nodes);
    CCNode* createBuildTools();
};

class $modify(RBTEditorPauseLayer, EditorPauseLayer){
	$registerEditorHooks(RelocateBuildTools, true)

	struct Fields {
		bool m_noResume = false;
	};
    
	static void _onModify(auto& self) {
        (void) self.setHookPriority("EditorPauseLayer::onResume", -10000); 
    }
    
    void onResume(CCObject* sender);
};