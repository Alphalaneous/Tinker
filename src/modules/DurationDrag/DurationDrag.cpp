#include "DurationDrag.hpp"
#include "Utils.hpp"
#include <alphalaneous.good_grid/include/DrawLayers/DurationLines.hpp>

using namespace tinker::ui;

void DDSetGroupIDLayer::onClose(cocos2d::CCObject* sender) {
    SetGroupIDLayer::onClose(sender);
    DurationDrag::get()->updateObjects();
}

void DurationDrag::onEditor() {
    m_durationControl = DurationControl::create();
	m_durationControl->setID("duration-control"_spr);
	m_editorLayer->m_objectLayer->addChild(m_durationControl);

    auto dgl = m_editorLayer->m_drawGridLayer;

    auto durationLines = dgl->getChildByType<good_grid::DurationLines>(0);
    durationLines->setPropertiesForObject([] (good_grid::GradientColor& color, EffectGameObject* object, float& lineWidth) {
        if (object->getPositionX() < 0 && !object->m_isSpawnTriggered || object->m_objectID == 1006) {
            color = {0,0,0,0};
        }
    });

    auto ddd = DurationDragDraw::create();
    ddd->setZOrder(60);
    ddd->setID("duration-drag"_spr);
    dgl->addChild(ddd);
}

void DurationDrag::updateObjects() {
	if (m_durationControl) {
		m_durationControl->updateObjects(m_editorUI);
	}
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

DurationDragDraw* DurationDragDraw::create() {
    auto ret = new DurationDragDraw();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void DurationDragDraw::drawDottedLine(const CCPoint& start, const CCPoint& end, const good_grid::GradientColor& color, float minX, float maxX, float minY, float maxY) {
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

        ccVertex2F segStart = { start.x + dirX * traveled, start.y + dirY * traveled };
        ccVertex2F segEnd   = { start.x + dirX * (traveled + segmentLength), start.y + dirY * (traveled + segmentLength) };

        if (segStart.x > maxX || segStart.y > maxY) break;

        traveled += (dashLength + gapLength);

        if (segEnd.x < minX || segEnd.y < minY) {
            continue;
        }

        drawLine(segStart, segEnd, color, lineWidth);
    }
}

void DurationDragDraw::draw(float minX, float maxX, float minY, float maxY) {
    if (!getDrawGridLayer()->m_editorLayer->m_showDurationLines || getDrawGridLayer()->m_editorLayer->m_playbackMode == PlaybackMode::Playing) return;

    const good_grid::GradientColor color = { 255, 255, 255, 115};

    auto drawPulseLine = [&](EffectGameObject* object, float x) {
        const ccVertex2F start = {x, object->getPositionY()};
        const ccVertex2F end = {object->m_endPosition.x, object->m_endPosition.y};

        const float total = object->m_fadeInDuration + object->m_holdDuration + object->m_fadeOutDuration;
        if (total <= 0.0f) return;

        const float fadeInPct = object->m_fadeInDuration / total;
        const float holdPct   = (object->m_fadeInDuration + object->m_holdDuration) / total;

        const ccVertex2F p1 = {std::lerp(start.x, end.x, fadeInPct), std::lerp(start.y, end.y, fadeInPct)};
        const ccVertex2F p2 = {std::lerp(start.x, end.x, holdPct), std::lerp(start.y, end.y, holdPct)};

        const good_grid::GradientColor startColor{{255,255,255,0}, color.getColorA()};
        const good_grid::GradientColor endColor{color.getColorA(), {255,255,255,0}};

        drawLine(start, p1, startColor, 2.f);
        drawLine(p1, p2, color, 2.f);
        drawLine(p2, end, endColor, 2.f);
    };

    if (getDrawGridLayer()->m_editorLayer->m_editorUI->m_selectedObjects->count() <= 100) {
        auto center = tinker::utils::duration_drag::getCenter(getDrawGridLayer()->m_editorLayer->m_editorUI);
        if (center) {

            bool first = true;
            int refChannel;
            bool drawCenter = true;

            for (auto object : CCArrayExt<EffectGameObject*>(getDrawGridLayer()->m_editorLayer->m_editorUI->m_selectedObjects)) {
                if (!object->m_dontIgnoreDuration || object->m_objectID == 3602) continue;
                if (first) {
                    refChannel = object->m_channelValue;
                    first = false;
                } else if (object->m_channelValue != refChannel) {
                    drawCenter = false;
                    break;
                }
            }

            if (drawCenter) {
                for (auto object : CCArrayExt<EffectGameObject*>(getDrawGridLayer()->m_editorLayer->m_editorUI->m_selectedObjects)) {
                    if (!object->m_dontIgnoreDuration || object->m_objectID == 3602) continue;

                    auto centerPoint = center.unwrap().second;
                    auto start = object->getPosition();
                    auto end = object->m_endPosition;

                    if (end == CCPointZero) end = start;

                    bool isLesser = end.x < start.x;

                    if (!object->m_isSpawnTriggered && !isLesser) {
                        start.x = std::max(start.x, 0.f);
                        end.x = std::max(end.x, 0.f);
                    }

                    drawDottedLine(end, centerPoint, {145, 170, 255, 180}, minX, maxX, minY, maxY);
                }
            }
        }
    }

    for (auto object : CCArrayExt<EffectGameObject*>(getDrawGridLayer()->m_editorLayer->m_durationObjects)) {
        if (!isObjectVisible(object)) continue;

        if (object->m_endPosition.x < 0 && !object->m_isSpawnTriggered) {
            object->m_endPosition = CCPointZero;
        }

        if (object->m_endPosition == CCPointZero || LevelEditorLayer::get()->m_drawGridLayer->m_updateTimeMarkers) {
            const CCPoint newPos = tinker::utils::duration_drag::getEndPos(object);
            object->m_endPosition = newPos;
            object->runAction(CallFuncExt::create([newPos, object] {
                object->m_endPosition = newPos;
            }));
        }
        
        if (object->getPositionX() < 0 && !object->m_isSpawnTriggered) {

            drawDottedLine(object->getPosition(), {0, object->getPositionY()}, color, minX, maxX, minY, maxY);

            if (object->m_objectID == 1006) {
                drawPulseLine(object, 0);
                continue;
            }

            drawLine({0, object->getPositionY()}, {object->m_endPosition.x, object->m_endPosition.y}, color, 2.f);
        }

        if (object->m_objectID == 1006) {
            drawPulseLine(object, object->getPositionX());
        }
    }
}