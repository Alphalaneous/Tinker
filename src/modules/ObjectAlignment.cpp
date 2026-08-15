#include "modules/ObjectAlignment.hpp"
#include "modules/TogglerOverflow.hpp"
#include "utils/Utils.hpp"
#include <alphalaneous.editorsounds/include/API.hpp>

bool ObjectAlignment::onSettingChanged(std::string_view key, const matjson::Value& value) {
    if (key == "snap-distance" || key == "align-modifier") return true;

    if (key == "show-toggle") {
        auto res = value.asBool();
        if (!res) return true;

        auto editor = getEditor();
        auto show = res.unwrap();

        if (!show) {
            m_alignToggled = false;
            m_toggler->removeFromParent();

            auto menu = editor->getChildByID("toolbar-toggles-menu");
            if (!menu) return true;

            menu->updateLayout();
        }
        else {
            m_alignToggled = Mod::get()->getSavedValue<bool>("object-alignment-toggle", false);
            auto menu = editor->getChildByID("toolbar-toggles-menu");
            if (!menu) return true;

            menu->addChild(m_toggler);
            m_toggler->toggle(m_alignToggled);

            menu->updateLayout();
        }

        if (TogglerOverflow::isEnabled()) {
            TogglerOverflow::get()->updateContainer();
        }

        return true;
    }

    return false;
}

ObjectAlignment::SnapEdge ObjectAlignment::closestSnapEdge(CCRect bounds) {
    float snapDistance = getSetting<float, "snap-distance">();

    std::optional<float> horizontalSource;
    std::optional<float> horizontalTarget;

    std::optional<float> verticalSource;
    std::optional<float> verticalTarget;

    float closestHorizontalDist = FLT_MAX;
    float closestVerticalDist = FLT_MAX;

    const float boundsX[] = {
        bounds.getMidX(),
        bounds.getMinX(),
        bounds.getMaxX()
    };

    const float boundsY[] = {
        bounds.getMidY(),
        bounds.getMinY(),
        bounds.getMaxY()
    };

    auto editor = getEditor();
    auto editorLayer = getEditorLayer();

    tinker::utils::forEachObject(editorLayer, [&](GameObject* object) {
        if (editorLayer->m_currentLayer != -1 
            && object->m_editorLayer != editorLayer->m_currentLayer 
            && object->m_editorLayer2 != editorLayer->m_currentLayer) return;
        if (editor->m_selectedObject == object) return;
        if (editor->m_selectedObjects->containsObject(object)) return;
        if (object->getPosition().getDistance({bounds.getMidX(), bounds.getMidY()}) > 200.f) return;

        auto objBounds = object->boundingBox();

        const float objectX[] = {
            objBounds.getMidX(),
            objBounds.getMinX(),
            objBounds.getMaxX()
        };

        const float objectY[] = {
            objBounds.getMidY(),
            objBounds.getMinY(),
            objBounds.getMaxY()
        };

        for (float edge : boundsX) {
            for (float target : objectX) {
                float dist = std::abs(edge - target);

                if (dist < snapDistance && dist < closestHorizontalDist) {
                    closestHorizontalDist = dist;
                    horizontalSource = edge;
                    horizontalTarget = target;
                }
            }
        }

        for (float edge : boundsY) {
            for (float target : objectY) {
                float dist = std::abs(edge - target);

                if (dist < snapDistance && dist < closestVerticalDist) {
                    closestVerticalDist = dist;
                    verticalSource = edge;
                    verticalTarget = target;
                }
            }
        }
    });

    return {
        horizontalSource,
        horizontalTarget,
        verticalSource,
        verticalTarget
    };
}

CCRect ObjectAlignment::getSelectedBounds() {
    auto editor = getEditor();
    if (editor->m_selectedObject) {
        return editor->m_selectedObject->boundingBox();
    }
    else if (editor->m_selectedObjects->count() > 0) {
        auto bb = editor->m_selectedObjects->asExt<GameObject>()[0]->boundingBox();
        float minX = bb.getMinX();
        float maxX = bb.getMaxX();
        float minY = bb.getMinY();
        float maxY = bb.getMaxY();

        for (auto obj : editor->m_selectedObjects->asExt<GameObject>()) {
            auto bb = obj->boundingBox();
            if (bb.getMinX() < minX) {
                minX = bb.getMinX();
            }
            if (bb.getMaxX() > maxX) {
                maxX = bb.getMaxX();
            }
            if (bb.getMinY() < minY) {
                minY = bb.getMinY();
            }
            if (bb.getMaxX() > maxY) {
                maxY = bb.getMaxY();
            }
        }
        return CCRect{minX, minY, maxX - minX, maxY - minY};
    }
    return {};
}

bool ObjectAlignment::onTouchBegan(CCTouch* touch, geode::Function<bool(CCTouch* touch)> next) {
    m_closestEdges = {};
    return next(touch);
}

void ObjectAlignment::onTouchMoved(CCTouch* touch, geode::Function<void(CCTouch* touch)> next) {
    next(touch);
    auto editor = getEditor();
    if (editor->m_snapObject) {
        m_closestEdges = closestSnapEdge(getSelectedBounds());
    }
}

