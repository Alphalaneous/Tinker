#include "InputsHandler.hpp"
#include "Events.hpp"
#include "modules/BetterSelect.hpp"
#include "modules/ScrollableObjects.hpp"
#include "modules/Gizmos/Gizmos.hpp"
#include "actions/CCCallbackAction.hpp"
#include "actions/CCValueTo.hpp"
#include <alphalaneous.alphas_geode_utils/ObjectModify.hpp>
#include <alphalaneous.alphas-ui-pack/API.hpp>
#include <alphalaneous.editortab_api/API.hpp>
//#include <geode.devtools/API.hpp>
#include "nodes/ToolbarTouch.hpp"
#include "utils/Mod.hpp"
#include "utils/UI.hpp"
#include "utils/Settings.hpp"
#include "MainHooks.hpp"

using namespace geode::prelude;

using namespace alpha::prelude;

#ifdef GEODE_IS_MOBILE
void InputAppDelegate::applicationDidEnterBackground() {
    auto editor = InputEditorUI::get();
    if (!editor) {
        AppDelegate::applicationDidEnterBackground();
        return;
    }

    LostFocusEvent().send();
    AppDelegate::applicationDidEnterBackground();
}
#endif

InputEditorUI* InputEditorUI::get() {
    return modify_cast<InputEditorUI*>(MainEditorUI::get());
}

bool InputEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) return false;
    
    auto fields = m_fields.self();

    editorLayer->schedule(schedule_selector(InputLevelEditorLayer::checkScrolling));

    addEventListener(ScrollWheelEvent(), [this, fields](double x, double y) {
        fields->m_scroll = CCPoint{static_cast<float>(x), static_cast<float>(y)};
        //onScroll();
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "ScrollableObjects-speed-modifier-key"), [this, fields] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        fields->m_tabModifierHeld = down;
    });

    addEventListener(KeyboardInputEvent(enumKeyCodes::KEY_LeftShift), [this] (KeyboardInputData& data) {
        if (!m_swipeEnabled && data.action == KeyboardInputData::Action::Release && tinker::utils::settings::get<bool, "stop-swipe-on-shift-release">()) {
            m_swipeActive = false;
        }
    });
    
    addEventListener(KeyboardInputEvent(enumKeyCodes::KEY_RightShift), [this] (KeyboardInputData& data) {
        if (!m_swipeEnabled && data.action == KeyboardInputData::Action::Release && tinker::utils::settings::get<bool, "stop-swipe-on-shift-release">()) {
            m_swipeActive = false;
        }
    });

    if (!std::isfinite(editorLayer->m_objectLayer->getPosition().x) || !std::isfinite(editorLayer->m_objectLayer->getPosition().y) || editorLayer->m_level->m_lastBuildSave->count() == 0) {
        editorLayer->m_objectLayer->setPosition({130, tinker::utils::ui::getToolbarHeight() / 2.f});
    }


    /*auto toolbarTouch = tinker::ui::ToolbarTouch::create();
    toolbarTouch->setZOrder(1);
    toolbarTouch->setID("toolbar-touch"_spr);

    addChild(toolbarTouch);*/

    return true;
}

