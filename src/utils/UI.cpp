#include "utils/UI.hpp"
#include "utils/Constants.hpp"
#include "MainHooks.hpp"
#include "modules/StatusBar.hpp"
#include "modules/UIScaling.hpp"

namespace tinker::utils::ui {

void resizeNodeToRealBounds(cocos2d::CCNode* node, const cocos2d::CCSize& offset, const std::vector<cocos2d::CCNode*>& ignore) {
    auto world = node->getParent()->convertToWorldSpace(node->getPosition());

    std::unordered_map<cocos2d::CCNode*, cocos2d::CCPoint> worldPoints;
    for (auto child : node->getChildrenExt()) {
        worldPoints[child] = node->convertToWorldSpace(child->getPosition());
        auto world = child->convertToWorldSpace(cocos2d::CCPointZero);
    }

    auto newSize = getRealBounds(node, ignore).size;
    node->setContentSize(newSize + offset);
    node->setPosition(node->getParent()->convertToNodeSpace(world));

    for (auto child : node->getChildrenExt()) {
        child->setPosition(node->convertToNodeSpace(worldPoints[child]));
    }
}

cocos2d::CCRect getRealBounds(cocos2d::CCNode* node, const std::vector<cocos2d::CCNode*>& ignore) {
    auto winSize = cocos2d::CCDirector::get()->getWinSize();
    if (!node->isVisible()) return {0, 0, 0, 0};

    float minX = FLT_MAX;
    float minY = FLT_MAX;
    float maxX = -FLT_MAX;
    float maxY = -FLT_MAX;

    bool hasBounds = false;

    auto isIgnored = [&] (cocos2d::CCNode* child) {
        return std::find(ignore.begin(), ignore.end(), child) != ignore.end();
    };

    for (auto child : node->getChildrenExt()) {
        if (isIgnored(child) || !child->isVisible()) continue;

        auto childSize = child->getContentSize();
        bool isFullscreen = childSize.width >= winSize.width && childSize.height >= winSize.height;

        if (!isFullscreen) {
            auto rect = child->boundingBox();

            minX = std::min(minX, rect.getMinX());
            minY = std::min(minY, rect.getMinY());
            maxX = std::max(maxX, rect.getMaxX());
            maxY = std::max(maxY, rect.getMaxY());

            hasBounds = true;
        }

        if (child->getChildrenCount() > 0) {
            auto nested = getRealBounds(child, ignore);

            if (nested.size.width > 0 || nested.size.height > 0) {
                auto corners = std::array<cocos2d::CCPoint, 4> {
                    nested.origin,
                    {nested.getMaxX(), nested.origin.y},
                    {nested.origin.x, nested.getMaxY()},
                    {nested.getMaxX(), nested.getMaxY()}
                };

                for (const auto& corner : corners) {
                    auto world = child->convertToWorldSpace(corner);
                    auto point = node->convertToNodeSpace(world);

                    minX = std::min(minX, point.x);
                    minY = std::min(minY, point.y);
                    maxX = std::max(maxX, point.x);
                    maxY = std::max(maxY, point.y);
                }

                hasBounds = true;
            }
        }
    }

    if (!hasBounds) return {0, 0, 0, 0};
    return {minX, minY, maxX - minX, maxY - minY};
}

AxisBounds getAvailableSpace(cocos2d::CCNode* a, cocos2d::CCNode* b, Axis axis, const AxisBounds& offset, const std::vector<cocos2d::CCNode*>& ignore) {
    auto parent = a->getParent();

    auto aBounds = getRealBounds(a, ignore);
    auto bBounds = getRealBounds(b, ignore);

    auto aBottomLeft = a->convertToWorldSpace(aBounds.origin);
    auto aTopRight = a->convertToWorldSpace({
        aBounds.getMaxX(),
        aBounds.getMaxY()
    });

    auto bBottomLeft = b->convertToWorldSpace(bBounds.origin);
    auto bTopRight = b->convertToWorldSpace({
        bBounds.getMaxX(),
        bBounds.getMaxY()
    });

    auto aMinPoint = parent->convertToNodeSpace(aBottomLeft);
    auto aMaxPoint = parent->convertToNodeSpace(aTopRight);

    auto bMinPoint = parent->convertToNodeSpace(bBottomLeft);
    auto bMaxPoint = parent->convertToNodeSpace(bTopRight);

    float aMin = axis == Axis::Horizontal ? aMinPoint.x : aMinPoint.y;
    float aMax = axis == Axis::Horizontal ? aMaxPoint.x : aMaxPoint.y;

    float bMin = axis == Axis::Horizontal ? bMinPoint.x : bMinPoint.y;
    float bMax = axis == Axis::Horizontal ? bMaxPoint.x : bMaxPoint.y;

    if (aMax <= bMin) {
        return {aMax + offset.min, bMin + offset.max};
    }

    if (bMax <= aMin) {
        return {bMax + offset.min, aMin + offset.max};
    }

    return {0, 0};
}

bool nodeFits(cocos2d::CCNode* node, const AxisBounds& bounds, Axis axis) {
    auto realBounds = getRealBounds(node);
    auto realSize = realBounds.size * node->getScale();
    float check = axis == Axis::Horizontal ? realSize.width : realSize.height;

    float sizeCheck = bounds.max - bounds.min;
    return check <= sizeCheck;
}

float getFurthestLeft(cocos2d::CCNode* node, float x) {
    auto parent = node->getParent();
    if (!parent) return x;

    auto nodeBounds = getRealBounds(node);

    auto nodeBottomLeft = node->convertToWorldSpace(nodeBounds.origin);
    auto nodeTopRight = node->convertToWorldSpace({
        nodeBounds.getMaxX(),
        nodeBounds.getMaxY()
    });

    auto nodeMin = parent->convertToNodeSpace(nodeBottomLeft);
    auto nodeMax = parent->convertToNodeSpace(nodeTopRight);

    float furthest = -FLT_MAX;

    for (auto child : parent->getChildrenExt()) {
        if (child == node) continue;
        if (!child->isVisible()) continue;

        auto childBounds = getRealBounds(child);

        auto childBottomLeft = child->convertToWorldSpace(childBounds.origin);
        auto childTopRight = child->convertToWorldSpace({
            childBounds.getMaxX(),
            childBounds.getMaxY()
        });

        auto childMin = parent->convertToNodeSpace(childBottomLeft);
        auto childMax = parent->convertToNodeSpace(childTopRight);

        if (childMax.x > x) continue;
        if (childMax.y <= nodeMin.y || childMin.y >= nodeMax.y) continue;

        furthest = std::max(furthest, childMax.x);
    }

    return furthest == -FLT_MAX ? 0 : furthest;
}

bool nodesIntersect(cocos2d::CCNode* a, cocos2d::CCNode* b) {
    if (!a || !b) return false;

    auto getWorldRect = [](cocos2d::CCNode* node) {
        auto size = node->getContentSize();

        cocos2d::CCPoint corners[4] = {
            {0, 0},
            {size.width, 0},
            {size.width, size.height},
            {0, size.height}
        };

        float minX = FLT_MAX;
        float minY = FLT_MAX;
        float maxX = -FLT_MAX;
        float maxY = -FLT_MAX;

        for (const auto& corner : corners) {
            auto world = node->convertToWorldSpace(corner);

            minX = std::min(minX, world.x);
            minY = std::min(minY, world.y);
            maxX = std::max(maxX, world.x);
            maxY = std::max(maxY, world.y);
        }

        return cocos2d::CCRect(
            minX,
            minY,
            maxX - minX,
            maxY - minY
        );
    };

    return getWorldRect(a).intersectsRect(getWorldRect(b));
}

float getToolbarHeight(bool checkVisible) {
    if (MainEditorUI::get() && checkVisible && !MainEditorUI::get()->isUIVisible()) {
        return 0;
    }
    float toolbarOffset = 0.f;
    float height = tinker::constants::ToolbarHeight;
    
    float scale = UIScaling::getToolbarScale();
    height *= scale;
    if (StatusBar::isEnabled() && scale <= 0.9f) {
        toolbarOffset = StatusBar::get()->m_toolbarOffset;
    } 
    
    return height + toolbarOffset;
}

cocos2d::CCPoint rotatePointAroundPivot(const cocos2d::CCPoint& point, const cocos2d::CCPoint& pivot, float angleDegrees) {
    float angleRadians = CC_DEGREES_TO_RADIANS(angleDegrees);

    float sinA = std::sinf(angleRadians);
    float cosA = std::cosf(angleRadians);

    auto newPoint = point - pivot;

    float xNew = newPoint.x * cosA - newPoint.y * sinA;
    float yNew = newPoint.x * sinA + newPoint.y * cosA;

    return cocos2d::CCPoint(xNew + pivot.x, yNew + pivot.y);
}

}