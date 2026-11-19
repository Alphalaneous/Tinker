#include "modules/PreviewObjectColors.hpp"
#include "module/ModuleCore.hpp"
#include "modules/ScrollableObjects.hpp"
#include "modules/UIScaling.hpp"
#include <alphalaneous.level-storage-api/LevelStorageAPI.hpp>
#include <alphalaneous.editortab_api/API.hpp>
#include <string>
#include "utils/Constants.hpp"
#include "utils/Objects.hpp"
#include "utils/UI.hpp"

using namespace geode::prelude;

bool PreviewObjectColors::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        auto fields = static_cast<POCEditorUI*>(getEditor())->m_fields.self();
        if (m_buttonContainer) {
            getEditor()->m_uiItems->removeObject(m_buttonContainer);
            m_buttonContainer->removeFromParent();
            m_buttonContainer = nullptr;
        }

        fields->m_defaultObject->m_baseColor->m_colorID = 0;
        fields->m_defaultObject->m_detailColor->m_colorID = 0;

        static_cast<POCLevelEditorLayer*>(getEditorLayer())->updateObjectColors(0);
        getEditorLayer()->unschedule(schedule_selector(POCLevelEditorLayer::updateObjectColors));

        if (ScrollableObjects::isEnabled()) {
            for (auto child : getEditor()->getChildrenExt()) {
                if (auto bar = typeinfo_cast<EditButtonBar*>(child)) {
                    if (!bar->m_hasCreateItems || bar->m_tabIndex == 13) continue;
                    auto soBar = static_cast<SOEditButtonBar*>(bar);
                    soBar->removeFromExtrasMenu("color-preview"_spr);
                }
            }
        }
        removeEventListener("scrollable-toggled");
        removeEventListener("level-saved-event");
        removeEventListener("ui-scale");
        removeEventListener("switch-tab");
        removeEventListener("switch-mode");

        m_buttonForScroll = nullptr;
        fields->m_defaultObject = nullptr;
    }
    return true;
}

void PreviewObjectColors::onEditor() {
    auto savedObj = getSaved<Save::Level, std::string, "color-object-string">();

    auto editorUI = static_cast<POCEditorUI*>(getEditor());
    auto fields = editorUI->m_fields.self();

    if (!savedObj.empty()) {
        auto objects = getEditorLayer()->createObjectsFromString(savedObj, true, true);
        fields->m_defaultObject = static_cast<GameObject*>(objects->firstObject());
        getEditor()->deleteObject(fields->m_defaultObject, true);
    }
    else {
        fields->m_defaultObject = GameObject::createWithKey(207);
        fields->m_defaultObject->m_baseColor = new GJSpriteColor();
        fields->m_defaultObject->m_detailColor = new GJSpriteColor();
    }

    fields->m_defaultObject->m_baseColor->m_defaultColorID = 0;
    fields->m_defaultObject->m_detailColor->m_defaultColorID = 0;

    editorUI->m_editorLayer->schedule(schedule_selector(POCLevelEditorLayer::updateObjectColors));

    setupButton(ScrollableObjects::isEnabled());

    addEventListener("scrollable-toggled", ScrollableObjects::ScrollableObjectsToggledEvent(), [this] (bool state) {
        setupButton(state);
    });

    addEventListener("level-saved-event", LevelSavedEvent(), [this] {
        auto editorUI = static_cast<POCEditorUI*>(getEditor());
        auto fields = editorUI->m_fields.self();
        setSaved<Save::Level, "color-object-string">(fields->m_defaultObject->getSaveString(getEditorLayer()));
    });
}

void PreviewObjectColors::setupButton(bool scrollable) {
    auto editor = getEditor();

    if (m_buttonContainer) {
        editor->m_uiItems->removeObject(m_buttonContainer);
        m_buttonContainer->removeFromParent();
        m_buttonContainer = nullptr;
    }
    m_buttonForScroll = nullptr;
    removeEventListener("ui-scale");

    if (scrollable) {
        editor->runAction(CallFuncExt::create([this, editor] {
            for (auto child : editor->getChildrenExt()) {
                if (auto bar = typeinfo_cast<EditButtonBar*>(child)) {
                    if (!bar->m_hasCreateItems || bar->m_tabIndex == 13) continue;
                    auto soBar = static_cast<SOEditButtonBar*>(bar);
                    auto btn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_editHSVBtn2_001.png", 1.f, [this] (CCMenuItemSpriteExtra* sender) {
                        editColor();
                    });
                    btn->setZOrder(-1);
                    btn->setID("color-preview"_spr);
                    soBar->addToExtrasMenu(btn);
                }
            }
        }));
    }
    else {
        m_buttonContainer = CCNode::create();
        m_buttonContainer->setID("color-preview-container"_spr);
        m_buttonContainer->setZOrder(12);
        m_buttonContainer->setAnchorPoint({1.f, 1.f});
        editor->addChild(m_buttonContainer);
        editor->m_uiItems->addObject(m_buttonContainer);

        m_buttonForScroll = geode::Button::createWithSpriteFrameName("GJ_editHSVBtn2_001.png", [this] (auto sender) {
            editColor();
        });
        m_buttonForScroll->setID("color-preview"_spr);
        m_buttonContainer->setContentSize(m_buttonForScroll->getScaledContentSize());
        m_buttonForScroll->setPosition(m_buttonContainer->getContentSize() / 2.f);

        m_buttonContainer->addChild(m_buttonForScroll);

        auto uiScale = [&] (float scale) {
            if (!m_buttonContainer) return;

            auto winSize = CCDirector::get()->getWinSize();
            float x = winSize.width - (95.f * scale);
            m_buttonContainer->setScale(0.65f * scale);

            auto spacer = getEditor()->getChildByID("spacer-line-right");
            if (spacer) {
                x = spacer->boundingBox().getMinX();
            }
            
            m_buttonContainer->setPosition({x - 3.f * scale, tinker::utils::ui::getToolbarHeight(false) - 4.f * scale});
        };

        addEventListener("ui-scale", UIScaleUpdated(), [this, uiScale] (float scale, bool scaleToolbars) {
            float trueScale = 1.f;
            if (scaleToolbars) {
                trueScale = scale;
            }
            uiScale(trueScale);
        });

        uiScale(UIScaling::getScale());

        addEventListener("switch-tab", alpha::editor_tabs::Tab::SwitchTabEvent(), [this] (alpha::editor_tabs::Tab* tab, bool show) {
            if (!m_buttonForScroll) return;
            setButtonVisible(m_buttonForScroll);
        });

        addEventListener("switch-mode", alpha::editor_tabs::Mode::SwitchModeEvent(), [this] (alpha::editor_tabs::Mode* mode, bool show) {
            if (!m_buttonForScroll) return;
            setButtonVisible(m_buttonForScroll);
        });
    }
}