bool InputEditorUI::handleAction(int action, bool down, double timestamp) {
    if (down && m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
        switch (action) {
            case 10010: {
                onStopPlaytest(nullptr);
                onPlaytest(nullptr);
                return true;
            }
            default: break;
        }
    }

    return EditorUI::handleAction(action, down, timestamp);
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

void InputEditorUI::cancelAllTextInputs() {
    for (auto input : m_fields->m_allTextInputs) {
        input->onClickTrackNode(false);
    }
}

#ifdef GEODE_IS_MACOS
#include <CoreFoundation/CoreFoundation.h>
#include "utils/MacUtils.hpp"

bool InputEditorUI::isNaturalScrollEnabled() {
    if (!tinker::utils::settings::get<bool, "ignore-natural-scrolling">()) return false;
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

// TODO FIX DEVTOOLS
bool InputEditorUI::isInDevtools() {
    return false; //getMousePos() == getRealMousePos() && devtools::isOpen();
}

bool InputEditorUI::onScroll() {
    auto quickVolume = CCScene::get()->getChildByID("hjfod.quick-volume-controls/overlay");
    if (quickVolume) {
        auto scale9 = quickVolume->getChildByType<CCScale9Sprite>(0);
        if (scale9->getOpacity() != 0) return false;
    }
    auto editorPause = CCScene::get()->getChildByType<EditorPauseLayer>(0);
    if (editorPause) return false;

    auto fields = m_fields.self();
    float x = fields->m_scroll.x;
    float y = fields->m_scroll.y;

    if (fields->m_scroll.equals({0.f, 0.f})) {
        return false;
    }

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
            return false;
        }
    }

    if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) return false;

    auto mousePos = getMousePos();

    if (m_dpadMoveNode) {
        auto scrollNode = m_dpadMoveNode->getChildByID("alphalaneous.editortab_api/edit-button-scroll-layer");
        if (scrollNode) {
            if (nodeIsVisible(scrollNode) && isPointInsideNode(scrollNode, mousePos)) return false;
        }
    }

    if (isInDevtools()) return false;
    
    if (mousePos.y < tinker::utils::ui::getToolbarHeight()) {
        auto currentTab = alpha::editor_tabs::getCurrentTab();
        if (currentTab) {
            auto node = currentTab->getNode();
            if (node && node->getUserFlag("disable-editor-scroll"_spr)) {
                return false;
            }
        }
    }

    for (auto textInput : fields->m_allTextInputs) {
        auto parent = textInput->getParentByType<geode::TextInput>();
        if (!parent) continue;

        if (nodeIsVisible(parent) && isPointInsideNode(parent, mousePos)) {
            return false;
        }
    }

    if (ScrollableObjects::isEnabled() && !ScrollableObjects::get()->canScroll()) {
        return false;
    }

    if (BetterSelect::isEnabled() && BetterSelect::get()->hoveringObjects()) {
        return false;
    }

    if (CCKeyboardDispatcher::get()->getShiftKeyPressed()) {
        std::swap(x, y);
    }

    auto layer = m_editorLayer->m_objectLayer;

    if (CCKeyboardDispatcher::get()->getControlKeyPressed()) {
        if (fields->m_moveX) layer->stopAction(fields->m_moveX);
        if (fields->m_moveY) layer->stopAction(fields->m_moveY);
        fields->m_activeScroll = false;

        if (!fields->m_activeZoom) {
            fields->m_targetScale = layer->getScale();
            fields->m_startSwipe = layer->convertToNodeSpace(m_swipeStart);
            fields->m_activeZoom = true;
        }
        fields->m_targetScale = std::max(fields->m_targetScale, tinker::utils::settings::get<float, "zoom-minimum">());

        auto winSize = CCDirector::get()->getWinSize();

        float zoomFactor = 1.05f;
        float zoomSpeed = 0.2f;

        #ifdef GEODE_IS_MACOS
        float newY = (y * tinker::utils::settings::get<float, "zoom-multiplier">()) * -yMult;
        #else
        float newY = (y * tinker::utils::settings::get<float, "zoom-multiplier">()) * yMult;
        #endif

        newY *= (tinker::utils::settings::get<bool, "invert-zoom-scroll">() ? -1 : 1);

        auto oldScale = fields->m_targetScale;

        float newScale = fields->m_targetScale * std::powf(zoomFactor, -newY * zoomSpeed);
        fields->m_targetScale = std::clamp(newScale, tinker::utils::settings::get<float, "zoom-minimum">(), tinker::utils::settings::get<float, "zoom-maximum">());

        if (tinker::utils::settings::get<bool, "smooth-scroll-enabled">()) {
            if (oldScale != fields->m_targetScale) {
                if (fields->m_scale) layer->stopAction(fields->m_scale);

                fields->m_scale = CCEaseOut::create(CCValueTo<float>::create(0.1f * fields->m_speedScale, std::clamp(layer->getScale(), tinker::utils::settings::get<float, "zoom-minimum">(), tinker::utils::settings::get<float, "zoom-maximum">()), fields->m_targetScale, [this, layer, fields, winSize] (float t, float start, float end, float& scale) {
                    scale = start + (end - start) * t;
                    if (scale == 0.f) return;

                    if (tinker::utils::settings::get<bool, "zoom-to-cursor">()) {
                        auto mousePos = tinker::utils::ui::rotatePointAroundPivot(getMousePos(), winSize / 2.f, m_editorLayer->m_gameState.m_cameraAngle);
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

            if (tinker::utils::settings::get<bool, "zoom-to-cursor">()) {
                auto mousePos = tinker::utils::ui::rotatePointAroundPivot(getMousePos(), winSize / 2.f, m_editorLayer->m_gameState.m_cameraAngle);
                if (mousePos.equals({0.f, 0.f})) return true;
                
                auto prevPos = layer->convertToNodeSpace(mousePos);
            
                if (fields->m_targetScale == 0.f) return true;
                updateZoom(fields->m_targetScale);

                auto newPos = layer->convertToWorldSpace(prevPos);
                layer->setPosition(layer->getPosition() + mousePos - newPos);
            }
            else {
                if (fields->m_targetScale == 0.f) return true;
                updateZoom(fields->m_targetScale);
            }
            m_swipeStart = layer->convertToWorldSpace(fields->m_startSwipe);
            constrainGameLayerPosition();
            updateSlider();
        }
        return true;
    }
    if (fields->m_scale) layer->stopAction(fields->m_scale);
    fields->m_activeZoom = false;

    if (!fields->m_activeScroll) {
        fields->m_targetPos = layer->getPosition();
        fields->m_startSwipe = layer->convertToNodeSpace(m_swipeStart);
        fields->m_activeScroll = true;
    }

    float multiplier = tinker::utils::settings::get<float, "scroll-multiplier">();

    float newX = (x * multiplier) * xMult;
    float newY = (y * multiplier) * yMult;

    if (tinker::utils::settings::get<bool, "invert-vertical-scroll">()) {
        newY *= -1;
    }
    if (tinker::utils::settings::get<bool, "invert-horizontal-scroll">()) {
        newX *= -1;
    }

    auto newPos = tinker::utils::ui::rotatePointAroundPivot({newX, newY}, {0.f, 0.f}, m_editorLayer->m_gameState.m_cameraAngle);

    auto oldPos = fields->m_targetPos;
    fields->m_targetPos = fields->m_targetPos + newPos;

    if (tinker::utils::settings::get<bool, "smooth-scroll-enabled">()) {
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

    return true;
}

void InputLevelEditorLayer::checkScrolling(float dt) {
    auto fields = InputEditorUI::get()->m_fields.self();

    fields->m_speedScale = dt / CCDirector::get()->getDeltaTime();

    if (!tinker::utils::settings::get<bool, "smooth-scroll-enabled">()) {
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

void InputEditorUI::addActiveInput(CCTextInputNode* input) {
    auto fields = m_fields.self();

    fields->m_allTextInputs.insert(input);
}

void InputEditorUI::removeActiveInput(CCTextInputNode* input) {
    auto fields = m_fields.self();

    fields->m_allTextInputs.erase(input);
}

bool InputEditorUI::scrollWheel(float y, float x) {
    auto fields = m_fields.self();

    bool stillEditorScroll = false;
    auto smoothScroll = tinker::utils::mod::getMod<"prevter.smooth-scroll">();
    bool hasSmoothScroll = false;
    if (smoothScroll) {
        hasSmoothScroll = smoothScroll->getSettingValue<bool>("scroll-enable-in-editor");
    }

    if (hasSmoothScroll) {
        stillEditorScroll = std::abs(fields->m_scrollFromEditor) > 0.1 || std::abs(fields->m_scrollFromEditor) > std::abs(y);
    }

    for (auto alert : fields->m_activeAlerts) {
        if (alert && alert->getParentByType<CCScene>() && nodeIsVisible(alert)) {
            return false;
        }
    }

    if (isInDevtools()) {
        return false;
    }

    auto editorPause = CCScene::get()->getChildByType<EditorPauseLayer>(0);
    if (editorPause) {
        return false;
    }

    // TODO cleanup, use Editor Tab API
    if (ScrollableObjects::isEnabled() && !ScrollableObjects::get()->canScroll() && !stillEditorScroll) {
        for (auto child : getChildrenExt()) {
            if (!nodeIsVisible(child)) continue;

            bool invertScroll = ScrollableObjects::getSetting<bool, "invert-scroll">();

            if (auto bar = static_cast<SOEditButtonBar*>(typeinfo_cast<EditButtonBar*>(child))) {
                auto barFields = bar->m_fields.self();
                float multiplier = fields->m_tabModifierHeld ? ScrollableObjects::getSetting<float, "speed-modifier">() : 1;

                // only allow scroll to propagate through the editor, not itself
                if (barFields->m_scrollLayer) {
                    barFields->m_scrollLayer->setVerticalScrollWheel(true);
                    barFields->m_scrollLayer->scrollWheel(y * multiplier * (invertScroll ? -1 : 1), 0.f);
                    barFields->m_scrollLayer->setVerticalScrollWheel(false);
                }
            }
        }
        fields->m_scrollFromEditor = 0.f;
        return true;
    }
    fields->m_scrollFromEditor = y;
    return onScroll();
    //return EditorUI::scrollWheel(y, x);
}

/*bool InputEditorUI::onTouchBegan(CCTouch* touch, geode::Function<bool(CCTouch* touch)> next) {
    auto fields = m_fields.self();
    
    if (CanvasRotate::isEnabled() && CanvasRotate::get()->isRotating()) {
        return false;
    }

    for (auto textInput : fields->m_textInputs) {
        if (nodeIsVisible(textInput) && isPointInsideNode(textInput, getTouchLocation(touch))) {
            return false;
        }
    }

    if (m_rotationTouchID == -1 && m_rotationControl && m_rotationControl->isVisible() && m_rotationControl->ccTouchBegan(touch, nullptr)) {
        m_rotationTouchID = touch->m_nId;
        return true;
    }

    if (m_scaleTouchID == -1 && m_scaleControl && m_scaleControl->isVisible() && m_scaleControl->ccTouchBegan(touch, nullptr)) {
        m_scaleTouchID = touch->m_nId;
        return true;
    }

    if (m_transformTouchID == -1 && m_transformControl && m_transformControl->isVisible() && m_transformControl->ccTouchBegan(touch, nullptr)) {
        m_transformTouchID = touch->m_nId;
        return true;
    }

    auto mainPos = getTouchLocation(touch);
    bool aboveToolbar = mainPos.y > tinker::utils::ui::getToolbarHeight();
    bool playing = m_editorLayer->m_playbackMode == PlaybackMode::Playing;

    if (aboveToolbar) {
        std::unordered_map<Gizmo*, std::set<CCTargetedTouchHandler*>> gizmoToHandlers{};
        std::map<ZLayer, std::vector<Gizmo*>> zLayerGizmos{};

        auto handlers = CCTouchDispatcher::get()->m_pTargetedHandlers->asExt<CCTargetedTouchHandler>();
        for (int i = handlers.size() - 1; i >= 0; i--) {
            auto node = typeinfo_cast<CCNode*>(handlers[i]->getDelegate());
            if (!node) continue;

            auto gizmo = node->getParentByType<Gizmo>();
            if (!gizmo) continue;

            if (gizmo->getOpacity() == 0) continue;
            if (playing && !gizmo->isInteractiveDuringPlaytest()) continue;

            gizmoToHandlers[gizmo].insert(handlers[i]);
            zLayerGizmos[gizmo->getObjectZLayer()].push_back(gizmo);
        }

        for (auto& [k, v] : zLayerGizmos) {
            std::sort(v.begin(), v.end(), [](Gizmo* a, Gizmo* b) {
                return a->getObjectZOrder() > b->getObjectZOrder(); 
            });
        }

        for (auto it = zLayerGizmos.rbegin(); it != zLayerGizmos.rend(); ++it) {
            for (auto gizmo : it->second) {
                if (m_editorLayer->validGroup(gizmo, false)) {
                    auto& handlers = gizmoToHandlers[gizmo];
                    for (auto handler : handlers) {
                        bool began = handler->getDelegate()->ccTouchBegan(touch, nullptr);
                        if (!began) continue;

                        if (handler->m_pClaimedTouches) {
                            handler->m_pClaimedTouches->addObject(touch);
                        }
                        fields->m_inGizmo = true;
                        fields->m_activeGizmoTouch = gizmo;
                        fields->m_touchedGizmoChild = static_cast<CCTargetedTouchDelegate*>(handler->getDelegate());
                        return true;
                    }
                }
            }
        }
    }

    if (!fields->m_blockPinching && tinker::utils::settings::get<bool, "pinch-to-zoom">()) {
        if (mainPos.y <= tinker::utils::ui::getToolbarHeight()) {
            if (!playing || m_playbackBtn->isVisible()) return false;

            m_editorLayer->m_uiLayer->ccTouchBegan(touch, nullptr);
            return true;
        }
        
        if (!playing && fields->m_touch1 && !fields->m_touch2) {
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
}*/

bool InputEditorUI::tryCancelGizmo(CCTouch* touch) {
    auto fields = m_fields.self();
    if (!m_editorLayer->validGroup(fields->m_activeGizmoTouch, false)) {
        fields->m_touchedGizmoChild->ccTouchCancelled(touch, nullptr);
        fields->m_touchedGizmoChild = nullptr;
        fields->m_activeGizmoTouch = nullptr;
        fields->m_inGizmo = false;
        return true;
    }
    return false;
}

/*void InputEditorUI::onTouchMoved(CCTouch* touch, geode::Function<void(CCTouch* touch)> next) {
    auto fields = m_fields.self();

    if (fields->m_inGizmo) {
        if (tryCancelGizmo(touch)) return;
        fields->m_touchedGizmoChild->ccTouchMoved(touch, nullptr);
        return;
    }

    if (!fields->m_blockPinching && tinker::utils::settings::get<bool, "pinch-to-zoom">()) {
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

                auto zoom = std::clamp(fields->m_initialScale / mult, tinker::utils::settings::get<float, "zoom-minimum">(), tinker::utils::settings::get<float, "zoom-maximum">());

                auto midPos = tinker::utils::ui::rotatePointAroundPivot(fields->m_touchMidPoint, CCDirector::get()->getWinSize() / 2.f, m_editorLayer->m_gameState.m_cameraAngle);
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
}*/

/*void InputEditorUI::onTouchEnded(CCTouch* touch, geode::Function<void(CCTouch* touch)> next) {
    auto fields = m_fields.self();

    if (fields->m_inGizmo) {
        fields->m_touchedGizmoChild->ccTouchEnded(touch, nullptr);
        fields->m_inGizmo = false;
        fields->m_touchedGizmoChild = nullptr;
        fields->m_activeGizmoTouch = nullptr;
        return;
    }

    if (tinker::utils::settings::get<bool, "pinch-to-zoom">()) {
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
    auto fields = m_fields.self();

    if (fields->m_inGizmo) {
        fields->m_touchedGizmoChild->ccTouchCancelled(touch, nullptr);
        fields->m_inGizmo = false;
        fields->m_touchedGizmoChild = nullptr;
        fields->m_activeGizmoTouch = nullptr;
        return;
    }

    onTouchEnded(touch, std::move(next));
}*/

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
    if (!EditorUI::get() || getUserFlag("ignore"_spr)) return;

    InputEditorUI::get()->cancelAllTextInputs();

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "Keybinds-exit-pause-menu"), [this] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (down && !repeat && !InputEditorUI::get()->hasActiveAlerts()) {
            auto ref = Ref(this);
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
}

class $baseModify(InputCCTextInputNode, CCTextInputNode) {
    void modify() {
        if (!tinker::utils::mod::shouldLoadTinker()) return;

        addOnEnterCallback([this] {
            auto editor = InputEditorUI::get();
            if (editor) {
                editor->addActiveInput(this);
            }
        });

        addOnExitCallback([this] {
            auto editor = InputEditorUI::get();
            if (editor) {
                editor->removeActiveInput(this);
            }
        });
    }
};

class $baseModify(BlockingFLAlertLayer, FLAlertLayer) {
    void modify() {
        if (!tinker::utils::mod::shouldLoadTinker()) return;

        auto editor = InputEditorUI::get();
        if (!editor) return;

        if (typeinfo_cast<ColorSelectLiveOverlay*>(this) || typeinfo_cast<HSVLiveOverlay*>(this) || typeinfo_cast<EditorWindowPopup*>(this)) {
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
        if (!tinker::utils::mod::shouldLoadTinker()) return;

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
