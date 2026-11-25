#include "modules/EditTools.hpp"
#include <alphalaneous.editortab_api/API.hpp>
#include "utils/Constants.hpp"
#include "utils/Objects.hpp"

using namespace geode::prelude;

void EditTools::onEditor() {
    auto mode = alpha::editor_tabs::getMode(alpha::editor_tabs::Edit);
    if (!mode) return;

    auto tab = mode->getTab("edit");
    if (!tab) return;

    auto node = tab->getNode();
    if (!node) return;

    auto editor = static_cast<ETEditorUI*>(getEditor());

    auto fields = editor->m_fields.self();

    fields->m_centerObjectButton = alpha::editor_tabs::createEditButton(CCSprite::create("center-object.png"_spr), 1, [this, editor] (auto sender) {
        editor->onCenterObjects();
    });
    fields->m_centerObjectButton->setID("center-objects"_spr);
    node->addItem(fields->m_centerObjectButton);

    fields->m_centerCameraButton = alpha::editor_tabs::createEditButton(CCSprite::create("center-camera.png"_spr), 1, [this, editor] (auto sender) {
        editor->onCenterCamera();
    });
    fields->m_centerCameraButton->setID("center-camera"_spr);
    node->addItem(fields->m_centerCameraButton);

    fields->m_lockFlipPosButton = alpha::editor_tabs::createEditButton(CCSprite::create("flip-position-lock.png"_spr), 1, [this, editor] (auto sender) {
        editor->onLockFlipPos();
    });
    fields->m_lockFlipPosButton->setID("lock-flip-pos"_spr);
    node->addItem(fields->m_lockFlipPosButton);

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "EditTools-camera-to-object-keybind"), [this, editor] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down & !repeat) {
            static_cast<ETEditorUI*>(editor)->onCenterCamera();
        }
    });

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "EditTools-object-to-camera-keybind"), [this, editor] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down) {
            static_cast<ETEditorUI*>(editor)->onCenterObjects();
        }
    });
}

void ETEditorUI::updateButtons() {
    EditorUI::updateButtons();
    auto fields = m_fields.self();
    if (!fields->m_centerObjectButton || !fields->m_centerCameraButton) return;

    if (m_selectedObject || (m_selectedObjects && m_selectedObjects->count() > 0)) {
        setButtonColor(fields->m_centerObjectButton, {255, 255, 255});
        setButtonColor(fields->m_centerCameraButton, {255, 255, 255});

        fields->m_centerObjectButton->setEnabled(true);
        fields->m_centerCameraButton->setEnabled(true);
    }
    else {
        setButtonColor(fields->m_centerObjectButton, {166, 166, 166});
        setButtonColor(fields->m_centerCameraButton, {166, 166, 166});

        fields->m_centerObjectButton->setEnabled(false);
        fields->m_centerCameraButton->setEnabled(false);
    }

    auto bg = fields->m_flipPosLocked ? "GJ_button_02.png" : "GJ_button_01.png";

    auto btnSpr = fields->m_lockFlipPosButton->getChildByType<ButtonSprite*>(0);
    btnSpr->updateBGImage(bg);
}

void ETEditorUI::setButtonColor(CCMenuItemSpriteExtra* btn, const ccColor3B& color) {
    auto btnSpr = btn->getChildByType<ButtonSprite*>(0);
    for (auto child : btnSpr->getChildrenExt()) {
        if (auto rgba = typeinfo_cast<CCNodeRGBA*>(child)) {
            rgba->setColor(color);
        }
    }
}

