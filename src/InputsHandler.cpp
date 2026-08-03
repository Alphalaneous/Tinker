#include "InputsHandler.hpp"
#include "Events.hpp"
#include "utils/Utils.hpp"
#include "modules/CanvasRotate/CanvasRotate.hpp"
#include "modules/ScrollableObjects.hpp"
#include "actions/CCCallbackAction.hpp"
#include "actions/CCValueTo.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <geode.devtools/include/API.hpp>

using namespace alpha::prelude;

namespace tinker::ui {

TouchForward* TouchForward::create(EditorUI* editorUI) {
    auto ret = new TouchForward();
    if (ret->init(editorUI)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

TouchForward* TouchForward::get() {
    return InputEditorUI::get()->m_fields->m_forward;
}

bool TouchForward::init(EditorUI* editorUI) {
    m_editorUI = editorUI;
    setZOrder(100);
    setTouchEnabled(true);

    editorUI->addOnEnterCallback([this] {
        CCTouchDispatcher::get()->removeDelegate(m_editorUI);
    });

    registerTouch<CanvasRotate>(2);
    registerTouch<InputEditorUI>(1);

    return true;
}

void TouchForward::registerWithTouchDispatcher() {
    CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
}

bool TouchForward::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    m_touches.insert(touch);

    return dispatch(0, touch,
        [] (auto& h) -> auto& { return h.beganCallback; },
        [this] (CCTouch* t) {
            return m_editorUI->ccTouchBegan(t, nullptr);
        }
    );
}

void TouchForward::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    dispatch(0, touch,
        [] (auto& h) -> auto& { return h.movedCallback; },
        [this] (CCTouch* t) {
            return m_editorUI->ccTouchMoved(t, nullptr);
        }
    );
}

void TouchForward::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    m_touches.erase(touch);

    dispatch(0, touch,
        [] (auto& h) -> auto& { return h.endedCallback; },
        [this] (CCTouch* t) {
            return m_editorUI->ccTouchEnded(t, nullptr);
        }
    );
}

void TouchForward::ccTouchCancelled(CCTouch* touch, CCEvent* event) {
    if (!m_skipErase) {
        m_touches.erase(touch);
    }

    dispatch(0, touch,
        [] (auto& h) -> auto& { return h.cancelledCallback; },
        [this] (CCTouch* t) {
            return m_editorUI->ccTouchCancelled(t, nullptr);
        }
    );
}

void TouchForward::cancelAllTouches() {
    for (const auto& touch : m_touches) {
        if (!touch) continue;
        m_skipErase = true;
        ccTouchCancelled(touch, nullptr);
        m_skipErase = false;
    }
    m_touches.clear();
}

}

#ifdef GEODE_IS_MOBILE
void InputAppDelegate::applicationDidEnterBackground() {
    auto editor = InputEditorUI::get();
    if (!editor) {
        AppDelegate::applicationDidEnterBackground();
        return;
    }

    tinker::ui::TouchForward::get()->cancelAllTouches();
    AppDelegate::applicationDidEnterBackground();
}
#endif

InputEditorUI* InputEditorUI::get() {
    return static_cast<InputEditorUI*>(EditorUI::get());
}

void InputEditorUI::disableBetterEditHook() {
    auto betterEdit = tinker::utils::getMod<"hjfod.betteredit">();
    if (!betterEdit) return;

    for (auto hook : betterEdit->getHooks()) {
        if (hook->getDisplayName() == "EditorUI::scrollWheel") {
            (void) hook->disable();
            break;
        }
    }
}

