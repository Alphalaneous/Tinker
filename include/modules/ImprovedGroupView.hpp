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
        std::vector<int> groups{};
        std::vector<int> parentGroups{};
        GameObject* object = nullptr;
    };

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};

class $modify(IGVSetGroupIDLayer, SetGroupIDLayer) {
    $registerHooks(ImprovedGroupView, true)

    struct Fields {
        AdvancedScrollLayer* m_scrollLayer = nullptr;
        AdvancedScrollBar* m_scrollBar = nullptr;
        geode::Label* m_groupCountLabel = nullptr;

        int m_lastRemoved = 0;
        std::unordered_map<std::string, short, geode::utils::StringHash, std::equal_to<>> m_namedIDs{};
        geode::ListenerHandle m_listener{};
    };

    static void _onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("SetGroupIDLayer::init", "spaghettdev.named-editor-groups");
    }
    
    bool init(GameObject* obj, cocos2d::CCArray* objs);
    void checkNamedIDs(float dt);
    void onRemoveFromGroup2(cocos2d::CCObject* obj);
    void onAddGroup2(cocos2d::CCObject* obj);
    void onAddGroupParent2(cocos2d::CCObject* obj);
    void regenerateGroupView();
    ImprovedGroupView::GroupData parseObjGroups(GameObject* obj);
};

class $modify(IGVSetupSpawnPopup, SetupSpawnPopup) {
    $registerHooks(ImprovedGroupView, true)

    struct Fields {
        bool m_needsUpdate = true;
        int m_lastPage = 0;
        AdvancedScrollLayer* m_scrollLayer = nullptr;
        AdvancedScrollBar* m_scrollBar = nullptr;
        geode::Label* m_groupCountLabel = nullptr;
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
        AdvancedScrollLayer* m_scrollLayer = nullptr;
        AdvancedScrollBar* m_scrollBar = nullptr;
        geode::Label* m_groupCountLabel = nullptr;
    };

    void updateGroupIDButtons();
};

class $modify(IGVSetupSequenceTriggerPopup, SetupSequenceTriggerPopup) {
    $registerHooks(ImprovedGroupView, true)

    static void _onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("SetupRandAdvTriggerPopup::updateGroupIDButtons", "spaghettdev.named-editor-groups");
    }

    struct Fields {
        AdvancedScrollLayer* m_scrollLayer = nullptr;
        AdvancedScrollBar* m_scrollBar = nullptr;
    };

    void updateGroupIDButtons();
};