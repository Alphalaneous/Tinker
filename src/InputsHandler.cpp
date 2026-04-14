#include "InputsHandler.hpp"
#include "Utils.hpp"
#include "modules/ScrollableObjects.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include "CCCallbackAction.hpp"

using namespace alpha::prelude;

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

    addEventListener(ScrollWheelEvent(), [this](double x, double y) {
        onScroll(x, y);
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

    addEventListener(KeyboardInputEvent(enumKeyCodes::KEY_LeftShift), [this] (KeyboardInputData& data) {
        if (data.action == KeyboardInputData::Action::Release) {
            m_swipeActive = false;
        }
    });
    addEventListener(KeyboardInputEvent(enumKeyCodes::KEY_RightShift), [this] (KeyboardInputData& data) {
        if (data.action == KeyboardInputData::Action::Release) {
            m_swipeActive = false;
        }
    });

    if (!std::isfinite(editorLayer->m_objectLayer->getPosition().x) || !std::isfinite(editorLayer->m_objectLayer->getPosition().y)) {
        editorLayer->m_objectLayer->setPosition({0, 90});
    }

    return true;
}

#ifdef GEODE_IS_MACOS
#include <CoreFoundation/CoreFoundation.h>
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

void InputEditorUI::onScroll(float x, float y) {
    using namespace tinker::utils;
    auto fields = m_fields.self();
    int naturalMult = isNaturalScrollEnabled() ? -1 : 1;

    #ifdef GEODE_IS_MACOS
    float xMult = 1;
    float yMult = 1;
    #else
    float xMult = 1;
    float yMult = -1;
    #endif

    xMult *= naturalMult;
    yMult *= naturalMult;

    for (auto alert : fields->m_activeAlerts) {
        if (alert && alert->getParentByType<CCScene>() && nodeIsVisible(alert)) {
            return;
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
            fields->m_targetPos = layer->getPosition();
            fields->m_targetScale = layer->getScale();
            fields->m_startSwipe = layer->convertToNodeSpace(m_swipeStart);
            fields->m_activeZoom = true;
        }
        fields->m_targetScale = std::max(fields->m_targetScale,0.1f);

        auto winSize = CCDirector::get()->getWinSize();
        CCPoint zoomPos;
        if (getSetting<bool, "zoom-to-cursor">()) {
            zoomPos = tinker::utils::rotatePointAroundPivot(getMousePos(), winSize/2, m_editorLayer->m_gameState.m_cameraAngle);
        }
        else {
            zoomPos = winSize / 2;
        }
        auto offset = zoomPos - fields->m_targetPos;

        float zoomFactor = 1.05f;
        float zoomSpeed = 0.2f;

        #ifdef GEODE_IS_MACOS
        float newY = (y * getSetting<float, "zoom-multiplier">()) * -yMult;
        #else
        float newY = (y * getSetting<float, "zoom-multiplier">()) * yMult;
        #endif

        newY *= (getSetting<bool, "invert-zoom-scroll">() ? -1 : 1);

        float newScale = fields->m_targetScale * std::powf(zoomFactor, -newY * zoomSpeed);
        newScale = std::min(std::max(newScale, getSetting<float, "zoom-minimum">()), getSetting<float, "zoom-maximum">());

        float scaleRatio = newScale / fields->m_targetScale;

        auto oldPos = fields->m_targetPos;
        auto oldScale = fields->m_targetScale;

        fields->m_targetPos = zoomPos - offset * scaleRatio;
        fields->m_targetScale = newScale;

        if (getSetting<bool, "smooth-scroll-enabled">()) {
            if (oldPos.x != fields->m_targetPos.x) {
                if (fields->m_moveX) layer->stopAction(fields->m_moveX);
                fields->m_moveX = CCEaseOut::create(CCCallbackAction::create(CCMoveToX::create(0.1f, fields->m_targetPos.x), [this, fields] (auto target) {
                    m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
                    constrainGameLayerPosition();
                }), 1.2f);
                layer->runAction(fields->m_moveX);
            }
            if (oldPos.y != fields->m_targetPos.y) {
                if (fields->m_moveY) layer->stopAction(fields->m_moveY);
                fields->m_moveY = CCEaseOut::create(CCCallbackAction::create(CCMoveToY::create(0.1f, fields->m_targetPos.y), [this, fields] (auto target) {
                    m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
                    constrainGameLayerPosition();
                }), 1.2f);
                layer->runAction(fields->m_moveY);
            }

            if (oldScale != fields->m_targetScale) {
                if (fields->m_scale) layer->stopAction(fields->m_scale);
                fields->m_scale = CCEaseOut::create(CCCallbackAction::create(CCScaleTo::create(0.1f, fields->m_targetScale), [this, fields] (auto target) {
                    m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
                    updateZoom(m_editorLayer->m_objectLayer->getScale());
                    constrainGameLayerPosition();
                }), 1.2f);
                layer->runAction(fields->m_scale);
            }
        }
        else {
            fields->m_activeZoom = false;

            layer->setScale(fields->m_targetScale);
            layer->setPosition(fields->m_targetPos);

            m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
            updateZoom(m_editorLayer->m_objectLayer->getScale());
            constrainGameLayerPosition();
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
            fields->m_moveX = CCEaseOut::create(CCCallbackAction::create(CCMoveToX::create(0.1f, fields->m_targetPos.x), [this, fields] (auto target) {
                m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
                constrainGameLayerPosition();
            }), 1.2f);
            layer->runAction(fields->m_moveX);
        }
        if (oldPos.y != fields->m_targetPos.y) {
            if (fields->m_moveY) layer->stopAction(fields->m_moveY);
            fields->m_moveY = CCEaseOut::create(CCCallbackAction::create(CCMoveToY::create(0.1f, fields->m_targetPos.y), [this, fields] (auto target) {
                m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
                constrainGameLayerPosition();
            }), 1.2f);
            layer->runAction(fields->m_moveY);
        }
    }
    else {
        fields->m_activeScroll = false;
        layer->setPosition(fields->m_targetPos);
        m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
        constrainGameLayerPosition();
    }
}

void InputEditorUI::checkScrolling(float dt) {
    auto fields = m_fields.self();

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
    fields->m_activeZoom = [this, fields] -> bool {
        if (fields->m_activeZoom) {
            if (fields->m_scale && !fields->m_scale->isDone()) {
                return true;
            }
        }
        return false;
    }();
}

void InputEditorUI::addActiveAlert(FLAlertLayer* alert) {
    auto fields = m_fields.self();

    fields->m_activeAlerts.insert(alert);
}

void InputEditorUI::removeActiveAlert(FLAlertLayer* alert) {
    auto fields = m_fields.self();

    fields->m_activeAlerts.erase(alert);
}

void InputEditorUI::scrollWheel(float y, float x) {
    //do nothing
}

class $baseModify(BlockingFLAlertLayer, FLAlertLayer) {
    struct Fields {
        FLAlertLayer* m_self;
        ~Fields() {
            auto editor = InputEditorUI::get();
            if (!editor) return;

            editor->removeActiveAlert(m_self);
        }
    };

    void modify() {
        auto editor = InputEditorUI::get();
        if (!editor) return;

        if (typeinfo_cast<ColorSelectLiveOverlay*>(this) || typeinfo_cast<HSVLiveOverlay*>(this)) {
            return;
        }

        auto fields = m_fields.self();
        fields->m_self = this;
        editor->addActiveAlert(this);
    }
};