bool InputEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) return false;
    disableBetterEditHook();
    
    auto fields = m_fields.self();

    schedule(schedule_selector(InputEditorUI::checkScrolling));

    addEventListener(ScrollWheelEvent(), [this, fields](double x, double y) {
        fields->m_scroll = CCPoint{static_cast<float>(x), static_cast<float>(y)};
        if (!tinker::utils::getSetting<bool, "scroll-delegate-to-vanilla">()) {
            onScroll();
        }
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "ScrollableObjects-speed-modifier-key"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        fields->m_tabModifierHeld = down;
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-edit-extras"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        if (auto popup = CCScene::get()->getChildByType<SetupObjectOptionsPopup>(0)) {
            popup->keyBackClicked();
        }
        else {
            if (m_selectedObject || m_selectedObjects->count() > 0) {
                auto idLayer = SetGroupIDLayer::create(m_selectedObject, m_selectedObjects);
                auto popup = SetupObjectOptionsPopup::create(m_selectedObject, m_selectedObjects, idLayer);
                popup->show();
            }
        }
    });

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-edit-object"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        if (auto popup = CCScene::get()->getChildByType<CustomizeObjectLayer>(0)) {
            popup->keyBackClicked();
        }
        else {
            editObject(nullptr);
        }
    });

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-edit-group"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        if (auto popup = CCScene::get()->getChildByType<SetGroupIDLayer>(0)) {
            popup->keyBackClicked();
        }
        else {
            editGroup(nullptr);
        }
    });

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-edit-special"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        if (auto popup = CCScene::get()->getChildByType<SetupSmartBlockLayer>(0)) {
            popup->keyBackClicked();
        }
        else if (auto popup = CCScene::get()->getChildByType<SetItemIDLayer>(0)) {
            popup->keyBackClicked();
        }
        else if (auto popup = CCScene::get()->getChildByType<EditGameObjectPopup>(0)) {
            popup->keyBackClicked();
        }
        else if (auto popup = CCScene::get()->getChildByType<SetupInteractObjectPopup>(0)) {
            popup->keyBackClicked();
        }
        else if (auto popup = CCScene::get()->getChildByType<SetupRotatePopup>(0)) {
            popup->keyBackClicked();
        }
        else if (auto popup = CCScene::get()->getChildByType<SetupGradientPopup>(0)) {
            popup->keyBackClicked();
        }
        else if (auto popup = CCScene::get()->getChildByType<SetupAnimSettingsPopup>(0)) {
            popup->keyBackClicked();
        }
        else if (auto popup = CCScene::get()->getChildByType<CreateParticlePopup>(0)) {
            popup->keyBackClicked();
        }
        else {
            editObjectSpecial(0);
        }
    });

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-copy-values"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        onCopyState(nullptr);
    });

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-paste-state"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        onPasteState(nullptr);
    });

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-paste-color"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        onPasteColor(nullptr);
    });
    
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-show-scale"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        activateScaleControl(getEditButtonByTag(29));
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-show-scale-xy"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        activateScaleControl(getEditButtonByTag(30));
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-show-warp"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        activateTransformControl(getEditButtonByTag(31));
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-move-obj-half-left"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        moveObjectCall(EditCommand::HalfLeft);
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-move-obj-half-right"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        moveObjectCall(EditCommand::HalfRight);
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-move-obj-half-up"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        moveObjectCall(EditCommand::HalfUp);
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-move-obj-half-down"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        moveObjectCall(EditCommand::HalfDown);
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-move-obj-big-left"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        moveObjectCall(EditCommand::BigLeft);
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-move-obj-big-right"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        moveObjectCall(EditCommand::BigRight);
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-move-obj-big-up"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        moveObjectCall(EditCommand::BigUp);
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-move-obj-big-down"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        moveObjectCall(EditCommand::BigDown);
    });

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-restart"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
            auto dummy = CCNode::create();
            onStopPlaytest(dummy);
            onPlaytest(dummy);
        }
    });

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-restart-from-beginning"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
            auto dummy = CCNode::create();
            dummy->setTag(1);
            onStopPlaytest(dummy);
            onPlaytest(dummy);
        }
    });

    addEventListener(KeyboardInputEvent(enumKeyCodes::KEY_LeftShift), [this] (KeyboardInputData& data) {
        if (!m_swipeEnabled && data.action == KeyboardInputData::Action::Release && tinker::utils::getSetting<bool, "stop-swipe-on-shift-release">()) {
            m_swipeActive = false;
        }
    });
    
    addEventListener(KeyboardInputEvent(enumKeyCodes::KEY_RightShift), [this] (KeyboardInputData& data) {
        if (!m_swipeEnabled && data.action == KeyboardInputData::Action::Release && tinker::utils::getSetting<bool, "stop-swipe-on-shift-release">()) {
            m_swipeActive = false;
        }
    });

    if (!std::isfinite(editorLayer->m_objectLayer->getPosition().x) || !std::isfinite(editorLayer->m_objectLayer->getPosition().y)) {
        editorLayer->m_objectLayer->setPosition({0, 90});
    }

    fields->m_forward = tinker::ui::TouchForward::create(this);
    editorLayer->addChild(fields->m_forward);

    return true;
}

