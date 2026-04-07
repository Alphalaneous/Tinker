#include "PreviewObjectColors.hpp"
#include "ScrollableObjects.hpp"
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>

void PreviewObjectColors::onEditor() {
    auto savedObj = alpha::level_storage::getSavedValue<std::string>(m_editorLayer, "color-object");

    if (!savedObj.empty()) {
        auto objects = m_editorLayer->createObjectsFromString(savedObj, true, true);
        m_defaultObject = static_cast<GameObject*>(objects->firstObject());
        m_editorUI->deleteObject(m_defaultObject, true);
    }
    else {
        m_defaultObject = GameObject::createWithKey(207);
        m_defaultObject->m_baseColor = new GJSpriteColor();
        m_defaultObject->m_detailColor = new GJSpriteColor();
    }

    m_defaultObject->m_baseColor->m_defaultColorID = 0;
    m_defaultObject->m_detailColor->m_defaultColorID = 0;

    m_editorUI->schedule(schedule_selector(POCEditorUI::updateObjectColors));

    onUpdateButtons();

    if (ScrollableObjects::isEnabled()) {
        for (auto child : m_editorUI->getChildrenExt()) {
            if (auto bar = typeinfo_cast<EditButtonBar*>(child)) {
                if (!bar->m_hasCreateItems) continue;
                auto soBar = static_cast<SOEditButtonBar*>(bar);
                auto btn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_editHSVBtn2_001.png", 1, [this] (CCMenuItemSpriteExtra* sender) {
                    auto customizeObjectLayer = CustomizeObjectLayer::create(m_defaultObject, nullptr);
                    customizeObjectLayer->show();
                });
                soBar->addToExtrasMenu(btn);
            }
        }
    }
}

void PreviewObjectColors::onSave() {
    alpha::level_storage::setSavedValue(m_editorLayer, "color-object", std::string(m_defaultObject->getSaveString(m_editorLayer)));
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

    if (!m_selectedObject && (!m_selectedObjects || m_selectedObjects->count() == 0)) {
        auto customizeObjectLayer = CustomizeObjectLayer::create(PreviewObjectColors::get()->m_defaultObject, nullptr);
        customizeObjectLayer->show();
    }
    else {
        EditorUI::editObject(sender);
    }
}

