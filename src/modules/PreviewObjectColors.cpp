#include "modules/PreviewObjectColors.hpp"
#include "modules/ScrollableObjects.hpp"
#include "modules/UIScaling.hpp"
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include "utils/Constants.hpp"
#include "utils/Utils.hpp"

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

        for (auto child : getEditor()->getChildrenExt()) {
            if (auto bar = typeinfo_cast<EditButtonBar*>(child)) {
                if (!bar->m_hasCreateItems) continue;
                auto soBar = static_cast<SOEditButtonBar*>(bar);
                soBar->removeFromExtrasMenu("color-preview"_spr);
            }
        }
        removeEventListener("scrollable-toggled");
        removeEventListener("level-saved-event");
        removeEventListener("ui-scale");

        m_buttonForScroll = nullptr;
        fields->m_defaultObject = nullptr;
    }
    return true;
}

void PreviewObjectColors::onEditor() {
    auto savedObj = alpha::level_storage::getSavedValue<std::string>(getEditorLayer(), "color-object");

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

    getEditorLayer()->schedule(schedule_selector(POCLevelEditorLayer::updateObjectColors));

    setupButton(ScrollableObjects::isEnabled());

    addEventListener("scrollable-toggled", ScrollableObjects::ScrollableObjectsToggledEvent(), [this] (bool state) {
        setupButton(state);
    });

    addEventListener("level-saved-event", LevelSavedEvent(), [this] {
        auto editorUI = static_cast<POCEditorUI*>(getEditor());
        auto fields = editorUI->m_fields.self();
        alpha::level_storage::setSavedValue(getEditorLayer(), "color-object", std::string(fields->m_defaultObject->getSaveString(getEditorLayer())));
    });
}

void PreviewObjectColors::setupButton(bool scrollable) {
    if (m_buttonContainer) {
        getEditor()->m_uiItems->removeObject(m_buttonContainer);
        m_buttonContainer->removeFromParent();
        m_buttonContainer = nullptr;
    }
    m_buttonForScroll = nullptr;
    removeEventListener("ui-scale");

    if (scrollable) {
        getEditor()->runAction(CallFuncExt::create([this] {
            for (auto child : getEditor()->getChildrenExt()) {
                if (auto bar = typeinfo_cast<EditButtonBar*>(child)) {
                    if (!bar->m_hasCreateItems) continue;
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
        m_buttonContainer->setZOrder(100);
        m_buttonContainer->setAnchorPoint({1.f, 1.f});
        getEditor()->addChild(m_buttonContainer);
        getEditor()->m_uiItems->addObject(m_buttonContainer);

        m_buttonForScroll = geode::Button::createWithSpriteFrameName("GJ_editHSVBtn2_001.png", [this] (auto sender) {
            editColor();
            float scale = 1.f;
            if (UIScaling::isEnabled()) {
                scale = UIScaling::get()->m_scale;
            }
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
            
            m_buttonContainer->setPosition({x - 3.f * scale, tinker::utils::getToolbarHeight(false) - 4.f * scale});
        };

        addEventListener("ui-scale", UIScaleUpdated(), [this, uiScale] (float scale, bool scaleToolbars, bool fullReload) {
            float trueScale = 1.f;
            if (scaleToolbars) {
                trueScale = scale;
            }
            uiScale(trueScale);
        });

        float scale = 1.f;
        if (UIScaling::isEnabled() && UIScaling::get()->m_scaleToolbar) {
            scale = UIScaling::get()->m_scale;
        }
        uiScale(scale);

        if (!m_callbacksAdded) {
            m_callbacksAdded = true;
            alpha::editor_tabs::addTabSwitchCallback([this] (auto id) {
                if (!m_buttonForScroll) return;
                setButtonVisible(m_buttonForScroll);
            });
            alpha::editor_tabs::addModeSwitchCallback([this] (auto id) {
                if (!m_buttonForScroll) return;
                setButtonVisible(m_buttonForScroll);
            });
        }
    }
}

void PreviewObjectColors::setButtonVisible(geode::Button* button) {
    auto tabRes = alpha::editor_tabs::nodeForTab(alpha::editor_tabs::getCurrentTab().unwrapOrDefault());
    if (!tabRes) return;
    auto tab = typeinfo_cast<EditButtonBar*>(tabRes.unwrap().data());

    button->setVisible(tab && tab->m_tabIndex != 13 && tab->m_hasCreateItems && alpha::editor_tabs::getCurrentMode().unwrapOrDefault() == alpha::editor_tabs::BUILD);
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

    if (!tinker::utils::isColorable(ret)) return ret;

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
                    tinker::utils::updateCreateButtonColor(this, item, fields->m_defaultObject->m_baseColor->m_colorID, fields->m_defaultObject->m_detailColor->m_colorID, fields->m_defaultObject->m_baseColor->m_hsv, fields->m_defaultObject->m_detailColor->m_hsv);
                }
            }
            else {
                for (auto item : bar->m_buttonArray->asExt<CreateMenuItem>()) {
                    if (!item->getParentByType<EditButtonBar>() || !nodeIsVisible(item)) continue;
                    tinker::utils::updateCreateButtonColor(this, item, fields->m_defaultObject->m_baseColor->m_colorID, fields->m_defaultObject->m_detailColor->m_colorID, fields->m_defaultObject->m_baseColor->m_hsv, fields->m_defaultObject->m_detailColor->m_hsv);
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
                    tinker::utils::updateCreateButtonColor(this, btn, fields->m_defaultObject->m_baseColor->m_colorID, fields->m_defaultObject->m_detailColor->m_colorID, fields->m_defaultObject->m_baseColor->m_hsv, fields->m_defaultObject->m_detailColor->m_hsv);
                }
            }
        }
    }
}