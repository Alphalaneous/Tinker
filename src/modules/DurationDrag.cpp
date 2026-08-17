#include "modules/DurationDrag.hpp"
#include "utils/Utils.hpp"
#include "utils/Constants.hpp"

using namespace tinker::ui;

bool DurationDrag::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        m_durationControl->removeFromParent();
        m_durationControl = nullptr;
    }
    return true;
}

void DDSetGroupIDLayer::onClose(cocos2d::CCObject* sender) {
    SetGroupIDLayer::onClose(sender);
    DurationDrag::get()->updateObjects();
}

void DurationDrag::onEditor() {
    m_durationControl = DurationControl::create();
	m_durationControl->setID("duration-control"_spr);
	getEditorLayer()->m_objectLayer->addChild(m_durationControl);
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

void DurationDragDraw::drawDottedLine(const CCPoint& start, const CCPoint& end, const LineColor& color, float minX, float maxX, float minY, float maxY) {
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

        DrawGridAPI::get().drawLine(segStart, segEnd, color, lineWidth);
    }
}


void DurationDragDraw::draw(DrawGridLayer* dgl, float minX, float maxX, float minY, float maxY) {
    if (!dgl->m_editorLayer->m_showDurationLines || dgl->m_editorLayer->m_playbackMode == PlaybackMode::Playing) return;

    auto& api = DrawGridAPI::get();
    const LineColor color = {100, 100, 100, 75};

    auto drawPulseLine = [&] (EffectGameObject* object, float x) {
        auto start = ccVertex2F{x, object->getPositionY()};
        auto end = ccVertex2F{object->m_endPosition.x, object->m_endPosition.y};

        const float total = object->m_fadeInDuration + object->m_holdDuration + object->m_fadeOutDuration;
        if (total <= 0.f) return;

        const float fadeInPct = object->m_fadeInDuration / total;
        const float holdPct   = (object->m_fadeInDuration + object->m_holdDuration) / total;

        auto p1 = ccVertex2F{std::lerp(start.x, end.x, fadeInPct), std::lerp(start.y, end.y, fadeInPct)};
        auto p2 = ccVertex2F{std::lerp(start.x, end.x, holdPct), std::lerp(start.y, end.y, holdPct)};

        auto startColor = LineColor{{0, 0, 0, 0}, color.getColorA()};
        auto endColor = LineColor{color.getColorA(), {0, 0, 0, 0}};

        api.drawLine(start, p1, startColor, 2.f);
        api.drawLine(p1, p2, color, 2.f);
        api.drawLine(p2, end, endColor, 2.f);
    };

    if (dgl->m_editorLayer->m_editorUI->m_selectedObjects->count() <= 100) {
        auto center = tinker::utils::getCenter(dgl->m_editorLayer->m_editorUI);
        if (center) {

            bool first = true;
            int refChannel;
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

                    drawDottedLine(end, centerPoint, {145, 170, 255, 180}, minX, maxX, minY, maxY);
                }
            }
        }
    }

    for (auto object : CCArrayExt<EffectGameObject*>(dgl->m_editorLayer->m_durationObjects)) {
        if (!DrawGridAPI::get().isObjectVisible(object)) continue;

        if (object->m_endPosition.x < 0.f && !object->m_isSpawnTriggered) {
            object->m_endPosition = CCPointZero;
        }

        if (object->m_endPosition == CCPointZero || LevelEditorLayer::get()->m_drawGridLayer->m_updateTimeMarkers) {
            const CCPoint newPos = tinker::utils::getEndPos(object);
            object->m_endPosition = newPos;
            object->runAction(CallFuncExt::create([newPos, object] {
                object->m_endPosition = newPos;
            }));
        }
        
        if (object->getPositionX() < 0.f && !object->m_isSpawnTriggered) {

            drawDottedLine(object->getPosition(), {0.f, object->getPositionY()}, color, minX, maxX, minY, maxY);

            if (object->m_objectID == tinker::constants::objects::PulseTrigger) {
                drawPulseLine(object, 0.f);
                continue;
            }

            api.drawLine({0.f, object->getPositionY()}, {object->m_endPosition.x, object->m_endPosition.y}, color, 2.f);
        }

        if (object->m_objectID == tinker::constants::objects::PulseTrigger) {
            drawPulseLine(object, object->getPositionX());
        }
    }
}

$on_mod(Loaded) {
	auto& api = DrawGridAPI::get();
    
	auto& node = api.addDraw<DurationDragDraw>("duration-drag");

    listenForSettingChanges<bool>("DurationDrag-enabled", [&node] (bool val) {
        node.setEnabled(val);
    });

    node.setEnabled(DurationDrag::isEnabled());

	if (auto durationLineRes = api.getNode<DurationLines>()) {
		auto& durationLines = durationLineRes.unwrap();

		durationLines.setPropertiesForObject([] (LineColor& color, EffectGameObject* object, float& lineWidth) {
            if (!DurationDrag::isEnabled()) return;
			if (object->getPositionX() < 0.f && !object->m_isSpawnTriggered || object->m_objectID == tinker::constants::objects::PulseTrigger) {
				color = {0, 0, 0, 0};
			}
		});
	}
}