#include "DurationDrag.hpp"
#include "Utils.hpp"

using namespace tinker::ui;

void DDSetGroupIDLayer::onClose(cocos2d::CCObject* sender) {
    SetGroupIDLayer::onClose(sender);
    DurationDrag::get()->updateObjects();
}

void DurationDrag::onEditor() {
    m_durationControl = DurationControl::create();
	m_durationControl->setID("duration-control"_spr);
	m_editorLayer->m_objectLayer->addChild(m_durationControl);
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

void DDEditorUI::selectObject(GameObject* object, bool p1) {
	EditorUI::selectObject(object, p1);
	DurationDrag::get()->updateObjects();
}

void DDEditorUI::selectObjects(CCArray* objects, bool p1) {
	EditorUI::selectObjects(objects, p1);
	DurationDrag::get()->updateObjects();
}

void DurationDragDraw::drawDottedLine(const CCPoint& start, const CCPoint& end, const LineColor& color, float lineWidth, float dashLength, float gapLength) {
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    float totalDist = std::sqrt(dx * dx + dy * dy);

    if (totalDist == 0.f) return;

    float dirX = dx / totalDist;
    float dirY = dy / totalDist;

    float traveled = 0.f;
    while (traveled < totalDist) {
        float segmentLength = std::min(dashLength, totalDist - traveled);

        ccVertex2F segStart = { start.x + dirX * traveled, start.y + dirY * traveled };
        ccVertex2F segEnd   = { start.x + dirX * (traveled + segmentLength), start.y + dirY * (traveled + segmentLength) };

        DrawGridAPI::get().drawLine(segStart, segEnd, color, lineWidth);

        traveled += dashLength + gapLength;
    }
}


void DurationDragDraw::draw(DrawGridLayer* dgl, float minX, float maxX, float minY, float maxY) {
    if (!dgl->m_editorLayer->m_showDurationLines || dgl->m_editorLayer->m_playbackMode == PlaybackMode::Playing) return;

    auto& api = DrawGridAPI::get();
    const LineColor color = { 255, 255, 255, 115};

    auto drawPulseLine = [&](EffectGameObject* object, float x) {
        const ccVertex2F start = {x, object->getPositionY()};
        const ccVertex2F end = {object->m_endPosition.x, object->m_endPosition.y};

        const float total = object->m_fadeInDuration + object->m_holdDuration + object->m_fadeOutDuration;
        if (total <= 0.0f) return;

        const float fadeInPct = object->m_fadeInDuration / total;
        const float holdPct   = (object->m_fadeInDuration + object->m_holdDuration) / total;

        const ccVertex2F p1 = {std::lerp(start.x, end.x, fadeInPct), std::lerp(start.y, end.y, fadeInPct)};
        const ccVertex2F p2 = {std::lerp(start.x, end.x, holdPct), std::lerp(start.y, end.y, holdPct)};

        const LineColor startColor{{255,255,255,0}, color.getColorA()};
        const LineColor endColor{color.getColorA(), {255,255,255,0}};

        api.drawLine(start, p1, startColor, 2.f);
        api.drawLine(p1, p2, color, 2.f);
        api.drawLine(p2, end, endColor, 2.f);
    };

    auto center = tinker::utils::duration_drag::getCenter(dgl->m_editorLayer->m_editorUI);
    if (center) {

        bool first = true;
        int refChannel;
        bool drawCenter = true;

        for (auto object : CCArrayExt<EffectGameObject*>(dgl->m_editorLayer->m_editorUI->m_selectedObjects)) {
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
            for (auto object : CCArrayExt<EffectGameObject*>(dgl->m_editorLayer->m_editorUI->m_selectedObjects)) {
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

                drawDottedLine(end, centerPoint, {145, 170, 255, 180});
            }
        }
    }

    for (auto object : CCArrayExt<EffectGameObject*>(dgl->m_editorLayer->m_durationObjects)) {
        if (!DrawGridAPI::get().isObjectVisible(object)) continue;

        if (object->m_endPosition.x < 0 && !object->m_isSpawnTriggered) {
            object->m_endPosition = CCPointZero;
        }

        if (object->m_endPosition == CCPointZero || LevelEditorLayer::get()->m_drawGridLayer->m_updateTimeMarkers) {
            const CCPoint newPos = tinker::utils::duration_drag::getEndPos(object);
            object->m_endPosition = newPos;
            queueInMainThread([object = Ref(object), newPos] { object->m_endPosition = newPos; });
        }
        
        if (object->getPositionX() < 0 && !object->m_isSpawnTriggered) {

            drawDottedLine(object->getPosition(), {0, object->getPositionY()}, color);

            if (object->m_objectID == 1006) {
                drawPulseLine(object, 0);
                continue;
            }

            api.drawLine({0, object->getPositionY()}, {object->m_endPosition.x, object->m_endPosition.y}, color, 2.f);
        }

        if (object->m_objectID == 1006) {
            drawPulseLine(object, object->getPositionX());
        }
    }
}

DurationDrag::DurationDrag() {
	auto& api = DrawGridAPI::get();
    auto nodeRes = api.getNode<DurationDragDraw>();
    if (nodeRes) {
        auto node = nodeRes.unwrap();
        node.setEnabled(true);
    }
}

DurationDrag::~DurationDrag() {
	auto& api = DrawGridAPI::get();
    auto nodeRes = api.getNode<DurationDragDraw>();
    if (nodeRes) {
        auto node = nodeRes.unwrap();
        node.setEnabled(false);
    }
}

$on_mod(Loaded) {
	auto& api = DrawGridAPI::get();
    
	api.addDraw<DurationDragDraw>("duration-drag");

	if (auto durationLineRes = api.getNode<DurationLines>()) {
		auto& durationLines = durationLineRes.unwrap();

		durationLines.setPropertiesForObject([] (LineColor& color, EffectGameObject* object, float& lineWidth) {
            if (!DurationDrag::isEnabled()) return;
			if (object->getPositionX() < 0 && !object->m_isSpawnTriggered || object->m_objectID == 1006) {
				color = {0,0,0,0};
			}
		});
	}
}