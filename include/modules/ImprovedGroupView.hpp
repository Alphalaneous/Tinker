#pragma once

#include "module/Module.hpp"
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/SetupSpawnPopup.hpp>
#include <Geode/modify/SetupRandAdvTriggerPopup.hpp>
#include <Geode/modify/SetupSequenceTriggerPopup.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

using namespace alpha::prelude;

class $module(ImprovedGroupView) {
    struct GroupData {
        std::vector<int> groups;
        std::vector<int> parentGroups;
        GameObject* object;
    };

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};

class $modify(IGVSetGroupIDLayer, SetGroupIDLayer) {
    $registerHooks(ImprovedGroupView, true)

    struct Fields {
        AdvancedScrollLayer* m_scrollLayer;
        AdvancedScrollBar* m_scrollBar;
        geode::Label* m_groupCountLabel;

        int m_lastRemoved = 0;
        std::unordered_map<std::string, short, geode::utils::StringHash, std::equal_to<>> m_namedIDs;
        ListenerHandle m_listener;
    };

    static void _onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("SetGroupIDLayer::init", "spaghettdev.named-editor-groups");
    }
    
    bool init(GameObject* obj, cocos2d::CCArray* objs);
    void checkNamedIDs(float dt);
    void onRemoveFromGroup2(CCObject* obj);
    void onAddGroup2(CCObject* obj);
    void onAddGroupParent2(CCObject* obj);
    void regenerateGroupView();
    ImprovedGroupView::GroupData parseObjGroups(GameObject* obj);
};

class $modify(IGVSetupSpawnPopup, SetupSpawnPopup) {
    $registerHooks(ImprovedGroupView, true)

    struct Fields {
        bool m_needsUpdate = true;
        int m_lastPage = 0;
        AdvancedScrollLayer* m_scrollLayer;
        AdvancedScrollBar* m_scrollBar;
        geode::Label* m_groupCountLabel;
    };

    static void _onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("SetupSpawnPopup::updateRemapButtons", "spaghettdev.named-editor-groups");
    }

    bool init(EffectGameObject* object, cocos2d::CCArray* objects);
    void fixNamedEditorGroups(float dt);
    void updateRemapButtons(float dt);
    void onSelectRemap2(cocos2d::CCObject* sender);
};

class $modify(IGVSetupRandAdvTriggerPopup, SetupRandAdvTriggerPopup) {
    $registerHooks(ImprovedGroupView, true)

    static void _onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("SetupRandAdvTriggerPopup::updateGroupIDButtons", "spaghettdev.named-editor-groups");
    }

    struct Fields {
        AdvancedScrollLayer* m_scrollLayer;
        AdvancedScrollBar* m_scrollBar;
        geode::Label* m_groupCountLabel;
    };

    void updateGroupIDButtons();
};

class $modify(IGVSetupSequenceTriggerPopup, SetupSequenceTriggerPopup) {
    $registerHooks(ImprovedGroupView, true)

    static void _onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("SetupRandAdvTriggerPopup::updateGroupIDButtons", "spaghettdev.named-editor-groups");
    }

    struct Fields {
        AdvancedScrollLayer* m_scrollLayer;
        AdvancedScrollBar* m_scrollBar;
    };

    void updateGroupIDButtons();
};