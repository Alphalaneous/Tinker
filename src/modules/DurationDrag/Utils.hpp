#include <Geode/Geode.hpp>
#include "utils/Constants.hpp"

using namespace geode::prelude;

namespace tinker::utils::duration_drag {

    static inline CCPoint getEndPos(EffectGameObject* object) {
        using namespace tinker::constants::objects;

        auto dgl = LevelEditorLayer::get()->m_drawGridLayer;
        const auto* settings = dgl->m_editorLayer->m_levelSettings;
        const int startSpeed = static_cast<int>(settings->m_startSpeed);

        float duration;

        if (object->m_objectID == PulseTrigger) {
            duration = object->m_fadeInDuration + object->m_holdDuration + object->m_fadeOutDuration;
        }
        else if (object->m_objectID == SFXTrigger) {
            SFXTriggerGameObject* sfxTrigger = static_cast<SFXTriggerGameObject*>(object);
            duration = sfxTrigger->m_soundDuration;
        }
        else {
            duration = object->m_duration;
        }

        CCPoint currentPos = object->getPosition();
        if (currentPos.x < 0.f && !object->m_isSpawnTriggered)
            currentPos.x = 0.00001f;

        if (duration <= 0.f) {
            return currentPos;
        }

        if (object->m_isSpawnTriggered) {
            return {
                currentPos.x + duration * 311.5801f,
                currentPos.y
            };
        }

        const float currentTime = LevelTools::timeForPos(
            currentPos,
            dgl->m_speedObjects,
            startSpeed,
            object->m_ordValue,
            object->m_channelValue,
            false,
            dgl->m_editorLayer->m_isPlatformer,
            true,
            false,
            false
        );

        const bool wasRotated = LevelTools::getLastGameplayRotated();

        CCPoint newPos = LevelTools::posForTimeInternal(
            currentTime + duration,
            dgl->m_speedObjects,
            startSpeed,
            dgl->m_editorLayer->m_isPlatformer,
            false,
            true,
            dgl->m_editorLayer->m_gameState.m_rotateChannel,
            false
        );

        const bool nowRotated = LevelTools::getLastGameplayRotated();

        if (wasRotated == nowRotated) {
            return wasRotated ? CCPoint{currentPos.x, newPos.y} : CCPoint{newPos.x, currentPos.y};
        }

        return newPos;
    }


    static inline EffectGameObject* getFurthestEndObject(const std::vector<EffectGameObject*>& objects, const CCPoint& unitRefDir) {

        float maxProj = -FLT_MAX;
        EffectGameObject* furthest = objects[0];

        auto refStart = furthest->getPosition();
        auto refEnd = furthest->m_endPosition;

        if (refEnd == CCPointZero) refEnd = refStart;

        bool refIsLesser = refEnd.x < refStart.x;

        if (!furthest->m_isSpawnTriggered && !refIsLesser) {
            refStart.x = std::max(refStart.x, 0.f);
            refEnd.x = std::max(refEnd.x, 0.f);
        }

        for (auto obj : objects) {
            auto start = obj->getPosition();
            auto end = obj->m_endPosition;

            if (end == CCPointZero) end = start;

            bool isLesser = end.x < start.x;

            if (!obj->m_isSpawnTriggered && !isLesser) {
                start.x = std::max(start.x, 0.f);
                end.x = std::max(end.x, 0.f);
            }

            if (isLesser != refIsLesser) continue;

            float proj = (end.x - refStart.x) * unitRefDir.x + (end.y - refStart.y) * unitRefDir.y;

            if (isLesser) {
                if (proj <= maxProj) {
                    maxProj = proj;
                    furthest = obj;
                }
            }
            else {
                if (proj >= maxProj) {
                    maxProj = proj;
                    furthest = obj;
                }
            }
        }

        return furthest;
    }

