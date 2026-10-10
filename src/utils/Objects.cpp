#include "utils/Objects.hpp"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/loader/Log.hpp"
#include "Geode/platform/windows.hpp"
#include "Geode/utils/function.hpp"
#include "MainHooks.hpp"
#include "utils/Constants.hpp"
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>

namespace tinker::utils::objects {

bool isColorable(GameObject* object) {
    using namespace tinker::constants::objects;

    static const std::unordered_set<int> allowedIDs = {
        TeleportOrb, 
        ToggleOrb
    };

    static const std::unordered_set<int> disallowedIDs = {
        ForceBlock, 
        ForceCircle, 
        KeyframePoint, 
        CameraGuide, 
        CollisionBlock, 
        BPMTrigger, 
        PlayerTouchToggle, 
        OrangeTeleportPortal
    };

    static const std::unordered_set<GameObjectType> disallowedTypes = {
        GameObjectType::YellowJumpPad,
        GameObjectType::PinkJumpPad,
        GameObjectType::RedJumpPad,
        GameObjectType::GravityPad,
        GameObjectType::SpiderPad,
        GameObjectType::SpiderOrb,
        GameObjectType::YellowJumpRing,
        GameObjectType::PinkJumpRing,
        GameObjectType::RedJumpRing,
        GameObjectType::GravityRing,
        GameObjectType::GreenRing,
        GameObjectType::DropRing,
        GameObjectType::DashRing,
        GameObjectType::GravityDashRing,
        GameObjectType::NormalGravityPortal,
        GameObjectType::InverseGravityPortal,
        GameObjectType::GravityTogglePortal,
        GameObjectType::CubePortal,
        GameObjectType::ShipPortal,
        GameObjectType::UfoPortal,
        GameObjectType::BallPortal,
        GameObjectType::WavePortal,
        GameObjectType::RobotPortal,
        GameObjectType::SpiderPortal,
        GameObjectType::SwingPortal,
        GameObjectType::NormalMirrorPortal,
        GameObjectType::InverseMirrorPortal,
        GameObjectType::MiniSizePortal,
        GameObjectType::RegularSizePortal,
        GameObjectType::DualPortal,
        GameObjectType::SoloPortal,
        GameObjectType::TeleportPortal,
        GameObjectType::SecretCoin,
        GameObjectType::UserCoin
    };

    if (allowedIDs.contains(object->m_objectID)) return true;
    if (disallowedIDs.contains(object->m_objectID)) return false;
    if (object->isTrigger() || object->isSpeedObject()) return false;
    if (disallowedTypes.contains(object->m_objectType)) return false;

    return true;
}

ColorData getActiveColor(int colorID) {
    using namespace tinker::constants::color_channels;

    auto editorLayer = LevelEditorLayer::get();

    for (auto action : editorLayer->m_effectManager->m_colorActionSpriteVector) {
        if (!action) continue;
        if (action->m_colorID != colorID || action->m_colorID <= 0) continue;

        cocos2d::ccColor3B color = action->m_color;

        if (colorID == PlayerColor1) color = GameManager::get()->colorForIdx(GameManager::get()->m_playerColor.value());
        if (colorID == PlayerColor2) color = GameManager::get()->colorForIdx(GameManager::get()->m_playerColor2.value());

        if (colorID == Black) color = {0, 0, 0};
        if (colorID == White) color = {255, 255, 255};

        for (auto& pulse : editorLayer->m_effectManager->m_pulseEffectVector) {
            if (pulse.m_targetGroupID == action->m_colorID) {
                color = editorLayer->m_effectManager->colorForPulseEffect(color, &pulse);
            }
        }

        bool blending = false;
        GLubyte opacity = 255;
        if (auto colorAction = action->m_colorAction) {
            blending = colorAction->m_blending;
            opacity = colorAction->m_currentOpacity * 255;
        }

        return {color, blending, opacity, action->m_colorAction};
    }
    return {{255, 255, 255}, false, 255};
}

void setGameObjectColor(GameObject* obj, std::optional<cocos2d::ccColor3B> baseColor, bool baseBlend, std::optional<cocos2d::ccColor3B> detailColor, bool detailBlend) {
    using namespace tinker::constants;
    using namespace tinker::constants::objects;
    
    if (!isColorable(obj)) return;

    auto anim = geode::cast::typeinfo_cast<AnimatedGameObject*>(obj);
    auto enhanced = geode::cast::typeinfo_cast<EnhancedGameObject*>(obj);

    auto base = obj->m_baseColor;
    auto detail = obj->m_detailColor;

    if (base && baseColor) {
        auto color = baseColor.value();

        obj->updateHSVState();

        auto blend = baseBlend 
            ? cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE} 
            : cocos2d::ccBlendFunc{GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

        if (anim) {
            auto paSpr = anim->m_animatedSprite ? anim->m_animatedSprite->m_paSprite : nullptr;
            if (paSpr) {
                for (auto child : paSpr->getChildrenExt()) {
                    if (child == anim->m_eyeSpritePart && !anim->m_childSprite) continue;

                    auto spr = geode::cast::typeinfo_cast<cocos2d::CCSprite*>(child);
                    if (!spr) continue;

                    spr->setBlendFunc(blend);
                }
            }
        }
        else if (enhanced || obj->m_hasCustomChild) {
            for (auto child : obj->getChildrenExt()) {
                if (child == obj->m_colorSprite) continue;

                auto spr = geode::cast::typeinfo_cast<cocos2d::CCSprite*>(child);
                if (!spr) continue;

                spr->setBlendFunc(blend);
            }
        }

        if (obj->m_objectID == SpikedSquareHazard 
         || obj->m_objectID == SpikedCircleHazard 
         || obj->m_objectID == TriangleHazard) {
            for (auto child : obj->getChildrenExt()) {
                if (child->getChildrenCount() != 0) continue;

                auto spr = geode::cast::typeinfo_cast<cocos2d::CCSprite*>(child);
                if (!spr) continue;
                
                spr->setBlendFunc(blend);
            }
        }

        obj->setBlendFunc(blend);

        if (base->m_usesHSV) {
            color = GameToolbox::transformColor(color, base->m_hsv);
        }

        obj->updateMainColor(color);
    }
    if (detail && detailColor) {
        auto color = detailColor.value();

        obj->updateHSVState();

        auto blend = detailBlend 
            ? cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE} 
            : cocos2d::ccBlendFunc{GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

        geode::Function<void(cocos2d::CCNode* node)> applyBlend = [&] (cocos2d::CCNode* node) {
            for (auto child : node->getChildrenExt()) {
                auto spr = geode::cast::typeinfo_cast<cocos2d::CCSprite*>(child);
                if (!spr) continue;

                spr->setBlendFunc(blend);
                applyBlend(spr);
            }
        };
        
        if (anim) {
            if (anim->m_childSprite) {
                anim->m_childSprite->setBlendFunc(blend);
            }
            else if (anim->m_eyeSpritePart) {
                anim->m_eyeSpritePart->setBlendFunc(blend);
            }
        }
        else if (enhanced) {
            for (auto child : obj->getChildrenExt()) {
                applyBlend(child);
            }
        }
        else {
            applyBlend(obj);
        }

        if (obj->m_colorSprite) {
            obj->m_colorSprite->setBlendFunc(blend);
        }

        if (detail->m_usesHSV) {
            color = GameToolbox::transformColor(color, detail->m_hsv);
        }
        obj->updateSecondaryColor(color);
    }

    obj->setOpacity(255);
}

void updateGameObjectColor(GameObject* obj) {
    using namespace tinker::constants;
    
    if (!isColorable(obj)) return;

    std::optional<cocos2d::ccColor3B> baseColor;
    std::optional<cocos2d::ccColor3B> detailColor;

    bool baseBlend;
    bool detailBlend;

    if (obj->m_baseColor) {
        auto colorData = getActiveColor(obj->m_baseColor->m_colorID);

        if (obj->m_baseColor->m_colorID == color_channels::Black) {
            baseColor = cocos2d::ccColor3B{0, 0, 0};
        }
        else {
            baseColor = colorData.color;
        }
        baseBlend = colorData.blending;
    }
    
    if (obj->m_detailColor) {
        auto colorData = getActiveColor(obj->m_detailColor->m_colorID);

        auto color = colorData.color;

        if (obj->m_detailColor->m_colorID == color_channels::Black) {
            detailColor = cocos2d::ccColor3B{0, 0, 0};
        }
        else {
            detailColor = colorData.color;
        }
        detailBlend = colorData.blending;
    }

    setGameObjectColor(obj, baseColor, baseBlend, detailColor, detailBlend);
}

void updateCreateButtonColor(cocos2d::CCNode* btn, int color1ID, int color2ID, const cocos2d::ccHSVValue& hsv1, const cocos2d::ccHSVValue& hsv2) {
    using namespace tinker::constants;
    
    auto btnSprite = btn->getChildByType<ButtonSprite>(0);
    if (!btnSprite) return;

    auto obj = btnSprite->getChildByType<GameObject>();
    if (!obj) return;

    if (!isColorable(obj)) return;

    std::optional<cocos2d::ccColor3B> baseColor;
    std::optional<cocos2d::ccColor3B> detailColor;

    bool baseBlend = false;
    bool detailBlend = false;

    if (obj->m_baseColor) {
        if (color1ID == color_channels::Default) {
            obj->m_baseColor->m_colorID = obj->m_baseColor->m_defaultColorID;
            obj->m_baseColor->m_hsv = cocos2d::ccHSVValue{0.f, 1.f, 1.f, false, false};
            obj->m_baseColor->m_opacity = 1.f;

            if (obj->m_baseColor->m_colorID == color_channels::Black) {
                baseColor = cocos2d::ccColor3B{0, 0, 0};
            }
            else {
                baseColor = cocos2d::ccColor3B{255, 255, 255};
            }
        }
        else {
            auto colorData = getActiveColor(color1ID);

            obj->m_baseColor->m_colorID = color1ID;
            obj->m_baseColor->m_hsv = hsv1;
            obj->m_baseColor->m_opacity = colorData.opacity / 255.f;
            baseColor = colorData.color;
            baseBlend = colorData.blending;
        }
    }
    if (obj->m_detailColor) {
        if (color2ID == color_channels::Default) {

            obj->m_detailColor->m_colorID = obj->m_detailColor->m_defaultColorID;
            obj->m_detailColor->m_hsv = cocos2d::ccHSVValue{0.f, 1.f, 1.f, false, false};
            obj->m_detailColor->m_opacity = 1.f;

            if (obj->m_detailColor->m_colorID == color_channels::Black) {
                detailColor = cocos2d::ccColor3B{0, 0, 0};
            }
            else {
                detailColor = cocos2d::ccColor3B{200, 200, 255};
            }
        }
        else {
            auto colorData = getActiveColor(color2ID);

            obj->m_detailColor->m_colorID = color2ID;
            obj->m_detailColor->m_hsv = hsv2;
            obj->m_detailColor->m_opacity = colorData.opacity / 255.f;
            detailColor = colorData.color;
            detailBlend = colorData.blending;
        }
    }

    setGameObjectColor(obj, baseColor, baseBlend, detailColor, detailBlend);
}

void forEachObject(geode::Function<void(GameObject*)> callback) {
    auto gjbgl = GJBaseGameLayer::get();

    int count = gjbgl->m_sections.empty() ? -1 : gjbgl->m_sections.size();
    for (int i = gjbgl->m_leftSectionIndex; i <= gjbgl->m_rightSectionIndex && i < count; ++i) {
        auto leftSection = gjbgl->m_sections[i];
        if (!leftSection) continue;

        auto leftSectionSize = leftSection->size();
        for (int j = gjbgl->m_bottomSectionIndex; j <= gjbgl->m_topSectionIndex && j < leftSectionSize; ++j) {
            auto section = leftSection->at(j);
            if (!section) continue;

            auto sectionSize = gjbgl->m_sectionSizes[i]->at(j);
            for (int k = 0; k < sectionSize; ++k) {
                auto obj = section->at(k);
                if (!obj) continue;

                callback(obj);
            }
        }
    }
}

cocos2d::ccColor3B getRealizedColor(int channelID, unsigned int depth) {
    auto channel = LevelEditorLayer::get()->m_levelSettings->m_effectManager->getColorAction(channelID);
    if (!channel) {
        return cocos2d::ccWHITE;
    }
    if (channel->m_copyColorLoop || depth > 10) {
        return cocos2d::ccGRAY;
    }
    if (channel->m_copyID) {
        return GameToolbox::transformColor(getRealizedColor(channel->m_copyID, depth + 1), channel->m_copyHSV);
    }
    return channel->m_fromColor;
}

cocos2d::CCPoint getEndPos(EffectGameObject* object) {
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

    cocos2d::CCPoint currentPos = object->getPosition();
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

    cocos2d::CCPoint newPos = LevelTools::posForTimeInternal(
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
        return wasRotated ? cocos2d::CCPoint{currentPos.x, newPos.y} : cocos2d::CCPoint{newPos.x, currentPos.y};
    }

    return newPos;
}

EffectGameObject* getFurthestEndObject(const std::vector<EffectGameObject*>& objects, const cocos2d::CCPoint& unitRefDir) {
    float maxProj = -FLT_MAX;
    EffectGameObject* furthest = objects[0];

    auto refStart = furthest->getPosition();
    auto refEnd = furthest->m_endPosition;

    if (refEnd == cocos2d::CCPoint{0.f, 0.f}) refEnd = refStart;

    bool refIsLesser = refEnd.x < refStart.x;

    if (!furthest->m_isSpawnTriggered && !refIsLesser) {
        refStart.x = std::max(refStart.x, 0.f);
        refEnd.x = std::max(refEnd.x, 0.f);
    }

    for (auto obj : objects) {
        auto start = obj->getPosition();
        auto end = obj->m_endPosition;

        if (end == cocos2d::CCPoint{0.f, 0.f}) end = start;

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

geode::Result<std::pair<cocos2d::CCPoint, cocos2d::CCPoint>> getCenter() {
    using namespace tinker::constants::objects;

    auto editorUI = MainEditorUI::get();

    std::vector<EffectGameObject*> objects;
    for (auto obj : editorUI->m_selectedObjects->asExt<GameObject*>()) {
        if (obj->m_dontIgnoreDuration && obj->m_objectID != SFXTrigger) {
            objects.push_back(static_cast<EffectGameObject*>(obj));
        }
    }

    if (objects.size() < 2) return geode::Err("Need at least two EffectGameObjects");

    auto refStart = objects[0]->getPosition();
    auto refEnd = objects[0]->m_endPosition;

    if (!objects[0]->m_isSpawnTriggered) {
        refStart.x = std::max(refStart.x, 0.f);
        refEnd.x = std::max(refEnd.x, 0.f);
    }

    if (refEnd == cocos2d::CCPoint{0.f, 0.f}) refEnd = refStart; 

    bool refNoDuration = objects[0]->m_duration == 0 || (objects[0]->m_objectID == PulseTrigger && objects[0]->m_fadeInDuration + objects[0]->m_holdDuration + objects[0]->m_fadeOutDuration == 0);

    auto refDir = refEnd - refStart;

    if (refDir.x <= 0.f) refDir.x = 0.00001f;

    float refLen = std::sqrt(refDir.x * refDir.x + refDir.y * refDir.y);
    if (refLen == 0.f) refLen = 0.00001f;

    auto unitRefDir = refDir / refLen;
    auto ortho = cocos2d::CCPoint{-unitRefDir.y, unitRefDir.x};

    float minProj = FLT_MAX;
    float maxProj = -FLT_MAX;
    for (auto obj : objects) {
        auto start = obj->getPosition();
        auto end = obj->m_endPosition;

        float proj = start.x * ortho.x + start.y * ortho.y;
        minProj = std::min(minProj, proj);
        maxProj = std::max(maxProj, proj);
    }

    float centerProj = (minProj + maxProj) / 2.f;
    float refProj = refStart.x * ortho.x + refStart.y * ortho.y;

    auto furthestObj = getFurthestEndObject(objects, unitRefDir);

    auto furthestStart = furthestObj->getPosition();
    if (furthestStart.x <= 0 && !furthestObj->m_isSpawnTriggered) furthestStart.x = 0.00001;

    auto furthestEnd = furthestObj->m_endPosition;

    if (furthestEnd == cocos2d::CCPoint{0.f, 0.f}) furthestEnd = furthestStart;

    bool isLesser = furthestEnd.x < furthestStart.x;
    int multiplier = isLesser ? -1 : 1;

    if (!furthestObj->m_isSpawnTriggered && !isLesser) {
        furthestStart.x = std::max(furthestStart.x, 0.f);
        furthestEnd.x = std::max(furthestEnd.x, 0.f);
    }

    float startAlong = (furthestStart.x - refStart.x) * unitRefDir.x + (furthestStart.y - refStart.y) * unitRefDir.y;

    float offset = 60.f * multiplier;

    float startAlongWith60 = startAlong + offset;

    auto startCenter = cocos2d::CCPoint{
        refStart.x + unitRefDir.x * startAlongWith60 + ortho.x * (centerProj - refProj),
        refStart.y + unitRefDir.y * startAlongWith60 + ortho.y * (centerProj - refProj)
    };

    float maxAlongDir = (furthestEnd.x - refStart.x) * unitRefDir.x + (furthestEnd.y - refStart.y) * unitRefDir.y;

    auto endCenter = cocos2d::CCPoint{
        refStart.x + unitRefDir.x * maxAlongDir + ortho.x * (centerProj - refProj),
        refStart.y + unitRefDir.y * maxAlongDir + ortho.y * (centerProj - refProj)
    };

    auto past = cocos2d::CCPoint{
        endCenter.x + unitRefDir.x * offset,
        endCenter.y + unitRefDir.y * offset
    };

    return geode::Ok(std::make_pair(startCenter, past));
}

}