#include "modules/LazyLoadedObjectButtons.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCArray.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/cocos/textures/CCTextureCache.h"
#include "Geode/utils/ZStringView.hpp"
#include "Geode/utils/cocos.hpp"
#include "MainHooks.hpp"
#include "modules/CenteredObjectButtons.hpp"
#include "utils/Constants.hpp"
#include <Geode/Enums.hpp>
#include <Geode/binding/CreateMenuItem.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/GameObject.hpp>
#include <string>
#include <unordered_map>
#include "../misc/OMFNISupport.hpp"

using namespace geode::prelude;

CreateMenuItem* LLOBEditorUI::getCreateBtn(int id, int bg) {
    auto fields = m_fields.self();
    if (!fields->m_useLazy) {
        return EditorUI::getCreateBtn(id, bg);
    }

    auto item = tinker::ui::LazyCreateMenuItem::create(id, bg, this, menu_selector(EditorUI::onCreateButton));

    m_createButtonArray->addObject(item);

    return item;
}

CreateMenuItem* LLOBEditorUI::menuItemFromObjectString(std::string str, int id) {
    auto fields = m_fields.self();
    if (!fields->m_useLazy) {
        return EditorUI::menuItemFromObjectString(str, id);
    }

    auto item = tinker::ui::LazyCreateMenuItem::createCustom(str, id, 4, this, menu_selector(EditorUI::onCreateButton));

    m_customObjectButtonArray->addObject(item);

    return item;
}

void LLOBEditorUI::setupCreateMenu() {
    auto fields = m_fields.self();
    fields->m_useLazy = true;
    EditorUI::setupCreateMenu();
    fields->m_useLazy = false;
}

void LLOBEditorUI::updateCreateMenu(bool selectTab) {
    if (selectTab) {
        CreateMenuItem* cmi = nullptr;

        for (auto item : m_createButtonArray->asExt<CreateMenuItem>()) {
            if (item->getUserFlag("search-item"_spr)) continue;
            if (item->m_objectID == m_selectedObjectIndex) {
                cmi = item;
                break;
            }
        }

        static_cast<tinker::ui::LazyCreateMenuItem*>(cmi)->loadObject();
    }
    
    EditorUI::updateCreateMenu(selectTab);
}

GameObject* LLOBEditorUI::getDummyObject() {
    auto editor = modify_cast<LLOBEditorUI*>(MainEditorUI::get());
    auto fields = editor->m_fields.self();
    if (!fields->m_dummyObject) {
        fields->m_dummyObject = EffectGameObject::create("square_01_001.png");
        fields->m_dummyObject->setVisible(false);
        fields->m_dummyObject->m_objectID = 1;

        // fix for omfni
        fields->m_dummyObject->m_animateOnTrigger = true;
        fields->m_dummyObject->m_animationTriggered = false;
        fields->m_dummyObject->m_colorSprite = CCSprite::create();
        fields->m_dummyObject->setUserObject("dummy-color-sprite"_spr, fields->m_dummyObject->m_colorSprite);
    }
    return fields->m_dummyObject;
}

cocos2d::CCTexture2D* LLOBEditorUI::getBGSprite(int id) {
    auto editor = modify_cast<LLOBEditorUI*>(MainEditorUI::get());
    auto fields = editor->m_fields.self();
    
    if (!fields->m_bgSprites[id]) {
        return fields->m_bgSprites[id] = CCTextureCache::get()->textureForKey(fmt::format("GJ_button_0{}.png", id).c_str());
    }
    return fields->m_bgSprites[id];
}

