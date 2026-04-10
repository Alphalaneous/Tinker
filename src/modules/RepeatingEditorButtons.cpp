#include "RepeatingEditorButtons.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>

void REBCCMenuItemSpriteExtra::setRepeatable(bool repeatable) {
    auto fields = m_fields.self();
    fields->m_repeatable = repeatable;
}

void REBCCMenuItemSpriteExtra::checkHold(float dt) {
    auto fields = m_fields.self();
    fields->m_isHolding = true;
    (m_pListener->*m_pfnSelector)(this);
    if (!m_animationEnabled) {
        setScale(m_baseScale);
    }
}

void REBCCMenuItemSpriteExtra::activate() {
    auto fields = m_fields.self();

    if (!fields->m_repeatable) return CCMenuItemSpriteExtra::activate();
    if (!fields->m_isHolding) {
        CCMenuItemSpriteExtra::activate();
    }
}

void REBCCMenuItemSpriteExtra::selected() {
    CCMenuItemSpriteExtra::selected();

    auto fields = m_fields.self();
    if (fields->m_repeatable) {
        schedule(schedule_selector(REBCCMenuItemSpriteExtra::checkHold), RepeatingEditorButtons::getSetting<int, "repeat-rate">()/1000.f, kCCRepeatForever, RepeatingEditorButtons::getSetting<int, "repeat-delay">()/1000.f);
    }
}

void REBCCMenuItemSpriteExtra::unselected() {
    CCMenuItemSpriteExtra::unselected();

    auto fields = m_fields.self();
    if (fields->m_repeatable) {
        unschedule(schedule_selector(REBCCMenuItemSpriteExtra::checkHold));
        runAction(CallFuncExt::create([fields] {
            fields->m_isHolding = false;
        }));
    }
}

void RepeatingEditorButtons::onEditor() {
    m_editorUI->runAction(CallFuncExt::create([this] {
        for (auto btn : CCArrayExt<REBCCMenuItemSpriteExtra*>(m_editorUI->m_editButtonBar->m_buttonArray)) {
            if (btn->getID() == "alphalaneous.tinker/reference-import") continue;
            btn->setRepeatable(true);
        }
    }));

    static_cast<REBCCMenuItemSpriteExtra*>(m_editorUI->m_undoBtn)->setRepeatable(true);
    static_cast<REBCCMenuItemSpriteExtra*>(m_editorUI->m_redoBtn)->setRepeatable(true);
    static_cast<REBCCMenuItemSpriteExtra*>(m_editorUI->m_layerNextBtn)->setRepeatable(true);
    static_cast<REBCCMenuItemSpriteExtra*>(m_editorUI->m_layerPrevBtn)->setRepeatable(true);

    if (auto zoomMenu = m_editorUI->getChildByID("zoom-menu")) {
        for (auto btn : zoomMenu->getChildrenExt<REBCCMenuItemSpriteExtra*>()) {
            btn->setRepeatable(true);
        }
    }

    if (auto customEditMenu = m_editorUI->getChildByID("hjfod.betteredit/custom-move-menu")) {
        recursivelySetRepeat(customEditMenu);
    }

    for (auto child : m_editorUI->getChildrenExt()) {
        auto bar = typeinfo_cast<EditButtonBar*>(child);
        if (!bar) continue;
        auto menu = bar->getChildByType<CCMenu>(0);
        if (menu) {
            auto leftBtn = menu->getChildByType<CCMenuItemSpriteExtra*>(0);
            auto rightBtn = menu->getChildByType<CCMenuItemSpriteExtra*>(1);

            static_cast<REBCCMenuItemSpriteExtra*>(leftBtn)->setRepeatable(true);
            static_cast<REBCCMenuItemSpriteExtra*>(rightBtn)->setRepeatable(true);
        }
    }
}

void RepeatingEditorButtons::onSetGroupIDLayer(SetGroupIDLayer* setGroupIDLayer, GameObject* obj, CCArray* objs) {
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
}

void RepeatingEditorButtons::applyRepeatIfExist(SetGroupIDLayer* setGroupIDLayer, ZStringView id) {
    if (auto node = setGroupIDLayer->getChildByIDRecursive(id)) {
        if (typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            static_cast<REBCCMenuItemSpriteExtra*>(node)->setRepeatable(true);
        }
    }
}

void RepeatingEditorButtons::recursivelySetRepeat(CCNode* node) {
    for (auto node : node->getChildrenExt()) {
        if (CCMenuItemSpriteExtra* btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            static_cast<REBCCMenuItemSpriteExtra*>(btn)->setRepeatable(true);
            continue;
        }
        recursivelySetRepeat(node);
    }
}

void REBEditButtonBar::loadFromItems(CCArray* p0, int p1, int p2, bool p3) {
    EditButtonBar::loadFromItems(p0, p1, p2, p3);\

    runAction(CallFuncExt::create([this] {
        auto menu = getChildByType<CCMenu>(0);
        if (menu) {
            auto leftBtn = menu->getChildByType<CCMenuItemSpriteExtra*>(0);
            auto rightBtn = menu->getChildByType<CCMenuItemSpriteExtra*>(1);

            static_cast<REBCCMenuItemSpriteExtra*>(leftBtn)->setRepeatable(true);
            static_cast<REBCCMenuItemSpriteExtra*>(rightBtn)->setRepeatable(true);
        }
    }));
}

class $nodeModify(REBMoveGroup, MoveGroup) {

    void modify() {
        if (!RepeatingEditorButtons::isEnabled()) return;
        for (auto node : getChildrenExt()) {
            if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                static_cast<REBCCMenuItemSpriteExtra*>(btn)->setRepeatable(true);
            }
        }
    }
};