#include "PreviewObjectColors.hpp"
#include "ScrollableObjects.hpp"
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>
#include "utils/Constants.hpp"

void PreviewObjectColors::onEditor() {
    auto savedObj = alpha::level_storage::getSavedValue<std::string>(m_editorLayer, "color-object");

    auto editorUI = static_cast<POCEditorUI*>(m_editorUI);
    auto fields = editorUI->m_fields.self();

    if (!savedObj.empty()) {
        auto objects = m_editorLayer->createObjectsFromString(savedObj, true, true);
        fields->m_defaultObject = static_cast<GameObject*>(objects->firstObject());
        m_editorUI->deleteObject(fields->m_defaultObject, true);
    }
    else {
        fields->m_defaultObject = GameObject::createWithKey(207);
        fields->m_defaultObject->m_baseColor = new GJSpriteColor();
        fields->m_defaultObject->m_detailColor = new GJSpriteColor();
    }

    fields->m_defaultObject->m_baseColor->m_defaultColorID = 0;
    fields->m_defaultObject->m_detailColor->m_defaultColorID = 0;

    m_editorUI->schedule(schedule_selector(POCEditorUI::updateObjectColors));

    onUpdateButtons();

    addEventListener(UpdateButtonsEvent(), [this] {
        onUpdateButtons();
    });

    m_editorUI->runAction(CallFuncExt::create([this, fields] {
        if (ScrollableObjects::isEnabled()) {
            for (auto child : m_editorUI->getChildrenExt()) {
                if (auto bar = typeinfo_cast<EditButtonBar*>(child)) {
                    if (!bar->m_hasCreateItems) continue;
                    auto soBar = static_cast<SOEditButtonBar*>(bar);
                    auto btn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_editHSVBtn2_001.png", 1, [this, fields] (CCMenuItemSpriteExtra* sender) {
                        auto customizeObjectLayer = CustomizeObjectLayer::create(fields->m_defaultObject, nullptr);
                        customizeObjectLayer->show();
                        customizeObjectLayer->updateColorSprite();
                        customizeObjectLayer->updateChannelLabel(fields->m_defaultObject->m_baseColor->m_colorID);
                    });
                    btn->setZOrder(-1);
                    soBar->addToExtrasMenu(btn);
                }
            }
        }
    }));

    addEventListener(LevelSavedEvent(), [this] {
        auto editorUI = static_cast<POCEditorUI*>(m_editorUI);
        auto fields = editorUI->m_fields.self();
        alpha::level_storage::setSavedValue(m_editorLayer, "color-object", std::string(fields->m_defaultObject->getSaveString(m_editorLayer)));
    });
}

void PreviewObjectColors::onUpdateButtons() {
    if (!ScrollableObjects::isEnabled()) {
        m_editorUI->m_editObjectBtn->setOpacity(255);
        m_editorUI->m_editObjectBtn->setColor({255, 255, 255});
        m_editorUI->m_editObjectBtn->m_animationEnabled = true;
    }
}

void POCEditorUI::editObject(cocos2d::CCObject* sender) {
    if (ScrollableObjects::isEnabled()) return EditorUI::editObject(sender);
    auto fields = m_fields.self();

    if (!m_selectedObject && (!m_selectedObjects || m_selectedObjects->count() == 0)) {
        auto customizeObjectLayer = CustomizeObjectLayer::create(fields->m_defaultObject, nullptr);
        customizeObjectLayer->show();
    }
    else {
        EditorUI::editObject(sender);
    }
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

void POCEditorUI::updateObjectColors(float dt) {
    auto fields = m_fields.self();
    for (auto child : getChildrenExt()) {
        if (auto bar = typeinfo_cast<EditButtonBar*>(child)) {
            if (!bar->isVisible()) continue;
            if (!bar->m_hasCreateItems) continue;

            if (ScrollableObjects::isEnabled()) {
                auto scrollEditButtonBar = static_cast<SOEditButtonBar*>(bar);
                for (auto item : scrollEditButtonBar->m_fields->m_visibleNodes) {
                    if (!nodeIsVisible(item)) continue;
                    tinker::utils::updateCreateButtonColor(m_editorLayer, item, fields->m_defaultObject->m_baseColor->m_colorID, fields->m_defaultObject->m_detailColor->m_colorID, fields->m_defaultObject->m_baseColor->m_hsv, fields->m_defaultObject->m_detailColor->m_hsv);
                }
            }
            else {
                for (auto item : bar->m_buttonArray->asExt<CreateMenuItem>()) {
                    if (!item->getParentByType<EditButtonBar>() || !nodeIsVisible(item)) continue;
                    tinker::utils::updateCreateButtonColor(m_editorLayer, item, fields->m_defaultObject->m_baseColor->m_colorID, fields->m_defaultObject->m_detailColor->m_colorID, fields->m_defaultObject->m_baseColor->m_hsv, fields->m_defaultObject->m_detailColor->m_hsv);
                }
            }
        }
    }
    if (auto pinned = getChildByID("razoom.object_groups/pinned-groups")) {
        if (!pinned->isVisible()) return;
        for (auto group : pinned->getChildrenExt()) {
            if (!group->isVisible()) continue;
            if (auto innerMenu = group->getChildByType<CCMenu>(1)) {
                for (auto btn : innerMenu->getChildrenExt()) {
                    if (!btn->isVisible()) continue;
                    tinker::utils::updateCreateButtonColor(m_editorLayer, btn, fields->m_defaultObject->m_baseColor->m_colorID, fields->m_defaultObject->m_detailColor->m_colorID, fields->m_defaultObject->m_baseColor->m_hsv, fields->m_defaultObject->m_detailColor->m_hsv);
                }
            }
        }
    }
}