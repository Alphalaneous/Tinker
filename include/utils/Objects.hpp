#pragma once

#include "Geode/cocos/cocoa/CCGeometry.h"
#include <Geode/binding/GameObject.hpp>
#include <Geode/cocos/base_nodes/CCNode.h>

class ColorAction;

namespace tinker::utils::objects {

struct ColorData {
    cocos2d::ccColor3B color{};
    bool blending = false;
    GLubyte opacity = 0;
    ColorAction* action = nullptr;
};

bool isColorable(GameObject* object);

ColorData getActiveColor(int colorID);

void updateGameObjectColor(GameObject* obj);

void setGameObjectColor(GameObject* obj, std::optional<cocos2d::ccColor3B> baseColor, bool baseBlend, std::optional<cocos2d::ccColor3B> detailColor, bool detailBlend);

void updateCreateButtonColor(cocos2d::CCNode* btn, int color1ID, int color2ID, const cocos2d::ccHSVValue& hsv1, const cocos2d::ccHSVValue& hsv2);

void forEachObject(geode::Function<void(GameObject*)> callback);

cocos2d::ccColor3B getRealizedColor(int channelID, unsigned int depth = 0);

cocos2d::CCPoint getEndPos(EffectGameObject* object);

EffectGameObject* getFurthestEndObject(const std::vector<EffectGameObject*>& objects, const cocos2d::CCPoint& unitRefDir);

geode::Result<std::pair<cocos2d::CCPoint, cocos2d::CCPoint>> getCenter();

cocos2d::CCPoint getCameraCenter();

void moveCameraToPos(cocos2d::CCPoint position, bool showCircle = true);

void moveCameraToObject(GameObject* object, bool showCircle = true);

void moveCameraToObjects(cocos2d::CCArray* objects, bool showCircle = true);

}