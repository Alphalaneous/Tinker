#include "modules/RepeatingEditorButtons.hpp"
#include "alphalaneous.editortab_api/api/Editor.hpp"
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <alphalaneous.alphas_geode_utils/ObjectModify.hpp>

using namespace geode::prelude;

bool RepeatingEditorButtons::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        removeEventListener("group-id-layer-event");
    }
    return true;
}

bool RepeatingEditorButtons::onSettingChanged(std::string_view key, const matjson::Value& value) {
    return true;
}

void RepeatingEditorButtons::setRepeatable(CCMenuItemSpriteExtra* item, bool repeatable) {
    if (!item) return;
    static_cast<REBCCMenuItemSpriteExtra*>(item)->setRepeatable(repeatable);
}

void RepeatingEditorButtons::forceNoRepeat(CCMenuItemSpriteExtra* item, bool force) {
    if (!item) return;
    static_cast<REBCCMenuItemSpriteExtra*>(item)->forceNoRepeat(force);
}

void REBCCMenuItemSpriteExtra::setRepeatable(bool repeatable) {
    auto fields = m_fields.self();
    fields->m_repeatable = repeatable;

    removeEventListener("lost-focus"_spr);
    addEventListener("lost-focus"_spr, LostFocusEvent(), [this, fields] {
        unschedule(schedule_selector(REBCCMenuItemSpriteExtra::checkHold));
        fields->m_isHolding = false;
    });
}

void REBCCMenuItemSpriteExtra::forceNoRepeat(bool force) {
    auto fields = m_fields.self();
    fields->m_forceNoRepeat = force;
}

void REBCCMenuItemSpriteExtra::checkHold(float dt) {
    auto fields = m_fields.self();
    fields->m_isHolding = true;
    auto scale = getScale();
    setScale(m_baseScale);
    (m_pListener->*m_pfnSelector)(this);
    if (!m_animationEnabled || !m_bEnabled) {
        setScale(m_baseScale);
    }
    else {
        setScale(scale);
    }
}

void REBCCMenuItemSpriteExtra::activate() {
    auto fields = m_fields.self();

    if (!fields->m_repeatable || fields->m_forceNoRepeat) return CCMenuItemSpriteExtra::activate();
    if (!fields->m_isHolding) {
        CCMenuItemSpriteExtra::activate();
    }
}

void REBCCMenuItemSpriteExtra::selected() {
    CCMenuItemSpriteExtra::selected();

    auto fields = m_fields.self();
    if (fields->m_repeatable && !fields->m_forceNoRepeat) {
        auto repeatRate = RepeatingEditorButtons::getSetting<int, "repeat-rate">() / 1000.f;
        auto repeatDelay = RepeatingEditorButtons::getSetting<int, "repeat-delay">() / 1000.f;

        schedule(schedule_selector(REBCCMenuItemSpriteExtra::checkHold), repeatRate, kCCRepeatForever, repeatDelay);
    }
}

void REBCCMenuItemSpriteExtra::unselected() {
    CCMenuItemSpriteExtra::unselected();

    auto fields = m_fields.self();
    if (fields->m_repeatable && !fields->m_forceNoRepeat) {
        unschedule(schedule_selector(REBCCMenuItemSpriteExtra::checkHold));
        runAction(CallFuncExt::create([fields] {
            fields->m_isHolding = false;
        }));
    }
}

