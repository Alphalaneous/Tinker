#pragma once

#include "Geode/cocos/cocoa/CCArray.h"
#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/modify/ButtonSprite.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/ObjectToolbox.hpp>
#include <vector>

using namespace geode::prelude;

/*class $modify(PerfButtonSprite, ButtonSprite) {

    bool init(CCSprite* topSprite, int width, int minWidth, float height, float scale, bool absolute, char const* bgSprite, bool noScaleSpriteForBG);
};

class $modify(PerfEditorUI, EditorUI) {

    struct Fields {
        StringMap<CCTexture2D*> m_textureLookup;

        ~Fields() {
            s_instance = nullptr;
        }
    };

    static PerfEditorUI* get();
    bool init(LevelEditorLayer* editorLayer);
    CreateMenuItem* getCreateBtn(int id, int bg);
    void addTextureLookup(ZStringView textureName, CCTexture2D* texture);
    CCTexture2D* getTextureFromLookup(ZStringView textureName);

    static PerfEditorUI* s_instance;
};
*/

/*class $modify(PerfEditorUI, EditorUI) {

    struct Tab {
        Ref<EditButtonBar> m_editButtonBar;
        Ref<CCMenuItemToggler> m_toggler;
        Ref<CCArray> m_customTabControls;
    };

    static std::vector<Tab> s_createTabs;

    void setupCreateMenu();

};*/