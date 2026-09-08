#include "modules/PlacePreview.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include "InputsHandler.hpp"
#include "modules/CanvasRotate.hpp"
#include "modules/GridControl.hpp"
#include "modules/PreviewObjectColors.hpp"
#include "utils/Utils.hpp"

bool PlacePreview::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        m_hover->removeFromParent();
        m_hover = nullptr;

        m_feedbackContainer->removeFromParent();
        m_feedbackContainer = nullptr;
    }
    return true;
}

bool PlacePreview::onSettingChanged(std::string_view key, const matjson::Value& value) {
    if (key == "opacity") {
        auto opacityRes = value.asDouble();
        if (opacityRes) {
            auto opacity = opacityRes.unwrap();
            if (m_hover) {
                m_hover->updateOpacity(opacity);
            }
        }
    }

    return true;
}

void PlacePreview::onEditor() {
    auto editor = getEditor();

    m_hover = tinker::ui::PlacePreviewNode::create();
    editor->addChild(m_hover);

    m_feedbackContainer = CCNode::create();
    m_feedbackContainer->setID("place-preview-container"_spr);
    m_feedbackContainer->setZOrder(9999);
    editor->m_editorLayer->m_objectLayer->addChild(m_feedbackContainer);

    if (!m_callbacksSet) {
        alpha::editor_tabs::addModeSwitchCallback([this] (ZStringView ID) {
            if (!m_feedbackContainer) return;
            m_feedbackContainer->setVisible(inValidTab());
        });

        alpha::editor_tabs::addTabSwitchCallback([this] (ZStringView ID) {
            if (!m_feedbackContainer) return;
            m_feedbackContainer->setVisible(inValidTab());
        });
        m_callbacksSet = true;
    }
}

bool PlacePreview::inValidTab() {
    auto mode = alpha::editor_tabs::getCurrentMode().unwrapOrDefault();
    if (mode != alpha::editor_tabs::BUILD) return false;

    auto tab = alpha::editor_tabs::getCurrentTab().unwrapOrDefault();
    if (tab == "alk.allium/allium") return false;
    if (tab == "michael.sculptorv4/sculptor") return false;

    return true;
}