void InputEditorUI::blockPinch(bool block) {
    auto fields = m_fields.self();
    if (block) {
        fields->m_touch1 = nullptr;
        fields->m_touch2 = nullptr;
        fields->m_isPinching = false;
    }
    fields->m_blockPinching = block;
}

CCMenuItemSpriteExtra* InputEditorUI::getEditButtonByTag(int tag) {
    return static_cast<CCMenuItemSpriteExtra*>(m_editButtonDict->objectForKey(numToString(tag)));
}

void InputEditorUI::addTextInput(TextInput* input) {
    if (!input) return;

    input->addOnEnterCallback([input] {
        InputEditorUI::get()->m_fields->m_textInputs.insert(input);
    });
    input->addOnExitCallback([input] {
        InputEditorUI::get()->m_fields->m_textInputs.erase(input);
    });
}

#ifdef GEODE_IS_MACOS
#include <CoreFoundation/CoreFoundation.h>
#include "utils/MacUtils.hpp"

bool InputEditorUI::isNaturalScrollEnabled() {
    if (!tinker::utils::getSetting<bool, "ignore-natural-scrolling">()) return false;
    CFPropertyListRef value =
        CFPreferencesCopyAppValue(
            CFSTR("com.apple.swipescrolldirection"),
            kCFPreferencesAnyApplication
        );

    if (value && CFGetTypeID(value) == CFBooleanGetTypeID()) {
        bool result = CFBooleanGetValue((CFBooleanRef)value);
        CFRelease(value);
        return result;
    }

    if (value) CFRelease(value);
    return true;
}
#else
bool InputEditorUI::isNaturalScrollEnabled() {
    return false;
}
#endif

CCPoint InputEditorUI::getRealMousePos() {
    #ifdef GEODE_IS_WINDOWS
    auto director = CCDirector::get();
    auto gl = CCEGLView::get();

    auto winSize = director->getWinSize();
    auto frameSize = gl->getFrameSize();
    auto mouse = gl->getMousePosition() / frameSize;

    return CCPoint{mouse.x, 1.f - mouse.y} * winSize;
    #elif defined(GEODE_IS_MACOS)
    return tinker::utils::getRealMousePos();
    #else
    return CCPoint{0, 0};
    #endif
}

