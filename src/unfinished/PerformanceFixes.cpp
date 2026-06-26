/*#include "PerformanceFixes.hpp"
#include "Geode/cocos/cocoa/CCArray.h"
#include "Geode/loader/Log.hpp"
#include "Geode/utils/ZStringView.hpp"
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/binding/EditorUI.hpp>

bool PerfButtonSprite::init(CCSprite* topSprite, int width, int minWidth, float height, float scale, bool absolute, char const* bgSprite, bool noScaleSpriteForBG) {
    if (!CCSprite::init()) return false;

    m_mode = 0;
    m_minWidth = 0.0f;
    m_absolute = true;
    m_width = static_cast<float>(width);
    m_scale = scale;
    m_subSprite = topSprite;

    addChild(topSprite, 1);

    if (noScaleSpriteForBG) {
        auto editorUI = PerfEditorUI::get();
        if (auto texture = editorUI->getTextureFromLookup(bgSprite)) {
            m_subBGSprite = CCSprite::createWithTexture(texture);
            addChild(m_subBGSprite, 0);
        }
        else {
            m_subBGSprite = CCSprite::create(bgSprite);
            editorUI->addTextureLookup(bgSprite, m_subBGSprite->getTexture());
            addChild(m_subBGSprite, 0);
        }
    }
    else {
        CCRect rect(0, 0, 40, 40);
        m_BGSprite = CCScale9Sprite::create(bgSprite, rect);

        m_BGSprite->setContentSize(CCSize(16, 16));
        addChild(m_BGSprite, 0);
    }

    if (m_width > 0.f) {
        float spriteWidth = topSprite->getContentSize().width;

        if (spriteWidth > m_width) {
            float scaleFactor = m_width / spriteWidth;
            topSprite->setScale(scaleFactor);
        }
    }

    if (height > 0.0f) {
        float spriteHeight = topSprite->getContentSize().height;
        float currentScale = topSprite->getScale();

        if (spriteHeight > height) {
            float scaleFactor = height / spriteHeight;

            if (scaleFactor < currentScale) {
                topSprite->setScale(scaleFactor);
            }
        }
    }

    float currentScale = topSprite->getScale();
    if (scale <= currentScale) {
        topSprite->setScale(scale);
    }

    updateSpriteBGSize();

    return true;
}

PerfEditorUI* PerfEditorUI::s_instance = nullptr;

PerfEditorUI* PerfEditorUI::get() {
    return s_instance;
}

bool PerfEditorUI::init(LevelEditorLayer* editorLayer) {
    s_instance = this;
    return EditorUI::init(editorLayer);
}

void PerfEditorUI::addTextureLookup(ZStringView textureName, CCTexture2D* texture) {
    m_fields->m_textureLookup[textureName] = texture;
}

CCTexture2D* PerfEditorUI::getTextureFromLookup(ZStringView textureName) {
    auto fields = m_fields.self();
    auto iter = fields->m_textureLookup.find(textureName);
    if (iter != fields->m_textureLookup.end()) {
        return iter->second;
    }
    return nullptr;
}

CreateMenuItem* PerfEditorUI::getCreateBtn(int id, int bg) {
    const char* bgSprite = "GJ_button_01.png";
    switch (bg) {
        case 2: bgSprite = "GJ_button_02.png"; break;
        case 3: bgSprite = "GJ_button_03.png"; break;
        case 4: bgSprite = "GJ_button_04.png"; break;
        case 5: bgSprite = "GJ_button_05.png"; break;
        case 6: bgSprite = "GJ_button_06.png"; break;
    }

    /*GameObject* obj = nullptr;

    if (id == 0x392 || id == 0x64F) {
        auto texture = CCTextureCache::get()->addImage("bigFont.png", false);
        obj = TextGameObject::create(texture);
    }
    else {
        obj = GameObject::createWithKey(id);
    }

    auto frame = ObjectToolbox::sharedState()->intKeyToFrame(id);

    obj->customSetup();
    obj->addColorSprite(frame);
    obj->setupCustomSprites(frame);

    if (id == 0x392) {
        static_cast<TextGameObject*>(obj)->updateTextObject("A", true);
    }
    else if (id == 0x64F) {
        static_cast<TextGameObject*>(obj)->updateTextObject("0", true);
    }
    else if (id == 0x419) {
        static_cast<EffectGameObject*>(obj)->updateSpecialColor();
    }

    if (obj->m_classType == GameObjectClassType::Effect) {
        auto effect = static_cast<EffectGameObject*>(obj);

        bool colorable = (effect->m_customColorType == 1) || (effect->m_customColorType == 0 && effect->m_maybeNotColorable);

        if (!colorable && !effect->m_colorSprite && effect->m_baseColor->m_defaultColorID != 1004 && effect->m_shouldPreview) {
            obj->setColor({255, 200, 200});
        }
    }

    if (obj->m_opacityMod2 > 0.0f) {
        obj->setOpacity(255);
    }*/

/*    auto fakeSprite = new GameObject();
    static_cast<CCSprite*>(fakeSprite)->init();
    fakeSprite->autorelease();

    auto normal = ButtonSprite::create(
        fakeSprite,
        32,
        1,
        32.0f,
        1.0f,
        false,
        bgSprite,
        true
    );

    /*auto rect = m_editorLayer->getObjectRect(obj, false, false);

    float maxSize = std::max(rect.size.width, rect.size.height);
    float scale = obj->getScale();

    if (32.f / maxSize < scale || obj->m_pixelScaleX > 1.0f) {
        obj->setScale(32.f / maxSize);
    }*/