void ObjectAlignment::onTouchEnded(CCTouch* touch, geode::Function<void(CCTouch* touch)> next) {
    auto editor = getEditor();
    if (!editor->m_snapObject || (!m_alignActive && !m_alignToggled)) {
        next(touch);
        return;
    }

    float deltaX = 0.f;
    if (m_closestEdges.horizontalTarget) {
        deltaX = m_closestEdges.horizontalTarget.value() - m_closestEdges.horizontalSource.value();
    }

    float deltaY = 0.f;
    if (m_closestEdges.verticalTarget) {
        deltaY = m_closestEdges.verticalTarget.value() - m_closestEdges.verticalSource.value();
    }

    if (deltaX != 0.f || deltaY != 0.f) {
        editor->m_snapObjectExists = false;
    }

    next(touch);

    if (editor->m_selectedObjects->count() == 0) {
        editor->moveObject(editor->m_selectedObject, {deltaX, deltaY});
    } 
    else {
        for (auto obj : CCArrayExt<GameObject, false>(editor->m_selectedObjects)) {
            editor->moveObject(obj, {deltaX, deltaY});
        }
    }
}

void ObjectAlignment::onTouchCancelled(CCTouch* touch, geode::Function<void(CCTouch* touch)> next) {
    next(touch);
}

void ObjectAlignment::onEditor() {
    auto editor = getEditor();
    auto editorLayer = getEditorLayer();
    m_alignmentNode = CCDrawNode::create();
    m_alignmentNode->setID("object-align-draw"_spr);
    m_alignmentNode->m_bUseArea = false;
    m_alignmentNode->setZOrder(9999);
    editorLayer->m_objectLayer->addChild(m_alignmentNode);

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "ObjectAlignment-align-modifier"), [this] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        m_alignActive = down;
    });

    editorLayer->schedule(schedule_selector(OALevelEditorLayer::updateAlignmentDraw));

    auto menu = editor->getChildByID("toolbar-toggles-menu");
    if (!menu) return;

    auto spr = CCSprite::create("align.png"_spr);
    spr->setID("object-align-sprite"_spr);
    
    auto sprOn = ButtonSprite::create(spr, 40, true, 40.f, "GJ_button_02.png", 1.f);
    auto sprOff = ButtonSprite::create(spr, 40, true, 40.f, "GJ_button_01.png", 1.f);
    
    sprOn->setID("object-align-sprite-on"_spr);
    sprOff->setID("object-align-sprite-off"_spr);

    sprOn->setContentSize({40.f, 40.f});
    sprOff->setContentSize({40.f, 40.f});

    m_toggler = CCMenuItemExt::createToggler(sprOn, sprOff, [this] (CCMenuItemToggler* toggler) {
        m_alignToggled = !toggler->isToggled();
        Mod::get()->setSavedValue<bool>("object-alignment-toggle", m_alignToggled);
    });
    m_toggler->setID("object-align-button"_spr);
    alpha::editor_sounds::assignToMenuItem(m_toggler, "toolbar-toggles");

    if (getSetting<bool, "show-toggle">()) {
        bool isToggled = Mod::get()->getSavedValue<bool>("object-alignment-toggle", false);

        m_toggler->toggle(isToggled);
        m_alignToggled = isToggled;

        menu->addChild(m_toggler);
        
        menu->updateLayout();
    }
    editor->m_uiItems->addObject(m_toggler);
}

void OALevelEditorLayer::updateAlignmentDraw(float dt) {
    auto objectAlignment = ObjectAlignment::get();
    objectAlignment->m_alignmentNode->clear();

    if (!m_editorUI->m_snapObject) return;
    if (!objectAlignment->m_alignActive && !objectAlignment->m_alignToggled) return;

    auto& edges = objectAlignment->m_closestEdges;

    auto winSize = CCDirector::get()->getWinSize();
    float scale = m_objectLayer->getScale();

    float width = 2.f / CCDirector::get()->getContentScaleFactor() / scale;

    auto bottomLeft = m_objectLayer->convertToNodeSpace({0.f, 0.f});
    auto topRight = m_objectLayer->convertToNodeSpace(winSize);

    if (edges.horizontalTarget.has_value()) {
        float minY = bottomLeft.y - (winSize.height / 2.f) / scale;
        float maxY = topRight.y + (winSize.height / 2.f) / scale;

        objectAlignment->m_alignmentNode->drawSegment({edges.horizontalTarget.value(), minY}, {edges.horizontalTarget.value(), maxY}, width, {1.f, 0.7f, 0.f, 1.f});
    }
    if (edges.verticalTarget.has_value()) {
        float minX = bottomLeft.x - (winSize.width / 2.f) / scale;
        float maxX = topRight.x + (winSize.width / 2.f) / scale;

        objectAlignment->m_alignmentNode->drawSegment({minX, edges.verticalTarget.value()}, {maxX, edges.verticalTarget.value()}, width, {1.f, 0.7f, 0.f, 1.f});
    }
}