    static inline geode::Result<std::pair<CCPoint, CCPoint>> getCenter(EditorUI* editorUI) {
        using namespace tinker::constants::objects;

        std::vector<EffectGameObject*> objects;
        for (auto obj : CCArrayExt<GameObject*>(editorUI->m_selectedObjects)) {
            if (obj->m_dontIgnoreDuration && obj->m_objectID != SFXTrigger) {
                objects.push_back(static_cast<EffectGameObject*>(obj));
            }
        }

        if (objects.size() < 2) return geode::Err("Need at least two EffectGameObjects");

        CCPoint refStart = objects[0]->getPosition();
        CCPoint refEnd = objects[0]->m_endPosition;

        if (!objects[0]->m_isSpawnTriggered) {
            refStart.x = std::max(refStart.x, 0.f);
            refEnd.x = std::max(refEnd.x, 0.f);
        }

        if (refEnd == CCPointZero) refEnd = refStart; 

        bool refNoDuration = objects[0]->m_duration == 0 || (objects[0]->m_objectID == PulseTrigger && objects[0]->m_fadeInDuration + objects[0]->m_holdDuration + objects[0]->m_fadeOutDuration == 0);

        CCPoint refDir = refEnd - refStart;

        if (refDir.x <= 0.f) refDir.x = 0.00001f;

        float refLen = std::sqrt(refDir.x * refDir.x + refDir.y * refDir.y);
        if (refLen == 0.f) refLen = 0.00001f;

        CCPoint unitRefDir = { refDir.x / refLen, refDir.y / refLen };
        CCPoint ortho = { -unitRefDir.y, unitRefDir.x };

        float minProj = FLT_MAX;
        float maxProj = -FLT_MAX;
        for (auto obj : objects) {
            CCPoint start = obj->getPosition();
            CCPoint end = obj->m_endPosition;

            float proj = start.x * ortho.x + start.y * ortho.y;
            minProj = std::min(minProj, proj);
            maxProj = std::max(maxProj, proj);
        }

        float centerProj = (minProj + maxProj) / 2.f;
        float refProj = refStart.x * ortho.x + refStart.y * ortho.y;

        auto furthestObj = getFurthestEndObject(objects, unitRefDir);

        CCPoint furthestStart = furthestObj->getPosition();
        if (furthestStart.x <= 0 && !furthestObj->m_isSpawnTriggered) furthestStart.x = 0.00001;

        CCPoint furthestEnd = furthestObj->m_endPosition;

        if (furthestEnd == CCPointZero) furthestEnd = furthestStart;

        bool isLesser = furthestEnd.x < furthestStart.x;
        int multiplier = isLesser ? -1 : 1;

        if (!furthestObj->m_isSpawnTriggered && !isLesser) {
            furthestStart.x = std::max(furthestStart.x, 0.f);
            furthestEnd.x = std::max(furthestEnd.x, 0.f);
        }

        float startAlong = (furthestStart.x - refStart.x) * unitRefDir.x + (furthestStart.y - refStart.y) * unitRefDir.y;

        float offset = 60.f * multiplier;

        float startAlongWith60 = startAlong + offset;

        auto startCenter = CCPoint{
            refStart.x + unitRefDir.x * startAlongWith60 + ortho.x * (centerProj - refProj),
            refStart.y + unitRefDir.y * startAlongWith60 + ortho.y * (centerProj - refProj)
        };

        float maxAlongDir = (furthestEnd.x - refStart.x) * unitRefDir.x + (furthestEnd.y - refStart.y) * unitRefDir.y;

        auto endCenter = CCPoint{
            refStart.x + unitRefDir.x * maxAlongDir + ortho.x * (centerProj - refProj),
            refStart.y + unitRefDir.y * maxAlongDir + ortho.y * (centerProj - refProj)
        };

        auto past = CCPoint{
            endCenter.x + unitRefDir.x * offset,
            endCenter.y + unitRefDir.y * offset
        };

        return geode::Ok(std::make_pair(startCenter, past));
    }

}