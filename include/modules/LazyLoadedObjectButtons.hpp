#pragma once

#include "Geode/cocos/textures/CCTexture2D.h"
#include "Geode/utils/ZStringView.hpp"
#include "module/Module.hpp"
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CreateMenuItem.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <unordered_map>

class $module(LazyLoadedObjectButtons) {

};

class $modify(LLOBEditorUI, EditorUI) {
    $registerHooks(LazyLoadedObjectButtons)

    struct Fields {
        geode::Ref<EffectGameObject> m_dummyObject = nullptr;
        std::unordered_map<int, geode::Ref<cocos2d::CCTexture2D>> m_bgSprites;
    };

    CreateMenuItem* getCreateBtn(int id, int bg);
    CreateMenuItem* menuItemFromObjectString(std::string str, int id);
    void updateCreateMenu(bool selectTab);
    static GameObject* getDummyObject();
    static cocos2d::CCTexture2D* getBGSprite(int id);
};

namespace tinker::ui {

class LazyButtonSprite : public ButtonSprite {
public:
    static LazyButtonSprite* create(CCSprite* topSprite, int bgID);
    void loadSprite(CCSprite* topSprite);

    void setColor(const cocos2d::ccColor3B& color3);
    void updateDisplayedColor(const cocos2d::ccColor3B& parentColor);
    void setOpacity(GLubyte opacity);
    void setOpacityModifyRGB(bool modify);
    void updateDisplayedOpacity(GLubyte parentOpacity);
    void draw();

protected:
    bool init(CCSprite* topSprite, int bgID);

    cocos2d::CCTexture2D* getTexture();
    void setTexture(cocos2d::CCTexture2D *texture);

    int m_bgID;
};

class LazyCreateMenuItem : public CreateMenuItem {
public:
    LazyCreateMenuItem();

    static LazyCreateMenuItem* createCustom(geode::ZStringView objStr, int objectID, int bgID, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);
    static LazyCreateMenuItem* create(int objectID, int bgID, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);
    void loadObject();
    void visit();
    void onEnter();

protected:
    bool init(int objectID, int bgID, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);
    bool initCustom(geode::ZStringView objStr, int objectID, int bgID, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);

    tinker::ui::LazyButtonSprite* m_buttonSprite = nullptr;
    bool m_loaded = false;
    bool m_customObject = false;
    std::string m_objectString = "";
};

}