void InputEditorUI::onScroll() {
    auto quickVolume = CCScene::get()->getChildByID("hjfod.quick-volume-controls/overlay");
    if (quickVolume) {
        auto scale9 = quickVolume->getChildByType<CCScale9Sprite>(0);
        if (scale9->getOpacity() != 0) return;
    }
    auto editorPause = m_editorLayer->getChildByType<EditorPauseLayer>(0);
    if (editorPause) return;

    using namespace tinker::utils;
    auto fields = m_fields.self();
    float x = fields->m_scroll.x;
    float y = fields->m_scroll.y;

    #ifdef GEODE_IS_MACOS
    int naturalMult = isNaturalScrollEnabled() ? 1 : -1;
    float xMult = 1 * naturalMult;
    float yMult = 1 * naturalMult;
    #else
    float xMult = 1;
    float yMult = -1;
    #endif

    for (auto alert : fields->m_activeAlerts) {
        if (alert && alert->getParentByType<CCScene>() && nodeIsVisible(alert)) {
            return;
        }
    }

    if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) return;

    auto mousePos = getMousePos();

    if (mousePos == getRealMousePos() && devtools::isOpen()) return;
    
    if (mousePos.y < getToolbarHeight()) {
        auto currentTabIDRes = alpha::editor_tabs::getCurrentTab();
        if (currentTabIDRes) {
            auto currentTabID = currentTabIDRes.unwrap();
            auto currentTabRes = alpha::editor_tabs::nodeForTab(currentTabID);
            if (currentTabRes) {
                auto currentTab = currentTabRes.unwrap();
                if (currentTab->getUserFlag("disable-editor-scroll"_spr)) {
                    return;
                }
            }
        }
    }

    if (ScrollableObjects::isEnabled() && !ScrollableObjects::get()->canScroll()) {
        for (auto child : getChildrenExt()) {
            if (!nodeIsVisible(child)) continue;

            bool invertScroll = ScrollableObjects::getSetting<bool, "invert-scroll">();

            if (auto bar = static_cast<SOEditButtonBar*>(typeinfo_cast<EditButtonBar*>(child))) {
                auto barFields = bar->m_fields.self();
                float multiplier = fields->m_tabModifierHeld ? 12 * getSetting<float, "ScrollableObjects-speed-modifier">() : 12;
                barFields->m_scrollBar->scroll((x * multiplier) * xMult * (invertScroll ? -1 : 1), (y * multiplier) * yMult * (invertScroll ? -1 : 1));
            }
        }
        return;
    }

    if (CCKeyboardDispatcher::get()->getShiftKeyPressed()) {
        std::swap(x, y);
    }

    auto layer = m_editorLayer->m_objectLayer;

    if (CCKeyboardDispatcher::get()->getControlKeyPressed()) {
        if (!fields->m_activeZoom) {
            fields->m_targetScale = layer->getScale();
            fields->m_startSwipe = layer->convertToNodeSpace(m_swipeStart);
            fields->m_activeZoom = true;
        }
        fields->m_targetScale = std::max(fields->m_targetScale, 0.1f);

        auto winSize = CCDirector::get()->getWinSize();

        float zoomFactor = 1.05f;
        float zoomSpeed = 0.2f;

        #ifdef GEODE_IS_MACOS
        float newY = (y * getSetting<float, "zoom-multiplier">()) * -yMult;
        #else
        float newY = (y * getSetting<float, "zoom-multiplier">()) * yMult;
        #endif

        newY *= (getSetting<bool, "invert-zoom-scroll">() ? -1 : 1);

        auto oldScale = fields->m_targetScale;

        float newScale = fields->m_targetScale * std::powf(zoomFactor, -newY * zoomSpeed);
        fields->m_targetScale = std::min(std::max(newScale, getSetting<float, "zoom-minimum">()), getSetting<float, "zoom-maximum">());

        if (getSetting<bool, "smooth-scroll-enabled">()) {
            if (oldScale != fields->m_targetScale) {
                if (fields->m_scale) layer->stopAction(fields->m_scale);

                fields->m_scale = CCEaseOut::create(CCValueTo<float>::create(0.1f * fields->m_speedScale, layer->getScale(), fields->m_targetScale, [this, layer, fields, winSize] (float t, float start, float end, float& scale) {
                    scale = start + (end - start) * t;

                    if (getSetting<bool, "zoom-to-cursor">()) {
                        auto mousePos = tinker::utils::rotatePointAroundPivot(getMousePos(), winSize / 2, m_editorLayer->m_gameState.m_cameraAngle);
                        auto prevPos = layer->convertToNodeSpace(mousePos);
                    
                        updateZoom(scale);

                        auto newPos = layer->convertToWorldSpace(prevPos);
                        layer->setPosition(layer->getPosition() + mousePos - newPos);
                    }
                    else {
                        updateZoom(scale);
                    }
                    m_swipeStart = layer->convertToWorldSpace(fields->m_startSwipe);
                    constrainGameLayerPosition();
                    updateSlider();
                }), 1.2f);

                layer->runAction(fields->m_scale);
            }
        }
        else {
            fields->m_activeZoom = false;

            if (getSetting<bool, "zoom-to-cursor">()) {
                auto mousePos = tinker::utils::rotatePointAroundPivot(getMousePos(), winSize / 2, m_editorLayer->m_gameState.m_cameraAngle);
                auto prevPos = layer->convertToNodeSpace(mousePos);
            
                updateZoom(fields->m_targetScale);

                auto newPos = layer->convertToWorldSpace(prevPos);
                layer->setPosition(layer->getPosition() + mousePos - newPos);
            }
            else {
                updateZoom(fields->m_targetScale);
            }
            m_swipeStart = layer->convertToWorldSpace(fields->m_startSwipe);
            constrainGameLayerPosition();
            updateSlider();
        }
        return;
    }

    if (!fields->m_activeScroll) {
        fields->m_targetPos = layer->getPosition();
        fields->m_startSwipe = layer->convertToNodeSpace(m_swipeStart);
        fields->m_activeScroll = true;
    }

    float multiplier = getSetting<float, "scroll-multiplier">();

    float newX = (x * multiplier) * xMult;
    float newY = (y * multiplier) * yMult;

    if (getSetting<bool, "invert-vertical-scroll">()) {
        newY *= -1;
    }
    if (getSetting<bool, "invert-horizontal-scroll">()) {
        newX *= -1;
    }

    auto newPos = tinker::utils::rotatePointAroundPivot({newX, newY}, {0, 0}, m_editorLayer->m_gameState.m_cameraAngle);

    auto oldPos = fields->m_targetPos;
    fields->m_targetPos = fields->m_targetPos + newPos;

    if (getSetting<bool, "smooth-scroll-enabled">()) {
        if (oldPos.x != fields->m_targetPos.x) {
            if (fields->m_moveX) layer->stopAction(fields->m_moveX);
            fields->m_moveX = CCEaseOut::create(CCCallbackAction::create(CCMoveToX::create(0.1f * fields->m_speedScale, fields->m_targetPos.x), [this, fields] (auto target) {
                m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
                constrainGameLayerPosition();
                updateSlider();
            }), 1.2f);
            layer->runAction(fields->m_moveX);
        }
        if (oldPos.y != fields->m_targetPos.y) {
            if (fields->m_moveY) layer->stopAction(fields->m_moveY);
            fields->m_moveY = CCEaseOut::create(CCCallbackAction::create(CCMoveToY::create(0.1f * fields->m_speedScale, fields->m_targetPos.y), [this, fields] (auto target) {
                m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
                constrainGameLayerPosition();
                updateSlider();
            }), 1.2f);
            layer->runAction(fields->m_moveY);
        }
    }
    else {
        fields->m_activeScroll = false;
        layer->setPosition(fields->m_targetPos);
        m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
        constrainGameLayerPosition();
        updateSlider();
    }
}