void RepeatingEditorButtons::onEditor() {
    auto editor = getEditor();

    editor->runAction(CallFuncExt::create([this, editor] {
        handleEditTab();

        auto buildTabsMenu = editor->getChildByID("build-tabs-menu");
        if (buildTabsMenu) {
            auto navMenu = buildTabsMenu->getChildByID("alphalaneous.editortab_api/navigation-menu");
            if (navMenu) {
                recursivelySetRepeat(navMenu);
            }
        }
    }));

    RepeatingEditorButtons::setRepeatable(editor->m_undoBtn, true);
    RepeatingEditorButtons::setRepeatable(editor->m_redoBtn, true);
    RepeatingEditorButtons::setRepeatable(editor->m_layerNextBtn, true);
    RepeatingEditorButtons::setRepeatable(editor->m_layerPrevBtn, true);

    auto zoomMenu = editor->getChildByID("zoom-menu");
    if (zoomMenu) {
        recursivelySetRepeat(zoomMenu);
    }

    addEventListener("move-menu-event", alpha::editor_tabs::SetupMoveMenuEvent(), [this] (alpha::editor_tabs::Tab* editTab) {
        handleEditTab();
    });

    addEventListener("group-id-layer-event", SetGroupIDLayerOpenedEvent(), [this] (SetGroupIDLayer* setGroupIDLayer, GameObject* obj, CCArray* objs) {
        applyRepeatIfExist(setGroupIDLayer, "add-group-id-next-button");
        applyRepeatIfExist(setGroupIDLayer, "add-group-id-prev-button");
        applyRepeatIfExist(setGroupIDLayer, "editor-layer-next-button");
        applyRepeatIfExist(setGroupIDLayer, "editor-layer-prev-button");
        applyRepeatIfExist(setGroupIDLayer, "editor-layer-2-next-button");
        applyRepeatIfExist(setGroupIDLayer, "editor-layer-2-prev-button");
        applyRepeatIfExist(setGroupIDLayer, "z-order-next-button");
        applyRepeatIfExist(setGroupIDLayer, "z-order-prev-button");
        applyRepeatIfExist(setGroupIDLayer, "channel-order-next-button");
        applyRepeatIfExist(setGroupIDLayer, "channel-order-prev-button");
        applyRepeatIfExist(setGroupIDLayer, "channel-next-button");
        applyRepeatIfExist(setGroupIDLayer, "channel-prev-button");
    });
}

void RepeatingEditorButtons::handleEditTab() {
    auto editor = getEditor();

    if (editor->m_editButtonBar) {
        for (auto node : editor->m_editButtonBar->m_buttonArray->asExt()) {
            auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
            if (!btn) continue;

            RepeatingEditorButtons::setRepeatable(btn, true);
        }
    }

#ifndef GEODE_IS_MOBILE
    if (editor->m_dpadMoveNode) {
        auto dpadMenu = editor->m_dpadMoveNode->getChildByID("dpad-menu");
        for (auto node : dpadMenu->getChildrenExt()) {
            auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
            if (!btn) continue;

            RepeatingEditorButtons::setRepeatable(btn, true);
        }
    }
#endif
}

void RepeatingEditorButtons::applyRepeatIfExist(SetGroupIDLayer* setGroupIDLayer, ZStringView id) {
    auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(setGroupIDLayer->getChildByIDRecursive(id));
    if (!btn) return;

    RepeatingEditorButtons::setRepeatable(btn, true);
}

void RepeatingEditorButtons::recursivelySetRepeat(CCNode* node) {
    for (auto node : node->getChildrenExt()) {
        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) {
            recursivelySetRepeat(node);
            continue;
        }

        RepeatingEditorButtons::setRepeatable(btn, true);
    }
}

void REBEditButtonBar::loadFromItems(CCArray* objects, int rows, int columns, bool keepPage) {
    EditButtonBar::loadFromItems(objects, rows, columns, keepPage);

    auto arrowsMenu = getChildByID("arrows-menu");
    if (arrowsMenu) {
        auto leftBtn = static_cast<CCMenuItemSpriteExtra*>(arrowsMenu->getChildByID("left-button"));
        auto rightBtn = static_cast<CCMenuItemSpriteExtra*>(arrowsMenu->getChildByID("right-button"));

        RepeatingEditorButtons::setRepeatable(leftBtn, true);
        RepeatingEditorButtons::setRepeatable(rightBtn, true);
    }
}