/*    auto item = CreateMenuItem::create(
        normal,
        nullptr,
        this,
        menu_selector(EditorUI::onCreateButton)
    );

    item->m_objectID = id;
    item->setScale(0.9f);
    item->m_baseScale = 0.9f;

    m_createButtonArray->addObject(item);

    /*if (obj->m_colorSprite && !obj->m_unk28c) {
        int z = obj->m_colorZLayerRelated ? 1 : -1;
        obj->addChild(obj->m_colorSprite, z);

        auto size = obj->getContentSize();
        obj->m_colorSprite->setPosition(size / 2);
        obj->m_colorSprite->setColor({255, 200, 200});
        obj->m_colorSprite->setScale(1);
    }*/

/*    return item;
}*/


/*#include "PerformanceFixes.hpp"
#include "Geode/cocos/cocoa/CCArray.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/textures/CCTexture2D.h"
#include "Geode/loader/Log.hpp"
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/CreateMenuItem.hpp>
#include <Geode/binding/EditButtonBar.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/GameObject.hpp>
#include "Events.hpp"
#include "Geode/utils/StringMap.hpp"
#include "Geode/utils/cocos.hpp"
#include "modules/ScrollableObjects.hpp"

std::vector<PerfEditorUI::Tab> PerfEditorUI::s_createTabs;

void PerfEditorUI::setupCreateMenu() {
    if (s_createTabs.empty()) {
        EditorUI::setupCreateMenu();
        for (int i = 0; i < m_createButtonBars->count(); i++) {
            auto bar = static_cast<EditButtonBar*>(m_createButtonBars->objectAtIndex(i));
            auto toggler = static_cast<CCMenuItemToggler*>(m_tabsArray->objectAtIndex(i));

            if (bar->m_hasCreateItems && bar->m_tabIndex < 14) {
                s_createTabs.push_back({bar, toggler, bar->m_tabIndex == 13 ? m_customTabControls : nullptr});
            }
        }
    }
    else {
        m_selectedTab = -1;
        m_selectedObjectIndex = 0;

        auto gameManager = GameManager::get();

        auto winSize = CCDirector::get()->getWinSize();
        m_createButtonArray = CCArray::create();
        m_createButtonArray->retain();

        m_customObjectButtonArray = CCArray::create();
        m_customObjectButtonArray->retain();

        m_createButtonBars = CCArray::create();
        m_createButtonBars->retain();

        m_tabsArray = CCArray::create();
        m_tabsArray->retain();

        m_tabsMenu = CCMenu::create();
        addChild(m_tabsMenu, 100);

        int buttonsPerRow = gameManager->getIntGameVariable(GameVar::EditorButtonsPerRow);
        int buttonRows = gameManager->getIntGameVariable(GameVar::EditorButtonRows);

        if (buttonsPerRow < 6) {
            buttonsPerRow = 6;
            gameManager->setIntGameVariable(GameVar::EditorButtonsPerRow, 6);
        }

        if (buttonRows < 2) {
            buttonRows = 2;
            gameManager->setIntGameVariable(GameVar::EditorButtonRows, 2);
        }
    
        auto screenBottom = CCDirector::get()->getScreenBottom();
        auto point = CCPoint{winSize.width / 2 - 5.f, screenBottom + m_toolbarHeight - 6.f};

        for (auto& tab : s_createTabs) {
            addChild(tab.m_editButtonBar, 10);
            m_createButtonBars->addObject(tab.m_editButtonBar);

            m_tabsMenu->addChild(tab.m_toggler);
            m_tabsArray->addObject(tab.m_toggler);
            tab.m_toggler->setSizeMult(1.2f);
            tab.m_toggler->m_pListener = this;

            if (tab.m_editButtonBar->m_tabIndex == 13) {
                m_customTabControls = tab.m_customTabControls->shallowCopy();
                m_customTabControls->retain();
                for (auto item : m_customTabControls->asExt<CreateMenuItem>()) {
                    item->m_pListener = this;
                }
                m_customTabBar = tab.m_editButtonBar;
            }

            for (auto item : tab.m_editButtonBar->m_buttonArray->asExt<CreateMenuItem>()) {
                item->m_pListener = this;
                
                if (tab.m_editButtonBar->m_tabIndex == 13) {
                    if (m_customTabControls->containsObject(item)) continue;
                    m_customObjectButtonArray->addObject(item);
                }
                else {
                    m_createButtonArray->addObject(item);
                }
            }

            auto lastBuildPage = m_editorLayer->m_level->getLastBuildPageForTab(tab.m_editButtonBar->m_tabIndex);
            tab.m_editButtonBar->goToPage(lastBuildPage);
        }

        m_tabsMenu->alignItemsHorizontallyWithPadding(2.f);
        m_tabsMenu->setPosition(point + CCPoint{0, 14});

        selectBuildTab(m_editorLayer->m_level->m_lastBuildTab);

        m_createButtonBar->goToPage(m_editorLayer->m_level->m_lastBuildPage);

        m_editorLayer->m_currentLayer = m_editorLayer->m_level->m_lastBuildGroupID;

        updateCreateMenu(false);
    }

    if (ScrollableObjects::isEnabled()) {
        addEventListener(EditorExitEvent(), [this] {
            for (auto tab : s_createTabs) {
                auto bar = static_cast<SOEditButtonBar*>(tab.m_editButtonBar.data());
                if (bar->m_hasCreateItems && bar->m_tabIndex < 13) {
                    bar->clearExtrasMenu();
                }
            }
        });
    }

    runAction(CallFuncExt::create([] {
        SetupCreateMenuEvent().send();
    }));
}*/