namespace tinker::ui {

PlacePreviewNode* PlacePreviewNode::create() {
    auto ret = new PlacePreviewNode();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool PlacePreviewNode::init() {
    setAnchorPoint({0.f, 0.f});
    setID("place-preview-node"_spr);
    setZOrder(500);
    setContentSize(CCDirector::get()->getWinSize());
    scheduleUpdate();

    return true;
}

bool PlacePreviewNode::mouseEntered(TouchEvent* touch) {
    return true;
}

void PlacePreviewNode::applyOffset(GameObject* object) {
    auto editor = EditorUI::get();
    auto offset = editor->offsetForKey(object->m_objectID);

    auto relativeOffset = GameToolbox::getRelativeOffset(object, offset);
    object->setPosition(object->getPosition() + relativeOffset);
}

void PlacePreviewNode::removeOffset(GameObject* object) {
    auto editor = EditorUI::get();
    auto offset = editor->offsetForKey(object->m_objectID);

    auto relativeOffset = GameToolbox::getRelativeOffset(object, offset);
    object->setPosition(object->getPosition() - relativeOffset);
}

void PlacePreviewNode::update(float dt) {
    if (!m_hoverObject) return;
    auto editor = EditorUI::get();

    auto editorPause = editor->m_editorLayer->getChildByType<EditorPauseLayer>(0);
    bool hasPopup = editorPause || InputEditorUI::get()->hasActiveAlerts();

    if (!hasPopup) {
        m_objectRender->setVisible(m_objectVisible);
        m_objectRender->setPosition(m_objectPos);
    }

    if (editor->m_selectedObject && m_hoverObject->m_objectID == editor->m_selectedObject->m_objectID) {
        removeOffset(m_hoverObject);
        m_hoverObject->duplicateValues(editor->m_selectedObject);
        applyOffset(m_hoverObject);

        m_wasDuplicated = true;
    }
    else if (m_wasDuplicated) {
        setObject(m_hoverObject->m_objectID);
        m_wasDuplicated = false;
    }
    updateObjectColor();

    if (!m_wasDuplicated) {
        if (CanvasRotate::isEnabled()) {
            int cameraRot = static_cast<int>(std::round(editor->m_editorLayer->m_gameState.m_cameraAngle));
            float rotationValue = 0.f;

            if (!CanvasRotate::get()->m_rotationNode->isAlignKeyDown()) {
                if (cameraRot < 45 || cameraRot >= 315) {
                    rotationValue = 0.f;
                }
                else if (cameraRot < 135) {
                    rotationValue = 270.f;
                }
                else if (cameraRot < 225) {
                    rotationValue = 180.f;
                }
                else {
                    rotationValue = 90.f;
                }
            }
            else {
                rotationValue = -editor->m_editorLayer->m_gameState.m_cameraAngle;
            }
            
            removeOffset(m_hoverObject);
            m_hoverObject->setRotation(rotationValue);
            applyOffset(m_hoverObject);
        }
        if (GridControl::isEnabled()) {
            auto mult = GridControl::get()->getGridMultiplier();
            m_hoverObject->updateCustomScaleX(m_hoverObject->m_pixelScaleX * mult);
            m_hoverObject->updateCustomScaleY(m_hoverObject->m_pixelScaleY * mult);
        }
    }
}

void PlacePreviewNode::showAtPos(const CCPoint& pos) {
    auto editor = EditorUI::get();

    auto basePos = editor->getGridSnappedPos(pos);
    auto offset = editor->offsetForKey(m_hoverObject->m_objectID);
    auto objectPos = basePos + offset;

    auto checkPos = objectPos;
    bool isFlipX = false;
    bool isFlipY = false;
    float rot = 0.0f;

    if (editor->m_selectedObject && editor->m_selectedObject->m_objectID == m_hoverObject->m_objectID) {
        isFlipX = m_hoverObject->isFlipX();
        isFlipY = m_hoverObject->isFlipY();
        rot = m_hoverObject->getRotation();
        checkPos = basePos + editor->getRelativeOffset(editor->m_selectedObject);
    }

    bool exists = editor->m_editorLayer->typeExistsAtPosition(m_hoverObject->m_objectID, checkPos, isFlipX, isFlipY, rot);
    bool canBeShown = !exists && pos.y > utils::getToolbarHeight();

    m_objectVisible = canBeShown;
    m_objectPos = objectPos;
}

void PlacePreviewNode::mouseMoved(TouchEvent* touch) {
    if (!m_objectRender || !m_hoverObject) return;

    auto editor = EditorUI::get();
    auto pos = touch->getLocation();

    auto winSize = CCDirector::get()->getWinSize();
    auto mousePos = tinker::utils::rotatePointAroundPivot(pos, winSize / 2.f, editor->m_editorLayer->m_gameState.m_cameraAngle);

    auto objectSpace = editor->m_editorLayer->m_objectLayer->convertToNodeSpace(mousePos);

    showAtPos(objectSpace);
}

void PlacePreviewNode::updateObjectColor() {
    auto editor = EditorUI::get();

    if (PreviewObjectColors::isEnabled()) {
        if (m_hoverObject->m_baseColor) {
            auto base = PreviewObjectColors::get()->getBaseColor();
            auto id = base->m_colorID;
            if (id == 0) {
                m_hoverObject->m_baseColor->m_colorID = m_hoverObject->m_baseColor->m_defaultColorID;
            }
            else {
                m_hoverObject->m_baseColor->m_colorID = id;
            }
            m_hoverObject->m_baseColor->m_hsv = base->m_hsv;
        }
        if (m_hoverObject->m_detailColor) {
            auto detail = PreviewObjectColors::get()->getDetailColor();
            auto id = detail->m_colorID;
            if (id == 0) {
                m_hoverObject->m_detailColor->m_colorID = m_hoverObject->m_detailColor->m_defaultColorID;
            }
            else {
                m_hoverObject->m_detailColor->m_colorID = id;
            }
            m_hoverObject->m_detailColor->m_hsv = detail->m_hsv;
        }
    }
    else {
        if (m_hoverObject->m_baseColor) {
            if (m_hoverObject->m_baseColor->m_colorID == 0) {
                m_hoverObject->m_baseColor->m_colorID = m_hoverObject->m_baseColor->m_defaultColorID;
            }
        }
        if (m_hoverObject->m_detailColor) {
            if (m_hoverObject->m_detailColor->m_colorID == 0) {
                m_hoverObject->m_detailColor->m_colorID = m_hoverObject->m_detailColor->m_defaultColorID;
            }
        }
    }

    tinker::utils::updateGameObjectColor(editor->m_editorLayer, m_hoverObject);
}

void PlacePreviewNode::updateOpacity(float opacity) {
    if (m_objectRender) {
        m_objectRender->setOpacity(255 * opacity);
    }
}

void PlacePreviewNode::setObject(int id) {
    if (m_hoverObject) {
        m_hoverObject->removeFromParent();
        m_hoverObject = nullptr;
    }
    if (m_objectRender) {
        m_objectRender->removeFromParent();
        m_objectRender = nullptr;
    }
    if (id == 0) return;

    auto editor = EditorUI::get();

    auto spr = editor->spriteFromObjectString(fmt::format("1,{}", id), true, false, 1, nullptr, nullptr, nullptr);
    if (!spr) return;

    auto object = spr->getChildByType<GameObject>(0);
    if (!object) return;

    auto pos = getMousePos();
    auto winSize = CCDirector::get()->getWinSize();
    auto mousePos = tinker::utils::rotatePointAroundPivot(pos, winSize / 2.f, editor->m_editorLayer->m_gameState.m_cameraAngle);

    auto objectSpace = editor->m_editorLayer->m_objectLayer->convertToNodeSpace(mousePos);

    if (id == 914) {
        static_cast<TextGameObject*>(object)->updateTextObject("A", false);
    } 

    m_hoverObject = object;

    m_objectRender = RenderNode::create(m_hoverObject, false);
    m_hoverObject->setPosition(m_objectRender->getContentSize() / 2.f);

    m_objectRender->setOpacity(255 * PlacePreview::getSetting<float, "opacity">());

    showAtPos(objectSpace);

    PlacePreview::get()->m_feedbackContainer->addChild(m_objectRender);
}

void PlacePreviewNode::onEnter() {
    CCNode::onEnter();
    CCTouchDispatcher::get()->addTargetedDelegate(this, -531, false);
}

void PlacePreviewNode::onExit() {
    CCNode::onExit();
    CCTouchDispatcher::get()->removeDelegate(this);
}

}

void PPEditorUI::onCreateButton(cocos2d::CCObject* sender) {
    EditorUI::onCreateButton(sender);
    PlacePreview::get()->m_hover->setObject(m_selectedObjectIndex);
}