bool REBConfigureValuePopup::init(ConfigureValuePopupDelegate* delegate, float value, float minimum, float maximum, std::string title, std::string description, int type) {
    if (!ConfigureValuePopup::init(delegate, value, minimum, maximum, title, description, type)) return false;

    auto leftBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildByTag(0));
    auto rightBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildByTag(1));

    RepeatingEditorButtons::setRepeatable(leftBtn, true);
    RepeatingEditorButtons::setRepeatable(rightBtn, true);

    return true;
}

bool REBCustomizeObjectLayer::init(GameObject* object, cocos2d::CCArray* objects) {
    if (!CustomizeObjectLayer::init(object, objects)) return false;

    RepeatingEditorButtons::setRepeatable(m_arrowUp, true);
    RepeatingEditorButtons::setRepeatable(m_arrowDown, true);

    return true;
}

bool REBLevelSettingsLayer::init(LevelSettingsObject* object, LevelEditorLayer* layer) {
    if (!LevelSettingsLayer::init(object, layer)) return false;

    if (!m_songSelectNode || !m_songSelectNode->m_normalSongObjects) return true;
    for (auto node : m_songSelectNode->m_normalSongObjects->asExt()) {
        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) continue;

        RepeatingEditorButtons::setRepeatable(btn, true);
    }

    return true;
}

bool REBSetupTriggerPopup::init(EffectGameObject* trigger, cocos2d::CCArray* triggers, float width, float height, int background, float p5) {
    if (!SetupTriggerPopup::init(trigger, triggers, width, height, background, p5)) return false;

    addOnEnterCallback([this] {
        RepeatingEditorButtons::setRepeatable(m_prevButton, true);
        RepeatingEditorButtons::setRepeatable(m_nextButton, true);
    });

    return true;
}

void REBSetupTriggerPopup::createEasingControls(cocos2d::CCPoint position, float scale, int page, int group) {
    auto before = m_buttonMenu->getChildren()->shallowCopy();
    SetupTriggerPopup::createEasingControls(position, scale, page, group);
    auto after = m_buttonMenu->getChildren()->shallowCopy();
    after->removeObjectsInArray(before);

    for (auto node : after->asExt()) {
        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) continue;

        RepeatingEditorButtons::setRepeatable(btn, true);
    }
}

void REBSetupTriggerPopup::createCustomEasingControls(std::string text, cocos2d::CCPoint position, float scale, int typeProperty, int rateProperty, int page, int group) {
    auto before = m_buttonMenu->getChildren()->shallowCopy();
    SetupTriggerPopup::createCustomEasingControls(text, position, scale, typeProperty, rateProperty, page, group);
    auto after = m_buttonMenu->getChildren()->shallowCopy();
    after->removeObjectsInArray(before);

    for (auto node : after->asExt()) {
        auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
        if (!btn) continue;

        RepeatingEditorButtons::setRepeatable(btn, true);
    }
}

cocos2d::CCArray* REBSetupTriggerPopup::createValueControlAdvanced(int property, std::string label, cocos2d::CCPoint position, float scale, bool noSlider, InputValueType valueType, int length, bool arrows, float sliderMin, float sliderMax, int page, int group, GJInputStyle inputStyle, int decimalPlaces, bool allowDisable) {
    auto ret = SetupTriggerPopup::createValueControlAdvanced(property, label, position, scale, noSlider, valueType, length, arrows, sliderMin, sliderMax, page, group, inputStyle, decimalPlaces, allowDisable);

    if (arrows) {
        for (auto node : ret->asExt()) {
            auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
            if (!btn) continue;

            RepeatingEditorButtons::setRepeatable(btn, true);
        }
    }

    return ret;
}

class $nodeModify(REBMoveGroup, MoveGroup) {

    void modify() {
        for (auto node : getChildrenExt()) {
            auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
            if (!btn) continue;

            RepeatingEditorButtons::setRepeatable(btn, true);
        }
    }
};