void InputEditorUI::checkScrolling(float dt) {
    auto fields = m_fields.self();

    fields->m_speedScale = dt / CCDirector::get()->getDeltaTime();

    if (!tinker::utils::getSetting<bool, "smooth-scroll-enabled">()) {
        fields->m_activeScroll = false;
        fields->m_activeZoom = false;
        return;
    }

    fields->m_activeScroll = [this, fields] -> bool {
        if (fields->m_activeScroll) {
            if (fields->m_moveX && !fields->m_moveX->isDone()) {
                return true;
            }
            if (fields->m_moveY && !fields->m_moveY->isDone()) {
                return true;
            }
        }
        return false;
    }();
    fields->m_activeZoom = fields->m_scale && !fields->m_scale->isDone();
}

void InputEditorUI::addActiveAlert(CCNode* alert) {
    auto fields = m_fields.self();

    if (fields->m_activeAlerts.empty()) {
        AlertsActiveEvent().send();
    }

    fields->m_activeAlerts.insert(alert);
}

void InputEditorUI::removeActiveAlert(CCNode* alert) {
    auto fields = m_fields.self();

    fields->m_activeAlerts.erase(alert);
}

void InputEditorUI::scrollWheel(float y, float x) {
    if (!tinker::utils::getSetting<bool, "scroll-delegate-to-vanilla">()) return;
    onScroll();
}

