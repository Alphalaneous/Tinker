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
        onScroll(x * -1, y * -1);
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

    return true;
}

bool InputEditorUI::hasSmoothScroll() {
    static auto enabled = Mod::get()->getSettingValue<bool>("smooth-scroll-enabled");
    static auto listener = listenForSettingChanges<bool>("smooth-scroll-enabled", [] (bool value) {
        enabled = value;
    });
    return enabled;
}

float InputEditorUI::getScrollMultiplier() {
    static auto multiplier = Mod::get()->getSettingValue<float>("scroll-multiplier");
    static auto listener = listenForSettingChanges<float>("scroll-multiplier", [] (float value) {
        multiplier = value;
    });
    return multiplier;
}

float InputEditorUI::getZoomMultiplier() {
    static auto multiplier = Mod::get()->getSettingValue<float>("zoom-multiplier");
    static auto listener = listenForSettingChanges<float>("zoom-multiplier", [] (float value) {
        multiplier = value;
    });
    return multiplier;
}

float InputEditorUI::getToolbarScrollSpeedMultiplier() {
    static auto multiplier = Mod::get()->getSettingValue<float>("ScrollableObjects-speed-modifier");
    static auto listener = listenForSettingChanges<float>("ScrollableObjects-speed-modifier", [] (float value) {
        multiplier = value;
    });
    return multiplier;
}

void InputEditorUI::onScroll(float x, float y) {
    auto fields = m_fields.self();

    for (auto alert : fields->m_activeAlerts) {
        if (alert && alert->getParentByType<CCScene>() && nodeIsVisible(alert)) {
            return;
        }
    }

    if (ScrollableObjects::isEnabled() && !ScrollableObjects::get()->canScroll()) {
        for (auto child : getChildrenExt()) {
            if (!nodeIsVisible(child)) continue;

            if (auto bar = static_cast<SOEditButtonBar*>(typeinfo_cast<EditButtonBar*>(child))) {
                auto barFields = bar->m_fields.self();
                float multiplier = fields->m_tabModifierHeld ? 12 * getToolbarScrollSpeedMultiplier() : 12;
                barFields->m_scrollBar->scroll(x * multiplier, y * multiplier);
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

        auto winSize = CCDirector::get()->getWinSize();
        auto mousePos = tinker::utils::rotatePointAroundPivot(getMousePos(), winSize/2, m_editorLayer->m_gameState.m_cameraAngle);
        auto offset = mousePos - fields->m_targetPos;

        float zoomFactor = 1.05f;
        float zoomSpeed = 0.2f;
        float zoomLimit = 4.f;

        if (tinker::utils::getMod<"hjfod.betteredit">()) {
            zoomLimit = 10000000.f;
        }

        float newY = y * getZoomMultiplier();

        float newScale = fields->m_targetScale * std::powf(zoomFactor, -newY * zoomSpeed);
        newScale = std::min(std::max(newScale, 0.1f), zoomLimit);

        float scaleRatio = newScale / fields->m_targetScale;

        auto oldPos = fields->m_targetPos;
        auto oldScale = fields->m_targetScale;

        fields->m_targetPos = mousePos - offset * scaleRatio;
        fields->m_targetScale = newScale;

        if (hasSmoothScroll()) {
            if (oldPos.x != fields->m_targetPos.x) {
                if (fields->m_moveX) layer->stopAction(fields->m_moveX);
                fields->m_moveX = CCEaseOut::create(CCCallbackAction::create(CCMoveToX::create(0.1f, fields->m_targetPos.x), [this, fields] (auto target) {
                    m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
                }), 1.2f);
                layer->runAction(fields->m_moveX);
            }
            if (oldPos.y != fields->m_targetPos.y) {
                if (fields->m_moveY) layer->stopAction(fields->m_moveY);
                fields->m_moveY = CCEaseOut::create(CCCallbackAction::create(CCMoveToY::create(0.1f, fields->m_targetPos.y), [this, fields] (auto target) {
                    m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
                }), 1.2f);
                layer->runAction(fields->m_moveY);
            }

            if (oldScale != fields->m_targetScale) {
                if (fields->m_scale) layer->stopAction(fields->m_scale);
                fields->m_scale = CCEaseOut::create(CCCallbackAction::create(CCScaleTo::create(0.1f, fields->m_targetScale), [this, fields] (auto target) {
                    m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
                    updateZoom(m_editorLayer->m_objectLayer->getScale());
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
        }

        return;
    }

    #ifdef GEODE_IS_WINDOWS
        x *= -1;
    #endif

    if (!fields->m_activeScroll) {
        fields->m_targetPos = layer->getPosition();
        fields->m_startSwipe = layer->convertToNodeSpace(m_swipeStart);
        fields->m_activeScroll = true;
    }

    float newX = x * getScrollMultiplier();
    float newY = y * getScrollMultiplier();

    auto newPos = tinker::utils::rotatePointAroundPivot({newX, newY}, {0, 0}, m_editorLayer->m_gameState.m_cameraAngle);

    auto oldPos = fields->m_targetPos;
    fields->m_targetPos = fields->m_targetPos + newPos;

    if (hasSmoothScroll()) {
        if (oldPos.x != fields->m_targetPos.x) {
            if (fields->m_moveX) layer->stopAction(fields->m_moveX);
            fields->m_moveX = CCEaseOut::create(CCCallbackAction::create(CCMoveToX::create(0.1f, fields->m_targetPos.x), [this, fields] (auto target) {
                m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
            }), 1.2f);
            layer->runAction(fields->m_moveX);
        }
        if (oldPos.y != fields->m_targetPos.y) {
            if (fields->m_moveY) layer->stopAction(fields->m_moveY);
            fields->m_moveY = CCEaseOut::create(CCCallbackAction::create(CCMoveToY::create(0.1f, fields->m_targetPos.y), [this, fields] (auto target) {
                m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
            }), 1.2f);
            layer->runAction(fields->m_moveY);
        }
    }
    else {
        fields->m_activeScroll = false;
        layer->setPosition(fields->m_targetPos);
        m_swipeStart = m_editorLayer->m_objectLayer->convertToWorldSpace(fields->m_startSwipe);
    }
}

void InputEditorUI::checkScrolling(float dt) {
    auto fields = m_fields.self();

    if (!hasSmoothScroll()) {
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