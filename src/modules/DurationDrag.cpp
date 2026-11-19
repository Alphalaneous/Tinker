#include "modules/DurationDrag.hpp"
#include "utils/Constants.hpp"
#include "utils/Objects.hpp"
#include <alphalaneous.good_grid/API.hpp>

using namespace geode::prelude;

using namespace tinker::ui;

bool DurationDrag::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        if (m_durationControl) {
            m_durationControl->removeFromParent();
            m_durationControl = nullptr;
        }
        if (m_durationDragDraw) {
            m_durationDragDraw->removeFromParent();
            m_durationDragDraw = nullptr;
        }
        auto editorLayer = getEditorLayer();
        auto durationLines = static_cast<alpha::grid::DurationLines*>(editorLayer->m_drawGridLayer->getChildByID("alphalaneous.good_grid/duration-lines"));
        durationLines->removePropertiesForObject("duration-drag"_spr);
    }
    return true;
}

void DDSetGroupIDLayer::onClose(cocos2d::CCObject* sender) {
    SetGroupIDLayer::onClose(sender);
    DurationDrag::get()->updateObjects();
}

void DurationDrag::onEditor() {
    auto editorLayer = getEditorLayer();

    m_durationControl = DurationControl::create();
	m_durationControl->setID("duration-control"_spr);
	editorLayer->m_objectLayer->addChild(m_durationControl);

    auto durationLines = static_cast<alpha::grid::DurationLines*>(editorLayer->m_drawGridLayer->getChildByID("alphalaneous.good_grid/duration-lines"));
    durationLines->setPropertiesForObject("duration-drag"_spr, [] (alpha::grid::Color& color, EffectGameObject* object, float& lineWidth) {
        if (!DurationDrag::isEnabled()) return;
        if (object->getPositionX() < 0.f && !object->m_isSpawnTriggered || object->m_objectID == tinker::constants::objects::PulseTrigger) {
            color = {0, 0, 0, 0};
        }
    });

    m_durationDragDraw = tinker::ui::DurationDragDraw::create();
    m_durationDragDraw->setID("duration-drag"_spr);
    m_durationDragDraw->setZOrder(durationLines->getZOrder() - 1);
    editorLayer->m_drawGridLayer->addChild(m_durationDragDraw);
}

void DurationDrag::updateObjects() {
	if (m_durationControl) {
		m_durationControl->updateObjects(getEditor());
	}
}

void DDEditorPauseLayer::onResume(CCObject* sender) {
    EditorPauseLayer::onResume(sender);
    DurationDrag::get()->updateObjects();
}

void DDEditorUI::deselectAll() {
	EditorUI::deselectAll();
	DurationDrag::get()->updateObjects();
};

void DDEditorUI::deselectObject(GameObject* object) {
	EditorUI::deselectObject(object);
	DurationDrag::get()->updateObjects();
}

void DDEditorUI::selectObject(GameObject* object, bool ignoreFilter) {
	EditorUI::selectObject(object, ignoreFilter);
	DurationDrag::get()->updateObjects();
}

void DDEditorUI::selectObjects(CCArray* objects, bool ignoreFilter) {
	EditorUI::selectObjects(objects, ignoreFilter);
	DurationDrag::get()->updateObjects();
}

void DDLevelEditorLayer::removeObject(GameObject* object, bool noUndo) {
    LevelEditorLayer::removeObject(object, noUndo);
	DurationDrag::get()->updateObjects();
}