GJSpriteColor* PreviewObjectColors::getBaseColor() {
    auto editorUI = static_cast<POCEditorUI*>(getEditor());
    auto fields = editorUI->m_fields.self();

    return fields->m_defaultObject->m_baseColor;
}

GJSpriteColor* PreviewObjectColors::getDetailColor() {
    auto editorUI = static_cast<POCEditorUI*>(getEditor());
    auto fields = editorUI->m_fields.self();

    return fields->m_defaultObject->m_detailColor;
}

void PreviewObjectColors::setButtonVisible(geode::Button* button) {
    auto tab = alpha::editor_tabs::getCurrentTab();
    if (!tab) return;

    auto node = tab->getNode();

    button->setVisible(node && node->m_tabIndex != 13 && node->m_hasCreateItems && alpha::editor_tabs::getCurrentModeID() == alpha::editor_tabs::Build);
}

void PreviewObjectColors::editColor() {
    auto editorUI = static_cast<POCEditorUI*>(getEditor());
    auto fields = editorUI->m_fields.self();

    auto customizeObjectLayer = CustomizeObjectLayer::create(fields->m_defaultObject, nullptr);
    customizeObjectLayer->show();
    customizeObjectLayer->updateColorSprite();
    customizeObjectLayer->updateChannelLabel(fields->m_defaultObject->m_baseColor->m_colorID);
}

GameObject* POCEditorUI::createObject(int objectID, cocos2d::CCPoint position) {
    auto ret = EditorUI::createObject(objectID, position);

    if (!tinker::utils::objects::isColorable(ret)) return ret;

    if (ret) {
        using namespace tinker::constants;

        auto fields = m_fields.self();

        auto defaultObject = fields->m_defaultObject;
        if (!defaultObject) return ret;

        int baseColorID = defaultObject->m_baseColor->m_colorID;

        if (baseColorID == color_channels::Lighter && !ret->m_detailColor) {
            baseColorID = color_channels::White;
        }

        if (auto baseColor = ret->m_baseColor) {
            baseColor->m_colorID = baseColorID;
            baseColor->m_hsv = defaultObject->m_baseColor->m_hsv;
        }
        if (auto detailColor = ret->m_detailColor) {
            detailColor->m_colorID = defaultObject->m_detailColor->m_colorID;
            detailColor->m_hsv = defaultObject->m_detailColor->m_hsv;
        }
    }

    return ret;
}

void POCLevelEditorLayer::updateObjectColors(float dt) {
    auto fields = static_cast<POCEditorUI*>(m_editorUI)->m_fields.self();
    for (auto child : m_editorUI->getChildrenExt()) {
        if (auto bar = typeinfo_cast<EditButtonBar*>(child)) {
            if (!bar->isVisible()) continue;
            if (!bar->m_hasCreateItems) continue;

            if (ScrollableObjects::isEnabled()) {
                auto scrollEditButtonBar = static_cast<SOEditButtonBar*>(bar);
                for (auto item : scrollEditButtonBar->m_fields->m_visibleNodes) {
                    if (!nodeIsVisible(item)) continue;
                    tinker::utils::objects::updateCreateButtonColor(item, fields->m_defaultObject->m_baseColor->m_colorID, fields->m_defaultObject->m_detailColor->m_colorID, fields->m_defaultObject->m_baseColor->m_hsv, fields->m_defaultObject->m_detailColor->m_hsv);
                }
            }
            else {
                for (auto item : bar->m_buttonArray->asExt<CreateMenuItem>()) {
                    if (!item->getParentByType<EditButtonBar>() || !nodeIsVisible(item)) continue;
                    tinker::utils::objects::updateCreateButtonColor(item, fields->m_defaultObject->m_baseColor->m_colorID, fields->m_defaultObject->m_detailColor->m_colorID, fields->m_defaultObject->m_baseColor->m_hsv, fields->m_defaultObject->m_detailColor->m_hsv);
                }
            }
        }
    }
    if (auto pinned = m_editorUI->getChildByID("razoom.object_groups/pinned-groups")) {
        if (!pinned->isVisible()) return;
        for (auto group : pinned->getChildrenExt()) {
            if (!group->isVisible()) continue;
            if (auto innerMenu = group->getChildByType<CCMenu>(1)) {
                for (auto btn : innerMenu->getChildrenExt()) {
                    if (!btn->isVisible()) continue;
                    tinker::utils::objects::updateCreateButtonColor(btn, fields->m_defaultObject->m_baseColor->m_colorID, fields->m_defaultObject->m_detailColor->m_colorID, fields->m_defaultObject->m_baseColor->m_hsv, fields->m_defaultObject->m_detailColor->m_hsv);
                }
            }
        }
    }
}