GameObject* POCEditorUI::createObject(int p0, cocos2d::CCPoint p1) {
    auto ret = EditorUI::createObject(p0, p1);

    if (!isColorable(ret)) return ret;

    auto defaultObject = PreviewObjectColors::get()->m_defaultObject;
    if (!defaultObject) return ret;

    int baseColorID = defaultObject->m_baseColor->m_colorID;

    if (baseColorID == 1012 && !ret->m_detailColor) {
        baseColorID = 1011;
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

PreviewObjectColors::ColorData POCEditorUI::getActiveColor(int colorID) {
    for (ColorActionSprite* action : m_editorLayer->m_effectManager->m_colorActionSpriteVector) {
        if (!action) continue;
        if (action->m_colorID != colorID || action->m_colorID <= 0) continue;

        ccColor3B color = action->m_color;

        if (colorID == 1005) color = GameManager::get()->colorForIdx(GameManager::get()->m_playerColor.value());
        if (colorID == 1006) color = GameManager::get()->colorForIdx(GameManager::get()->m_playerColor2.value());

        if (colorID == 1010) color = {0, 0, 0};
        if (colorID == 1011) color = {255, 255, 255};

        for (PulseEffectAction& pulse : m_editorLayer->m_effectManager->m_pulseEffectVector) {
            if (pulse.m_targetGroupID == action->m_colorID) {
                color = m_editorLayer->m_effectManager->colorForPulseEffect(color, &pulse);
            }
        }

        bool blending = false;
        GLubyte opacity = 255;
        if (auto colorAction = action->m_colorAction) {
            blending = colorAction->m_blending;
            opacity = colorAction->m_currentOpacity * 255;
        }

        return {color, blending, opacity};
    }
    return {{255, 255, 255}, false, 255};
}


bool POCEditorUI::isColorable(GameObject* object) {
    const auto type = object->m_objectType;
    const auto id = object->m_objectID;

    static const std::unordered_set<int> allowedIDs = {
        3027, 1594
    };

    static const std::unordered_set<int> disallowedIDs = {
        2069, 3645, 3032, 2016, 1816, 3642, 3643, 2064
    };

    static const std::unordered_set<GameObjectType> disallowedTypes = {
        GameObjectType::YellowJumpPad,
        GameObjectType::PinkJumpPad,
        GameObjectType::RedJumpPad,
        GameObjectType::GravityPad,
        GameObjectType::SpiderPad,
        GameObjectType::SpiderOrb,
        GameObjectType::YellowJumpRing,
        GameObjectType::PinkJumpRing,
        GameObjectType::RedJumpRing,
        GameObjectType::GravityRing,
        GameObjectType::GreenRing,
        GameObjectType::DropRing,
        GameObjectType::DashRing,
        GameObjectType::GravityDashRing,
        GameObjectType::NormalGravityPortal,
        GameObjectType::InverseGravityPortal,
        GameObjectType::GravityTogglePortal,
        GameObjectType::CubePortal,
        GameObjectType::ShipPortal,
        GameObjectType::UfoPortal,
        GameObjectType::BallPortal,
        GameObjectType::WavePortal,
        GameObjectType::RobotPortal,
        GameObjectType::SpiderPortal,
        GameObjectType::SwingPortal,
        GameObjectType::NormalMirrorPortal,
        GameObjectType::InverseMirrorPortal,
        GameObjectType::MiniSizePortal,
        GameObjectType::RegularSizePortal,
        GameObjectType::DualPortal,
        GameObjectType::SoloPortal,
        GameObjectType::TeleportPortal,
        GameObjectType::SecretCoin,
        GameObjectType::UserCoin
    };

    if (allowedIDs.contains(id)) return true;
    if (disallowedIDs.contains(id)) return false;
    if (object->isTrigger() || object->isSpeedObject()) return false;
    if (disallowedTypes.contains(type)) return false;

    return true;
}

void POCEditorUI::updateButton(CCNode* btn) {
    auto defaultObject = PreviewObjectColors::get()->m_defaultObject;

    auto detailColorData = getActiveColor(defaultObject->m_detailColor->m_colorID);
    auto baseColorData = getActiveColor(defaultObject->m_baseColor->m_colorID);

    if (auto btnSprite = btn->getChildByType<ButtonSprite>(0)) {
        for (auto child : btnSprite->getChildrenExt()) {
            if (auto gameObject = typeinfo_cast<GameObject*>(child)) {
                if (!isColorable(gameObject)) return;
                
                if (auto baseColor = gameObject->m_baseColor) {
                    baseColor->m_colorID = defaultObject->m_baseColor->m_colorID;
                    baseColor->m_hsv = defaultObject->m_baseColor->m_hsv;

                    auto color = ccColor3B{255, 255, 255};
                    bool blending = false;
                    gameObject->updateHSVState();

                    if (defaultObject->m_baseColor->m_colorID != 0) {
                        color = baseColorData.color;
                        blending = baseColorData.blending;
                        baseColor->m_opacity = baseColorData.opacity;
                    }

                    auto blend = blending 
                        ? ccBlendFunc{GL_SRC_ALPHA, GL_ONE} 
                        : ccBlendFunc{GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

                    if (auto anim = typeinfo_cast<AnimatedGameObject*>(gameObject)) {
                        if (auto animSpr = anim->m_animatedSprite) {
                            if (auto paSpr = animSpr->m_paSprite) {
                                for (auto child : paSpr->getChildrenExt()) {
                                    if (child == anim->m_eyeSpritePart && !anim->m_childSprite) continue;
                                    auto spr = static_cast<CCSprite*>(child);
                                    spr->setBlendFunc(blend);
                                }
                            }
                        }
                    }
                    else if (typeinfo_cast<EnhancedGameObject*>(gameObject) || gameObject->m_hasCustomChild) {
                        for (auto child : gameObject->getChildrenExt()) {
                            if (child == gameObject->m_colorSprite) continue;
                            if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                spr->setBlendFunc(blend);
                            }
                        }
                    }

                    const auto& id = gameObject->m_objectID;
                    if (id == 1701 || id == 1702 || id == 1703) {
                        for (auto child : gameObject->getChildrenExt()) {
                            if (child->getChildrenCount() == 0) {
                                if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                    spr->setBlendFunc(blend);
                                }
                            }
                        }
                    }

                    gameObject->setBlendFunc(blend);

                    if (baseColor->m_usesHSV) color = GameToolbox::transformColor(color, baseColor->m_hsv);
                    gameObject->updateMainColor(color);
                }
                if (auto detailColor = gameObject->m_detailColor) {
                    detailColor->m_colorID = defaultObject->m_detailColor->m_colorID;
                    detailColor->m_hsv = defaultObject->m_detailColor->m_hsv;

                    auto color = ccColor3B{200, 200, 255};
                    bool blending = false;
                    gameObject->updateHSVState();

                    if (defaultObject->m_detailColor->m_colorID != 0) {
                        color = detailColorData.color;
                        blending = detailColorData.blending;
                        detailColor->m_opacity = detailColorData.opacity;
                    }

                    auto blend = blending 
                        ? ccBlendFunc{GL_SRC_ALPHA, GL_ONE} 
                        : ccBlendFunc{GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

                    std::function<void(CCNode*)> applyBlend = [&](CCNode* node) {
                        for (auto child : node->getChildrenExt()) {
                            if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                spr->setBlendFunc(blend);
                                applyBlend(spr);
                            }
                        }
                    };
                    
                    if (auto anim = typeinfo_cast<AnimatedGameObject*>(gameObject)) {
                        if (anim->m_childSprite) {
                            anim->m_childSprite->setBlendFunc(blend);
                        }
                        else {
                            if (auto eye = anim->m_eyeSpritePart) {
                                eye->setBlendFunc(blend);
                            }
                        }
                    }
                    else if (typeinfo_cast<EnhancedGameObject*>(gameObject)) {
                        for (auto child : gameObject->getChildrenExt()) {
                            applyBlend(child);
                        }
                    }
                    else {
                        const auto& id = gameObject->m_objectID;
                        if (id == 1701 || id == 1702 || id == 1703) {
                            for (auto child : gameObject->getChildrenExt()) {
                                if (child->getChildrenCount() > 0) {
                                    applyBlend(child);
                                    if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                        spr->setBlendFunc(blend);
                                    }
                                }
                            }
                        }
                        else {
                            applyBlend(gameObject);
                        }
                    }

                    if (auto spr = gameObject->m_colorSprite) {
                        spr->setBlendFunc(blend);
                    }

                    if (detailColor->m_usesHSV) color = GameToolbox::transformColor(color, detailColor->m_hsv);
                    gameObject->updateSecondaryColor(color);
                }
            }
        }
    }
}

void POCEditorUI::updateObjectColors(float dt) {
    for (auto child : getChildrenExt()) {
        if (auto bar = typeinfo_cast<EditButtonBar*>(child)) {
            if (!bar->isVisible()) continue;
            if (!bar->m_hasCreateItems) continue;

            if (ScrollableObjects::isEnabled()) {
                auto scrollEditButtonBar = static_cast<SOEditButtonBar*>(bar);
                for (auto item : scrollEditButtonBar->m_fields->m_visibleNodes) {
                    if (!nodeIsVisible(item)) continue;
                    updateButton(item);
                }
            }
            else {
                for (auto item : bar->m_buttonArray->asExt<CreateMenuItem>()) {
                    if (!item->getParentByType<EditButtonBar>() || !nodeIsVisible(item)) continue;
                    updateButton(item);
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
                    updateButton(btn);
                }
            }
        }
    }
}