namespace tinker::ui {

DurationDragDraw* DurationDragDraw::create() {
    auto ret = new DurationDragDraw();
    ret->autorelease();
    return ret;
}

void DurationDragDraw::drawDottedLine(const cocos2d::CCPoint& start, const cocos2d::CCPoint& end, const alpha::grid::Color& color, const cocos2d::CCRect& visibleRect) {
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    float totalDist = std::sqrt(dx * dx + dy * dy);

    if (totalDist == 0.f) return;

    float dirX = dx / totalDist;
    float dirY = dy / totalDist;

    constexpr float dashLength = 10.f;
    constexpr float lineWidth = 2.f;
    constexpr float gapLength = 5.f;

    float traveled = 0.f;
    while (traveled < totalDist) {
        float segmentLength = std::min(dashLength, totalDist - traveled);

        auto segStart = CCPoint{start.x + dirX * traveled, start.y + dirY * traveled};
        auto segEnd = CCPoint{start.x + dirX * (traveled + segmentLength), start.y + dirY * (traveled + segmentLength)};

        if (segStart.x > visibleRect.getMaxX() || segStart.y > visibleRect.getMaxY()) break;

        traveled += (dashLength + gapLength);

        if (segEnd.x < visibleRect.getMinX() || segEnd.y < visibleRect.getMinY()) {
            continue;
        }

        drawLine(segStart, segEnd, color, lineWidth);
    }
}

void DurationDragDraw::draw(const cocos2d::CCRect& visibleRect) {
    auto dgl = getDrawGridLayer();

    if (!dgl->m_editorLayer->m_showDurationLines || dgl->m_editorLayer->m_playbackMode == PlaybackMode::Playing) return;

    const alpha::grid::Color color = {100, 100, 100, 75};

    auto drawPulseLine = [&] (EffectGameObject* object, float x) {
        auto start = CCPoint{x, object->getPositionY()};
        auto end = CCPoint{object->m_endPosition.x, object->m_endPosition.y};

        float total = object->m_fadeInDuration + object->m_holdDuration + object->m_fadeOutDuration;
        if (total <= 0.f) return;

        float fadeInPct = object->m_fadeInDuration / total;
        float holdPct = (object->m_fadeInDuration + object->m_holdDuration) / total;

        auto p1 = CCPoint{std::lerp(start.x, end.x, fadeInPct), std::lerp(start.y, end.y, fadeInPct)};
        auto p2 = CCPoint{std::lerp(start.x, end.x, holdPct), std::lerp(start.y, end.y, holdPct)};

        auto startColor = alpha::grid::Color{{0, 0, 0, 0}, color.getColorA()};
        auto endColor = alpha::grid::Color{color.getColorA(), {0, 0, 0, 0}};

        drawLine(start, p1, startColor, 2.f);
        drawLine(p1, p2, color, 2.f);
        drawLine(p2, end, endColor, 2.f);
    };

    if (dgl->m_editorLayer->m_editorUI->m_selectedObjects->count() <= 100) {
        auto center = tinker::utils::objects::getCenter();
        if (center) {

            bool first = true;
            int refChannel = 0;
            bool drawCenter = true;

            for (auto object : CCArrayExt<EffectGameObject*>(dgl->m_editorLayer->m_editorUI->m_selectedObjects)) {
                if (!object->m_dontIgnoreDuration || object->m_objectID == tinker::constants::objects::SFXTrigger) continue;
                if (first) {
                    refChannel = object->m_channelValue;
                    first = false;
                } else if (object->m_channelValue != refChannel) {
                    drawCenter = false;
                    break;
                }
            }

            if (drawCenter) {
                for (auto object : CCArrayExt<EffectGameObject*>(dgl->m_editorLayer->m_editorUI->m_selectedObjects)) {
                    if (!object->m_dontIgnoreDuration || object->m_objectID == tinker::constants::objects::SFXTrigger) continue;

                    auto centerPoint = center.unwrap().second;
                    auto start = object->getPosition();
                    auto end = object->m_endPosition;

                    if (end == CCPoint{0.f, 0.f}) end = start;

                    bool isLesser = end.x < start.x;

                    if (!object->m_isSpawnTriggered && !isLesser) {
                        start.x = std::max(start.x, 0.f);
                        end.x = std::max(end.x, 0.f);
                    }

                    drawDottedLine(end, centerPoint, {145, 170, 255, 180}, visibleRect);
                }
            }
        }
    }

    for (auto object : CCArrayExt<EffectGameObject*>(dgl->m_editorLayer->m_durationObjects)) {
        if (!alpha::grid::isObjectVisible(object)) continue;

        if (object->m_endPosition.x < 0.f && !object->m_isSpawnTriggered) {
            object->m_endPosition = CCPoint{0.f, 0.f};
        }

        if (object->m_endPosition == CCPoint{0.f, 0.f} || LevelEditorLayer::get()->m_drawGridLayer->m_updateTimeMarkers) {
            const CCPoint newPos = tinker::utils::objects::getEndPos(object);
            object->m_endPosition = newPos;
            object->runAction(CallFuncExt::create([newPos, object] {
                object->m_endPosition = newPos;
            }));
        }
        
        if (object->getPositionX() < 0.f && !object->m_isSpawnTriggered) {

            drawDottedLine(object->getPosition(), {0.f, object->getPositionY()}, color, visibleRect);

            if (object->m_objectID == tinker::constants::objects::PulseTrigger) {
                drawPulseLine(object, 0.f);
                continue;
            }

            drawLine({0.f, object->getPositionY()}, {object->m_endPosition.x, object->m_endPosition.y}, color, 2.f);
        }

        if (object->m_objectID == tinker::constants::objects::PulseTrigger) {
            drawPulseLine(object, object->getPositionX());
        }
    }
}

}