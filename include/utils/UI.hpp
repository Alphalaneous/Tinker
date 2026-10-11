#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>

namespace tinker::utils::ui {

struct AxisBounds {
    float min = 0.f;
    float max = 0.f;
};

enum class Axis {
    Vertical,
    Horizontal
};

void resizeNodeToRealBounds(cocos2d::CCNode* node, const cocos2d::CCSize& offset = {0.f, 0.f}, const std::vector<cocos2d::CCNode*>& ignore = {});

cocos2d::CCRect getRealBounds(cocos2d::CCNode* node, const std::vector<cocos2d::CCNode*>& ignore = {});

AxisBounds getAvailableSpace(cocos2d::CCNode* a, cocos2d::CCNode* b, Axis axis, const AxisBounds& offset = {0.f, 0.f}, const std::vector<cocos2d::CCNode*>& ignore = {});

bool nodeFits(cocos2d::CCNode* node, const AxisBounds& bounds, Axis axis);

float getFurthestLeft(cocos2d::CCNode* node, float x);

bool nodesIntersect(cocos2d::CCNode* a, cocos2d::CCNode* b);

float getToolbarHeight(bool checkVisible = true);

cocos2d::CCPoint rotatePointAroundPivot(const cocos2d::CCPoint& point, const cocos2d::CCPoint& pivot, float angleDegrees);

}