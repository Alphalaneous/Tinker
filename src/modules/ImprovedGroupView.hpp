#pragma once

#include "../Module.hpp"
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

using namespace alpha::prelude;

class $editorModule(ImprovedGroupView) {
    struct GroupData {
        std::vector<int> groups;
        std::vector<int> parentGroups;
        GameObject* object;
    };

    bool onToggled(bool state) override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;
};

class $modify(IGVSetGroupIDLayer, SetGroupIDLayer) {
    $registerEditorHooks(ImprovedGroupView, true)

    struct Fields {
        AdvancedScrollLayer* m_scrollLayer;
        CCLabelBMFont* m_groupCountLabel;

        int m_lastRemoved = 0;
        std::unordered_map<std::string, short, geode::utils::StringHash, std::equal_to<>> m_namedIDs;
        ListenerHandle m_listener;
    };

    static void _onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("SetGroupIDLayer::init", "spaghettdev.named-editor-groups");
    }
    
    void checkNamedIDs(float dt);
    bool init(GameObject* obj, cocos2d::CCArray* objs);
    void onRemoveFromGroup2(CCObject* obj);
    void onAddGroup2(CCObject* obj);
    void onAddGroupParent2(CCObject* obj);
    void regenerateGroupView();
    ImprovedGroupView::GroupData parseObjGroups(GameObject* obj);
};