void ETEditorUI::objectMoved(GameObject* object) {
    if (!object) return;

    using namespace tinker::constants::objects;

    constexpr std::array<int, 51> effectObjects = {
        NoEnterEffect,
        FadeBottomEnterEffect,
        FadeTopEnterEffect,
        FadeLeftEnterEffect,
        FadeRightEnterEffect,
        SmallToBigEnterEffect,
        BigToSmallEnterEffect,
        StartPosition,
        EnableGhostTrail,
        DisableGhostTrail,
        ChaoticEnterEffect,
        HalveLeftEnterEffect,
        HalveRightEnterEffect,
        HalveEnterEffect,
        HalveInverseEnterEffect,
        NoEnterExitEffect,
        ScaleTrigger,
        GradientTrigger,
        ShaderTrigger,
        ShockWaveShaderTrigger,
        ShockLineShaderTrigger,
        GlitchShaderTrigger,
        ChromaticShaderTrigger,
        ChromaticGlitchShaderTrigger,
        PixelateShaderTrigger,
        LensCircleShaderTrigger,
        RadialBlurShaderTrigger,
        MotionBlurShaderTrigger,
        BulgeShaderTrigger,
        PinchShaderTrigger,
        GrayscaleShaderTrigger,
        SepiaShaderTrigger,
        InvertColorShaderTrigger,
        HueShaderTrigger,
        EditColorShaderTrigger,
        SplitScreenShaderTrigger,
        AreaMoveTrigger,
        AreaRotateTrigger,
        AreaScaleTrigger,
        AreaFadeTrigger,
        AreaTintTrigger,
        AdvancedFollowTrigger,
        EnterMoveTrigger,
        EnterRotateTrigger,
        EnterScaleTrigger,
        EnterFadeTrigger,
        EnterTintTrigger,
        TeleportTrigger,
        EnterStopTrigger,
        AreaStopTrigger,
        EditAdvancedFollowTrigger
    };

    constexpr std::array<int, 6> colorObjects = {
        BackgroundColorTrigger,
        Ground1ColorTrigger,
        ObjColorTrigger,
        ColorTrigger,
        Ground2ColorTrigger, 
        LineColorTrigger
    };

    if (std::find(effectObjects.begin(), effectObjects.end(), object->m_objectID) != effectObjects.end()) {
        m_editorLayer->m_drawGridLayer->m_sortEffects = true;
    }
    else if (std::find(colorObjects.begin(), colorObjects.end(), object->m_objectID) != colorObjects.end()) {
        m_editorLayer->m_colorTriggersChanged = true;
    }
    else if (object->m_objectID == AlphaTrigger) {
        m_editorLayer->m_alphaTriggersChanged = true;
    }
    else if (object->m_objectID == PulseTrigger) {
        m_editorLayer->m_pulseTriggersChanged = true;
    }
    else if (object->m_objectID == SpawnTrigger || object->m_objectID == AdvancedRandomTrigger) {
        m_editorLayer->m_spawnTriggersChanged = true;
    }

    if (object->m_isSpawnOrderTrigger) {
        m_editorLayer->m_spawnOrderObjectsChanged = true;
    }
    if (object->m_dontIgnoreDuration) {
        static_cast<EffectGameObject*>(object)->m_endPosition = CCPoint{0.f, 0.f};
    }
}

void ETEditorUI::moveObjects(CCArray* objects, cocos2d::CCPoint deltaPos) {
    if (objects->count() == 0) return;

    using namespace tinker::constants::objects;

    for (auto object : objects->asExt<GameObject>()) {
        auto limitedPos = getLimitedPosition(object->getPosition() + deltaPos);
        object->setPosition(limitedPos);
        object->updateStartValues();

        m_editorLayer->reorderObjectSection(object);

        if (object->m_objectID == LinkedTeleportPortal) {
            auto teleportObject = static_cast<TeleportPortalObject*>(object);
            if (teleportObject->m_orangePortal) {
                m_editorLayer->reorderObjectSection(teleportObject->m_orangePortal);
            }
        }

        objectMoved(object);
        
        if (object->isSpeedObject() || object->canReverse()) {
            m_editorLayer->m_drawGridLayer->m_updateSpeedObjects = true;
        }
    }

    ObjectMovedEvent().send();
}

void ETEditorUI::onCenterObjects() {
    auto arr = CCArray::create();
    if (m_selectedObjects) arr->addObjectsFromArray(m_selectedObjects);
    if (m_selectedObject) arr->addObject(m_selectedObject);

    if (arr->count() == 0) return;

    auto center = getGroupCenter(arr, false);
    auto camCenter = tinker::utils::objects::getCameraCenter();

    auto snapped = getGridSnappedPos(camCenter);
    auto offset = snapped - center;

    m_editorLayer->addToUndoList(UndoObject::createWithTransformObjects(arr, UndoCommand::Transform), false);

    moveObjects(arr, offset);

    auto selectArr = m_selectedObjects->shallowCopy();
    if (m_selectedObject) selectArr->addObject(m_selectedObject);

    m_pivotPoint = getGroupCenter(selectArr, false);

    m_transformControl->setPosition(snapped);
    m_scaleControl->setPosition(snapped + CCPoint{0.f, 40.f});
    m_rotationControl->setPosition(snapped);
}

void ETEditorUI::onCenterCamera() {
    if (m_selectedObject) {
        tinker::utils::objects::moveCameraToObject(m_selectedObject);
    }
    else if (m_selectedObjects->count() > 0) {
        tinker::utils::objects::moveCameraToObjects(m_selectedObjects);
    }
}

void ETEditorUI::transformObjectCall(EditCommand command) {
    auto fields = m_fields.self();
    if (!fields->m_flipPosLocked) {
        EditorUI::transformObjectCall(command);
        return;
    }
    if (command == EditCommand::FlipX || command == EditCommand::FlipY) {
        fields->m_isFlipping = true;
    }
    EditorUI::transformObjectCall(command);
    fields->m_isFlipping = false;
}

void ETEditorUI::moveObject(GameObject* object, cocos2d::CCPoint offset) {
    auto fields = m_fields.self();
    if (fields->m_flipPosLocked && fields->m_isFlipping) return;

    EditorUI::moveObject(object, offset);
}

void ETEditorUI::onLockFlipPos() {
    auto fields = m_fields.self();
    fields->m_flipPosLocked = !fields->m_flipPosLocked;
    
    updateButtons();
}