CCPoint InputEditorUI::getTouchLocation(CCTouch* touch) {
    if (CanvasRotate::isEnabled()) {
        return CanvasRotate::get()->getPreTransformPoint(touch);
    }
    return touch->getLocation();
}

bool InputEditorUI::onTouchBegan(CCTouch* touch, geode::Function<bool(CCTouch* touch)> next) {
    auto fields = m_fields.self();

    if (CanvasRotate::isEnabled() && CanvasRotate::get()->isRotating()) {
        return false;
    }

    for (auto textInput : fields->m_textInputs) {
        if (nodeIsVisible(textInput) && isPointInsideNode(textInput, getTouchLocation(touch))) {
            return false;
        }
    }
    
    if (!fields->m_blockPinching && tinker::utils::getSetting<bool, "pinch-to-zoom">()) {
        auto mainPos = getTouchLocation(touch);
        if (mainPos.y <= tinker::utils::getToolbarHeight()) {
            if (m_editorLayer->m_playbackMode != PlaybackMode::Playing || m_playbackBtn->isVisible()) return false;

            m_editorLayer->m_uiLayer->ccTouchBegan(touch, nullptr);
            return true;
        }
        
        if (m_editorLayer->m_playbackMode != PlaybackMode::Playing && fields->m_touch1 && !fields->m_touch2) {
            stopActionByTag(123);
            
            auto firstPos = getTouchLocation(fields->m_touch1);
            auto secondPos = mainPos;

            fields->m_touchMidPoint = (firstPos + secondPos) / 2.f;
            fields->m_initialScale = std::max(m_editorLayer->m_objectLayer->getScale(), 0.01f);
            fields->m_initialDistance = std::max(firstPos.getDistance(secondPos), 0.01f);

            fields->m_touch2 = touch;

            if (CanvasRotate::isEnabled()) {
                fields->m_lastAngle = std::atan2(secondPos.y - firstPos.y, secondPos.x - firstPos.x);
            }

            fields->m_isPinching = true;
            m_swipeModeTriggered = false;
            m_isDraggingCamera = true;
            m_swipeSelected = false;
            m_swipeActive = false;

            return true;
        }
        else if (!fields->m_touch1 && next(touch)) {
            fields->m_touch1 = touch;
            return true;
        }
    }

    return next(touch);
}

void InputEditorUI::onTouchMoved(CCTouch* touch, geode::Function<void(CCTouch* touch)> next) {
    auto fields = m_fields.self();

    if (!fields->m_blockPinching && tinker::utils::getSetting<bool, "pinch-to-zoom">()) {
        if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
            fields->m_touch1 = nullptr;
            fields->m_touch2 = nullptr;
            fields->m_isPinching = false;
            fields->m_lastAngle = 0;
        }
        if (m_editorLayer->m_playbackMode != PlaybackMode::Playing && fields->m_touch1 && fields->m_touch2) {
            auto layer = m_editorLayer->m_objectLayer;
            stopActionByTag(123);

            auto firstPos = getTouchLocation(fields->m_touch1);
            auto secondPos = getTouchLocation(fields->m_touch2);

            auto center = (firstPos + secondPos) / 2.f;
            auto distNow = std::max(firstPos.getDistance(secondPos), 0.01f);

            if (std::abs(distNow) >= 20.f) {
                auto mult = fields->m_initialDistance / distNow;

                auto zoom = std::clamp(fields->m_initialScale / mult, tinker::utils::getSetting<float, "zoom-minimum">(), tinker::utils::getSetting<float, "zoom-maximum">());

                auto midPos = tinker::utils::rotatePointAroundPivot(fields->m_touchMidPoint, CCDirector::get()->getWinSize() / 2.f, m_editorLayer->m_gameState.m_cameraAngle);
                auto prevPos = layer->convertToNodeSpace(midPos);
            
                updateZoom(zoom);

                auto newPos = layer->convertToWorldSpace(prevPos);
                layer->setPosition(layer->getPosition() + midPos - newPos);

                fields->m_touchMidPoint = center;
                m_isDraggingCamera = true;
                
                if (CanvasRotate::isEnabled() && CanvasRotate::getSetting<bool, "pinch-to-rotate">()) {
                    auto diff = getTouchLocation(fields->m_touch2) - getTouchLocation(fields->m_touch1);
                    auto angle = std::atan2(diff.y, diff.x);
                    auto delta = angle - fields->m_lastAngle;

                    while (delta > M_PI) {
                        delta -= 2.f * M_PI;
                    }

                    while (delta < -M_PI) {
                        delta += 2.f * M_PI;
                    }

                    fields->m_lastAngle = angle;
                    CanvasRotate::get()->m_rotationNode->updateCanvasRotation(CC_RADIANS_TO_DEGREES(delta));
                }
                return;
            }
        }
    }
    if (!fields->m_isPinching) {
        next(touch);
    }
}