namespace tinker::ui {

LazyButtonSprite* LazyButtonSprite::create(CCSprite* topSprite, int bgID) {
    auto ret = new LazyButtonSprite();
    if (ret->init(topSprite, bgID)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

// saves a few milliseconds if I make it act like a CCNodeRGBA instead
bool LazyButtonSprite::init(CCSprite* topSprite, int bgID) {
    if (!CCNodeRGBA::init()) return false;

    m_bgID = bgID;
    m_mode = 0;
    m_minWidth = 0.f;
    m_absolute = true;
    m_width = static_cast<float>(32.f);
    m_scale = 1.f;
    m_subSprite = topSprite;

    setContentSize({40.f, 40.f});

    addChild(topSprite, 1);

    m_subBGSprite = CCSprite::create();
    addChild(m_subBGSprite, 0);

    return true;
}

void LazyButtonSprite::draw() {
    CCNodeRGBA::draw();
}

cocos2d::CCTexture2D* LazyButtonSprite::getTexture() {
    return nullptr;
}

void LazyButtonSprite::setTexture(cocos2d::CCTexture2D *texture) {

}

void LazyButtonSprite::setColor(const cocos2d::ccColor3B& color3) {
    CCNodeRGBA::setColor(color3);
}

void LazyButtonSprite::updateDisplayedColor(const cocos2d::ccColor3B& parentColor) {
    CCNodeRGBA::updateDisplayedColor(parentColor);
}

void LazyButtonSprite::setOpacity(GLubyte opacity) {
    CCNodeRGBA::setOpacity(opacity);
}

void LazyButtonSprite::setOpacityModifyRGB(bool modify) {
    if (m_bOpacityModifyRGB != modify) {
        m_bOpacityModifyRGB = modify;
    }
}

void LazyButtonSprite::updateDisplayedOpacity(GLubyte parentOpacity) {
    CCNodeRGBA::updateDisplayedOpacity(parentOpacity);
}

void LazyButtonSprite::loadSprite(CCSprite* topSprite) {
    removeChild(m_subSprite, true);

    m_subSprite = topSprite;
    addChild(topSprite, 1);

    m_subBGSprite->removeFromParent();

    m_subBGSprite = CCSprite::createWithTexture(LLOBEditorUI::getBGSprite(m_bgID));
    addChild(m_subBGSprite, 0);
    
    if (0.f < m_width && m_width < m_subSprite->getContentSize().width) {
        m_subSprite->setScale(m_width / m_subSprite->getContentSize().width);
    }

    float height = 32.f;

    if (0.f < height && height < m_subSprite->getContentSize().height) {
        float fit = height / m_subSprite->getContentSize().height;
        if (fit < m_subSprite->getScale()) m_subSprite->setScale(height / m_subSprite->getContentSize().height);
    }
    
    m_subSprite->setScale(std::min(m_subSprite->getScale(), m_scale));
    updateSpriteBGSize();
}

LazyCreateMenuItem::LazyCreateMenuItem() : CreateMenuItem(ZeroConstructor) {
    m_uReference = 1;

    auto director = CCDirector::get();
    m_pActionManager = director->getActionManager();
    m_pActionManager->retain();
    m_pScheduler = director->getScheduler();
    m_pScheduler->retain();

    auto pEngine = CCScriptEngineManager::sharedManager()->getScriptEngine();
    m_eScriptType = pEngine != nullptr ? pEngine->getScriptType() : kScriptTypeNone;

    m_fScaleX = 1.f;
    m_fScaleY = 1.f;

    m_nTag = -1;
    m_sAdditionalTransform = CCAffineTransformMakeIdentity();
    m_bTransformDirty = true;
    m_bInverseDirty = true;
    m_bVisible = true;

    // cheaty
    auto obj = new CCObject();
    m_uID = obj->m_uID;
    obj->release();

    m_pComponentContainer = new CCComponentContainer(ZeroConstructor);
    m_pComponentContainer->m_pComponents = nullptr;
    m_pComponentContainer->m_pOwner = this;

    _displayedOpacity = 255;
    _realOpacity = 255;
    _displayedColor = ccWHITE;
    _realColor = ccWHITE;
    _cascadeColorEnabled = false;
    _cascadeOpacityEnabled = false;

    m_bSelected = false;
    m_bEnabled = false;
    m_pListener = nullptr;
    m_pfnSelector = nullptr;
    m_nScriptTapHandler = 0;

    m_pNormalImage = nullptr;
    m_pSelectedImage = nullptr;
    m_pDisabledImage = nullptr;
    
    m_scaleMultiplier = 1.f;
    m_baseScale = 0.f;
    m_animationEnabled = false;
    m_colorEnabled = false;
    m_volume = 1.f;
    m_colorDip = 0.784314f;
    m_duration = 0.3f;
    m_unselectedDuration = 0.4f;

    m_animationType = MenuAnimationType::Scale;
    m_iconType = IconType::Cube;
}

LazyCreateMenuItem* LazyCreateMenuItem::createCustom(geode::ZStringView objStr, int objectID, int bgID, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector) {
    auto ret = new LazyCreateMenuItem();
    if (ret->initCustom(objStr, objectID, bgID, target, selector)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

LazyCreateMenuItem* LazyCreateMenuItem::create(int objectID, int bgID, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector) {
    auto ret = new LazyCreateMenuItem();
    if (ret->init(objectID, bgID, target, selector)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LazyCreateMenuItem::init(int objectID, int bgID, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector) {
    m_objectID = objectID;

    m_buttonSprite = tinker::ui::LazyButtonSprite::create(LLOBEditorUI::getDummyObject(), bgID);
    
    if (!CreateMenuItem::init(m_buttonSprite, nullptr, target, selector)) return false;

    setScale(0.9f);
    m_baseScale = 0.9f;

    return true;
}

bool LazyCreateMenuItem::initCustom(geode::ZStringView objStr, int objectID, int bgID, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector) {
    m_objectID = objectID;
    m_customObject = true;
    m_objectString = objStr;

    m_buttonSprite = tinker::ui::LazyButtonSprite::create(LLOBEditorUI::getDummyObject(), bgID);
    
    if (!CreateMenuItem::init(m_buttonSprite, nullptr, target, selector)) return false;

    setScale(0.9f);
    m_baseScale = 0.9f;

    return true;
}

void LazyCreateMenuItem::onEnter() {
    CreateMenuItem::onEnter();
    loadObject();
}

void LazyCreateMenuItem::visit() {
    CreateMenuItem::visit();
    loadObject();
}

void LazyCreateMenuItem::loadObject() {
    using namespace tinker::constants;

    if (m_loaded) return;

    if (m_customObject) {
        if (!nodeIsVisible(this)) return;

        auto sprite = MainEditorUI::get()->spriteFromObjectString(m_objectString, true, false, 100, nullptr, nullptr, nullptr);
        float width = sprite->getContentSize().width;
        float height = sprite->getContentSize().height;

        sprite->setScale(std::min(40.f / (width <= height ? sprite->getContentSize().height : sprite->getContentSize().width), 1.f));
        sprite->setZOrder(1);

        m_buttonSprite->loadSprite(sprite);
        m_buttonSprite->updateSpriteOffset({0.f, -1.f});

        m_loaded = true;
        return;
    }

    if (!isVisible()) return;

    GameObject* obj = nullptr;

    if (m_objectID == objects::Text || m_objectID == objects::Counter) {
        auto texture = CCTextureCache::get()->textureForKey("bigFont.png");
        if (texture) {
            obj = TextGameObject::create(texture);
        }
    }
    
    if (!obj) {
        obj = GameObject::createWithKey(m_objectID);
    }

    m_spriteFileName = ObjectToolbox::sharedState()->m_allKeys[m_objectID];

    obj->customSetup();
    obj->addColorSprite(m_spriteFileName);
    obj->setupCustomSprites(m_spriteFileName);

    if (m_objectID == objects::Text) {
        static_cast<TextGameObject*>(obj)->updateTextObject("A", true);
    }
    else if (m_objectID == objects::Counter) {
        static_cast<TextGameObject*>(obj)->updateTextObject("0", true);
    }
    else if (m_objectID == objects::ToggleTrigger) {
        static_cast<EffectGameObject*>(obj)->updateSpecialColor();
    }

    if (obj->m_classType == GameObjectClassType::Effect) {
        auto effect = static_cast<EffectGameObject*>(obj);

        bool colorable = (effect->m_customColorType == 1) || (effect->m_customColorType == 0 && effect->m_maybeNotColorable);

        if (!colorable && !effect->m_colorSprite && effect->m_baseColor->m_defaultColorID != color_channels::Obj && effect->m_shouldPreview) {
            obj->setColor({200, 200, 255});
        }
    }

    if (obj->m_opacityMod2 > 0.f) {
        obj->setOpacity(255);
    }

    auto rect = LevelEditorLayer::get()->getObjectRect(obj, false, false);

    float maxSize = std::max(rect.size.width, rect.size.height);
    float scale = obj->getScale();

    m_buttonSprite->loadSprite(obj);

    if (32.f / maxSize < scale || obj->m_pixelScaleX > 1.f) {
        obj->setScale(32.f / maxSize);
    }

    obj->setZOrder(1);
    
    if (CenteredObjectButtons::isEnabled()) {
        m_buttonSprite->updateSpriteOffset({-0.25f, -1.5f});
    }
    else {
        m_buttonSprite->updateSpriteOffset({0.f, 0.f});
    }

    if (obj->m_colorSprite && !obj->m_unk28c) {
        int z = obj->m_colorZLayerRelated ? 1 : -1;
        obj->addChild(obj->m_colorSprite, z);

        auto size = obj->getContentSize();
        obj->m_colorSprite->setPosition(size / 2.f);
        obj->m_colorSprite->setColor({200, 200, 255});
        obj->m_colorSprite->setScale(1.f);
    }

    tinker::support::toOMFNI(obj);

    m_loaded = true;
}

}