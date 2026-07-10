#include "EditTools.hpp"
#include "utils/Utils.hpp"
#include "utils/Constants.hpp"

void EditTools::onEditor() {
    auto fields = static_cast<ETEditorUI*>(m_editorUI)->m_fields.self();

    fields->m_centerObjectButton = m_editorUI->getSpriteButton("center-object.png"_spr, menu_selector(ETEditorUI::onCenterObjects), nullptr, 0.9f);
    fields->m_centerObjectButton->setID("center-objects"_spr);
    m_editorUI->m_editButtonBar->m_buttonArray->addObject(fields->m_centerObjectButton);

    fields->m_centerCameraButton = m_editorUI->getSpriteButton("center-camera.png"_spr, menu_selector(ETEditorUI::onCenterCamera), nullptr, 0.9f);
    fields->m_centerCameraButton->setID("center-camera"_spr);
    m_editorUI->m_editButtonBar->m_buttonArray->addObject(fields->m_centerCameraButton);

    auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
    auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

    m_editorUI->m_editButtonBar->reloadItems(cols, rows);

    m_editorUI->addEventListener(KeybindSettingPressedEvent(Mod::get(), "EditTools-camera-to-object-keybind"), [this] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down & !repeat) {
            static_cast<ETEditorUI*>(m_editorUI)->onCenterCamera(nullptr);
        }
    });

    m_editorUI->addEventListener(KeybindSettingPressedEvent(Mod::get(), "EditTools-object-to-camera-keybind"), [this] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down) {
            static_cast<ETEditorUI*>(m_editorUI)->onCenterObjects(nullptr);
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
        CCPoint limitedPos = getLimitedPosition(object->getPosition() + deltaPos);
        object->setPosition(limitedPos);
        object->updateStartValues();

        m_editorLayer->reorderObjectSection(object);

        if (object->m_objectID == LinkedTeleportPortal) {
            TeleportPortalObject* teleportObject = static_cast<TeleportPortalObject*>(object);
            if (teleportObject->m_orangePortal) {
                m_editorLayer->reorderObjectSection(teleportObject->m_orangePortal);
            }
        }

        objectMoved(object);
        
        if (object->isSpeedObject() || object->canReverse()) {
            m_editorLayer->m_drawGridLayer->m_updateSpeedObjects = true;
        }
    }
}

void ETEditorUI::onCenterObjects(CCObject* sender) {
    auto arr = CCArray::create();
    if (m_selectedObjects) arr->addObjectsFromArray(m_selectedObjects);
    if (m_selectedObject) arr->addObject(m_selectedObject);

    if (arr->count() == 0) return;

    auto winSize = CCDirector::get()->getWinSize();

    auto center = getGroupCenter(arr, false);

    auto camCenter = m_editorLayer->m_objectLayer->convertToNodeSpace(winSize / 2.f + CCPoint{0.f, tinker::utils::getToolbarHeight() / 2.f});
    CCPoint snapped = getGridSnappedPos(camCenter);

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

void ETEditorUI::onCenterCamera(CCObject* sender) {
    auto arr = CCArray::create();
    if (m_selectedObjects) arr->addObjectsFromArray(m_selectedObjects);
    if (m_selectedObject) arr->addObject(m_selectedObject);

    if (arr->count() == 0) return;

    auto winSize = CCDirector::get()->getWinSize();

    auto center = getGroupCenter(arr, false);
    m_editorLayer->m_objectLayer->setPosition(-(center * m_editorLayer->m_objectLayer->getScale()) + winSize / 2.f + CCPoint{0.f, tinker::utils::getToolbarHeight() / 2.f});

    auto pos = m_editorLayer->m_objectLayer->convertToWorldSpace(center);

    playCircleAnim(pos, 50.f, 0.8f);

    updateSlider();
}