void InputEditorUI::onTouchEnded(CCTouch* touch, geode::Function<void(CCTouch* touch)> next) {
    auto fields = m_fields.self();

    if (tinker::utils::getSetting<bool, "pinch-to-zoom">()) {
        if (fields->m_touch1 == touch) {
            fields->m_touch1 = fields->m_touch2;
            fields->m_touch2 = nullptr;
        }
        if (fields->m_touch2 == touch) {
            fields->m_touch2 = nullptr;
        }

        if (!fields->m_touch1 && !fields->m_touch2) {
            fields->m_isPinching = false;
        }
    }

    next(touch);
}

void InputEditorUI::onTouchCancelled(CCTouch* touch, geode::Function<void(CCTouch* touch)> next) {
    onTouchEnded(touch, std::move(next));
}

void InputEditorUI::onPause(cocos2d::CCObject* sender) {
    if (!m_fields->m_blockPause) {
        EditorUI::onPause(sender);
    }
}

void InputEditorUI::blockPause() {
    m_fields->m_blockPause = true;
}

void InputEditorUI::unblockPause() {
    runAction(CallFuncExt::create([this] {
        m_fields->m_blockPause = false;
    }));
}

bool InputEditorUI::hasActiveAlerts() {
    return m_fields->m_activeAlerts.size() > 0;
}

void InputEditorPauseLayer::customSetup() {
    EditorPauseLayer::customSetup();
    #ifndef GEODE_IS_MACOS
    if (!EditorUI::get() || getUserFlag("ignore"_spr)) return;

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-exit-pause-menu"), [this] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down && !repeat && !InputEditorUI::get()->hasActiveAlerts()) {
            onResume(nullptr);
        }
    });

    addOnEnterCallback([] {
        if (auto editor = InputEditorUI::get()) {
            editor->blockPause();
        }
    });

    addOnExitCallback([] {
        if (auto editor = InputEditorUI::get()) {
            editor->unblockPause();
        }
    });
    #endif
}

class $baseModify(BlockingFLAlertLayer, FLAlertLayer) {
    void modify() {
        auto editor = InputEditorUI::get();
        if (!editor) return;

        if (typeinfo_cast<ColorSelectLiveOverlay*>(this) || typeinfo_cast<HSVLiveOverlay*>(this)) {
            return;
        }

        addOnEnterCallback([this] {
            auto editor = InputEditorUI::get();
            if (editor) {
                editor->addActiveAlert(this);
            }
        });

        addOnExitCallback([this] {
            auto editor = InputEditorUI::get();
            if (editor) {
                editor->removeActiveAlert(this);
            }
        });
    }
};

class $baseModify(BlockingGJDropDownLayer, GJDropDownLayer) {
    void modify() {
        auto editor = InputEditorUI::get();
        if (!editor) return;

        addOnEnterCallback([this] {
            auto editor = InputEditorUI::get();
            if (editor) {
                editor->addActiveAlert(this);
            }
        });

        addOnExitCallback([this] {
            auto editor = InputEditorUI::get();
            if (editor) {
                editor->removeActiveAlert(this);
            }
        });
    }
};
