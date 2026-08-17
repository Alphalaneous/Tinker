#include "MainHooks.hpp"
#include "modules/UIScaling.hpp"
#include "utils/Constants.hpp"
#include "utils/Utils.hpp"

namespace tinker::utils {

    void hijackButton(CCMenuItem* btn, HijackCallback::Hijack method) {
        if (btn->getUserObject("hijack"_spr)) return;

        auto hijack = HijackCallback::create(method, btn->m_pfnSelector);
        btn->setUserObject("hijack"_spr, hijack);
        btn->m_pfnSelector = menu_selector(HijackCallback::callback);
    }

    std::pair<std::string, std::string> splitIntoPair(const std::string& str) {
        auto split = geode::utils::string::split(str, ":");
        auto& key = split[0];
        if (split.size() < 2) {
            return {key, ""};
        }
        auto value = str.substr(key.size() + 1);

        return {key, value};
    }

    void forEachObject(GJBaseGameLayer const* game, geode::Function<void(GameObject*)> callback) {
        int count = game->m_sections.empty() ? -1 : game->m_sections.size();
        for (int i = game->m_leftSectionIndex; i <= game->m_rightSectionIndex && i < count; ++i) {
            auto leftSection = game->m_sections[i];
            if (!leftSection) continue;

            auto leftSectionSize = leftSection->size();
            for (int j = game->m_bottomSectionIndex; j <= game->m_topSectionIndex && j < leftSectionSize; ++j) {
                auto section = leftSection->at(j);
                if (!section) continue;

                auto sectionSize = game->m_sectionSizes[i]->at(j);
                for (int k = 0; k < sectionSize; ++k) {
                    auto obj = section->at(k);
                    if (!obj) continue;

                    callback(obj);
                }
            }
        }
    }

    CCPoint rotatePointAroundPivot(CCPoint point, CCPoint pivot, float angleDegrees) {
        float angleRadians = CC_DEGREES_TO_RADIANS(angleDegrees);

        float sinA = std::sinf(angleRadians);
        float cosA = std::cosf(angleRadians);

        point.x -= pivot.x;
        point.y -= pivot.y;

        float xNew = point.x * cosA - point.y * sinA;
        float yNew = point.x * sinA + point.y * cosA;

        return CCPoint(xNew + pivot.x, yNew + pivot.y);
    }

    std::string capitalize(std::string_view input) {
        std::string result(input);

        bool newWord = true;

        for (char &ch : result) {
            if (std::isspace(static_cast<unsigned char>(ch))) {
                newWord = true;
            } 
            else {
                if (newWord) {
                    ch = std::toupper(static_cast<unsigned char>(ch));
                    newWord = false;
                }
            }
        }

        return result;
    }

    std::vector<std::string> split(const std::string& str, const std::string& delimiter, int limit) {
        std::vector<std::string> result;
        size_t start = 0;
        size_t end = str.find(delimiter);
        int count = 0;

        while (end != std::string::npos && (count < limit - 1 || limit == -1)) {
            result.push_back(str.substr(start, end - start));
            start = end + delimiter.length();
            end = str.find(delimiter, start);
            count++;
        }

        result.push_back(str.substr(start));
        return result;
    }

    float getToolbarHeight(bool checkVisible) {
        if (MainEditorUI::get() && checkVisible && !MainEditorUI::get()->isUIVisible()) {
            return 0;
        }
        float height = tinker::constants::ToolbarHeight;
        if (UIScaling::isEnabled()) {
            height *= UIScaling::get()->m_scaleToolbar ? UIScaling::get()->m_scale : 1.f;
        }

        return height;
    }

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

    ColorData getActiveColor(LevelEditorLayer* editorLayer, int colorID) {
        using namespace tinker::constants::color_channels;
        for (ColorActionSprite* action : editorLayer->m_effectManager->m_colorActionSpriteVector) {
            if (!action) continue;
            if (action->m_colorID != colorID || action->m_colorID <= 0) continue;

            ccColor3B color = action->m_color;

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

    void updateGameObjectColor(LevelEditorLayer* levelEditorLayer, GameObject* gameObject) {
        using namespace tinker::constants;
        
        if (!isColorable(gameObject)) return;

        if (auto baseColor = gameObject->m_baseColor) {
            auto baseColorData = getActiveColor(levelEditorLayer, baseColor->m_colorID);
            bool blending = baseColorData.blending;
            auto color = baseColorData.color;

            if (baseColor->m_colorID == color_channels::Black) {
                color = ccColor3B{0, 0, 0};
            }

            gameObject->updateHSVState();

            auto blend = blending 
                ? ccBlendFunc{GL_SRC_ALPHA, GL_ONE} 
                : ccBlendFunc{GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

            if (auto anim = typeinfo_cast<AnimatedGameObject*>(gameObject)) {
                if (auto animSpr = anim->m_animatedSprite) {
                    if (auto paSpr = animSpr->m_paSprite) {
                        for (auto child : paSpr->getChildrenExt()) {
                            if (child == anim->m_eyeSpritePart && !anim->m_childSprite) continue;
                            auto spr = static_cast<CCSprite*>(child);
                            spr->setBlendFunc(blend);
                        }
                    }
                }
            }
            else if (typeinfo_cast<EnhancedGameObject*>(gameObject) || gameObject->m_hasCustomChild) {
                for (auto child : gameObject->getChildrenExt()) {
                    if (child == gameObject->m_colorSprite) continue;
                    if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                        spr->setBlendFunc(blend);
                    }
                }
            }

            if (gameObject->m_objectID == objects::SpikedSquareHazard || gameObject->m_objectID == objects::SpikedCircleHazard || gameObject->m_objectID == objects::TriangleHazard) {
                for (auto child : gameObject->getChildrenExt()) {
                    if (child->getChildrenCount() == 0) {
                        if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                            spr->setBlendFunc(blend);
                        }
                    }
                }
            }

            gameObject->setBlendFunc(blend);

            if (baseColor->m_usesHSV) {
                color = GameToolbox::transformColor(color, baseColor->m_hsv);
            }
            gameObject->updateMainColor(color);
        }
        if (auto detailColor = gameObject->m_detailColor) {
            auto detailColorData = getActiveColor(levelEditorLayer, detailColor->m_colorID);

            bool blending = detailColorData.blending;
            auto color = detailColorData.color;

            if (detailColor->m_colorID == color_channels::Black) {
                color = ccColor3B{0, 0, 0};
            }

            gameObject->updateHSVState();

            auto blend = blending 
                ? ccBlendFunc{GL_SRC_ALPHA, GL_ONE} 
                : ccBlendFunc{GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

            std::function<void(CCNode*)> applyBlend = [&](CCNode* node) {
                for (auto child : node->getChildrenExt()) {
                    if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                        spr->setBlendFunc(blend);
                        applyBlend(spr);
                    }
                }
            };
            
            if (auto anim = typeinfo_cast<AnimatedGameObject*>(gameObject)) {
                if (anim->m_childSprite) {
                    anim->m_childSprite->setBlendFunc(blend);
                }
                else {
                    if (auto eye = anim->m_eyeSpritePart) {
                        eye->setBlendFunc(blend);
                    }
                }
            }
            else if (typeinfo_cast<EnhancedGameObject*>(gameObject)) {
                for (auto child : gameObject->getChildrenExt()) {
                    applyBlend(child);
                }
            }
            else {
                if (gameObject->m_objectID == objects::SpikedSquareHazard || gameObject->m_objectID == objects::SpikedCircleHazard || gameObject->m_objectID == objects::TriangleHazard) {
                    for (auto child : gameObject->getChildrenExt()) {
                        if (child->getChildrenCount() > 0) {
                            applyBlend(child);
                            if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                spr->setBlendFunc(blend);
                            }
                        }
                    }
                }
                else {
                    applyBlend(gameObject);
                }
            }

            if (auto spr = gameObject->m_colorSprite) {
                spr->setBlendFunc(blend);
            }

            if (detailColor->m_usesHSV) {
                color = GameToolbox::transformColor(color, detailColor->m_hsv);
            }
            gameObject->updateSecondaryColor(color);
        }
        gameObject->setOpacity(255);
    }

    void updateCreateButtonColor(LevelEditorLayer* levelEditorLayer, CCNode* btn, int color1ID, int color2ID, const cocos2d::ccHSVValue& hsv1, const cocos2d::ccHSVValue& hsv2) {
        using namespace tinker::constants;
        
        auto baseColorData = getActiveColor(levelEditorLayer, color1ID);
        auto detailColorData = getActiveColor(levelEditorLayer, color2ID);

        if (auto btnSprite = btn->getChildByType<ButtonSprite>(0)) {
            for (auto child : btnSprite->getChildrenExt()) {
                if (auto gameObject = typeinfo_cast<GameObject*>(child)) {
                    if (!isColorable(gameObject)) return;

                    auto baseColorDataObj = baseColorData;
                    auto detailColorDataObj = detailColorData;

                    if (auto baseColor = gameObject->m_baseColor) {

                        bool blending = false;

                        if (color1ID == 0) {
                            baseColor->m_colorID = baseColor->m_defaultColorID;
                            baseColorDataObj = getActiveColor(levelEditorLayer, baseColor->m_colorID);
                            blending = false;
                            baseColor->m_hsv = ccHSVValue{0.f, 1.f, 1.f, false, false};
                            baseColor->m_opacity = 1.f;
                        }
                        else {
                            baseColor->m_colorID = color1ID;
                            blending = baseColorDataObj.blending;
                            baseColor->m_hsv = hsv1;
                            baseColor->m_opacity = baseColorDataObj.opacity / 255.f;
                        }

                        auto color = baseColorDataObj.color;

                        if (color1ID == 0) {
                            if (baseColor->m_colorID == color_channels::Black) {
                                color = ccColor3B{0, 0, 0};
                            }
                            else {
                                color = ccColor3B{255, 255, 255};
                            }
                        }

                        gameObject->updateHSVState();

                        auto blend = blending 
                            ? ccBlendFunc{GL_SRC_ALPHA, GL_ONE} 
                            : ccBlendFunc{GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

                        if (auto anim = typeinfo_cast<AnimatedGameObject*>(gameObject)) {
                            if (auto animSpr = anim->m_animatedSprite) {
                                if (auto paSpr = animSpr->m_paSprite) {
                                    for (auto child : paSpr->getChildrenExt()) {
                                        if (child == anim->m_eyeSpritePart && !anim->m_childSprite) continue;
                                        auto spr = static_cast<CCSprite*>(child);
                                        spr->setBlendFunc(blend);
                                    }
                                }
                            }
                        }
                        else if (typeinfo_cast<EnhancedGameObject*>(gameObject) || gameObject->m_hasCustomChild) {
                            for (auto child : gameObject->getChildrenExt()) {
                                if (child == gameObject->m_colorSprite) continue;
                                if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                    spr->setBlendFunc(blend);
                                }
                            }
                        }

                        if (gameObject->m_objectID == objects::SpikedSquareHazard || gameObject->m_objectID == objects::SpikedCircleHazard || gameObject->m_objectID == objects::TriangleHazard) {
                            for (auto child : gameObject->getChildrenExt()) {
                                if (child->getChildrenCount() == 0) {
                                    if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                        spr->setBlendFunc(blend);
                                    }
                                }
                            }
                        }

                        gameObject->setBlendFunc(blend);

                        if (baseColor->m_usesHSV) {
                            color = GameToolbox::transformColor(color, baseColor->m_hsv);
                        }
                        gameObject->updateMainColor(color);
                    }
                    if (auto detailColor = gameObject->m_detailColor) {
                        bool blending = false;

                        if (color2ID == 0) {
                            detailColor->m_colorID = detailColor->m_defaultColorID;
                            detailColorDataObj = getActiveColor(levelEditorLayer, detailColor->m_colorID);
                            blending = false;
                            detailColor->m_hsv = ccHSVValue{0.f, 1.f, 1.f, false, false};
                            detailColor->m_opacity = 1.f;
                        }
                        else {
                            detailColor->m_colorID = color2ID;
                            blending = detailColorDataObj.blending;
                            detailColor->m_hsv = hsv2;
                            detailColor->m_opacity = detailColorDataObj.opacity / 255.f;
                        }

                        auto color = detailColorDataObj.color;

                        if (color2ID == 0) {
                            if (detailColor->m_colorID == color_channels::Black) {
                                color = ccColor3B{0, 0, 0};
                            }
                            else {
                                color = ccColor3B{200, 200, 255};
                            }
                        }

                        gameObject->updateHSVState();

                        auto blend = blending 
                            ? ccBlendFunc{GL_SRC_ALPHA, GL_ONE} 
                            : ccBlendFunc{GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

                        std::function<void(CCNode*)> applyBlend = [&](CCNode* node) {
                            for (auto child : node->getChildrenExt()) {
                                if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                    spr->setBlendFunc(blend);
                                    applyBlend(spr);
                                }
                            }
                        };
                        
                        if (auto anim = typeinfo_cast<AnimatedGameObject*>(gameObject)) {
                            if (anim->m_childSprite) {
                                anim->m_childSprite->setBlendFunc(blend);
                            }
                            else {
                                if (auto eye = anim->m_eyeSpritePart) {
                                    eye->setBlendFunc(blend);
                                }
                            }
                        }
                        else if (typeinfo_cast<EnhancedGameObject*>(gameObject)) {
                            for (auto child : gameObject->getChildrenExt()) {
                                applyBlend(child);
                            }
                        }
                        else {
                            if (gameObject->m_objectID == objects::SpikedSquareHazard || gameObject->m_objectID == objects::SpikedCircleHazard || gameObject->m_objectID == objects::TriangleHazard) {
                                for (auto child : gameObject->getChildrenExt()) {
                                    if (child->getChildrenCount() > 0) {
                                        applyBlend(child);
                                        if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                            spr->setBlendFunc(blend);
                                        }
                                    }
                                }
                            }
                            else {
                                applyBlend(gameObject);
                            }
                        }

                        if (auto spr = gameObject->m_colorSprite) {
                            spr->setBlendFunc(blend);
                        }

                        if (detailColor->m_usesHSV) {
                            color = GameToolbox::transformColor(color, detailColor->m_hsv);
                        }
                        gameObject->updateSecondaryColor(color);
                    }
                    gameObject->setOpacity(255);
                }
            }
        }
    }

    ccColor3B getRealizedColor(int channelID, unsigned int depth) {
        auto channel = LevelEditorLayer::get()->m_levelSettings->m_effectManager->getColorAction(channelID);
        if (!channel) {
            return ccWHITE;
        }
        if (channel->m_copyColorLoop || depth > 10) {
            return ccGRAY;
        }
        if (channel->m_copyID) {
            return GameToolbox::transformColor(getRealizedColor(channel->m_copyID, depth + 1), channel->m_copyHSV);
        }
        return channel->m_fromColor;
    }

    CCRect getRealBounds(CCNode* node, const std::vector<CCNode*>& ignore) {
        auto winSize = CCDirector::get()->getWinSize();
        if (!node->isVisible()) return {0, 0, 0, 0};

        float minX = FLT_MAX;
        float minY = FLT_MAX;
        float maxX = -FLT_MAX;
        float maxY = -FLT_MAX;

        bool hasBounds = false;

        auto isIgnored = [&] (CCNode* child) {
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
                    auto corners = std::array<CCPoint, 4> {
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
    
    AxisBounds getAvailableSpace(CCNode* a, CCNode* b, Axis axis, AxisBounds offset, const std::vector<CCNode*>& ignore) {
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

    bool nodeFits(CCNode* node, const AxisBounds& bounds, Axis axis) {
        auto parent = node->getParent();
        if (!parent) return false;

        auto realBounds = getRealBounds(node);

        auto bottomLeft = node->convertToWorldSpace(realBounds.origin);
        auto topRight = node->convertToWorldSpace({
            realBounds.getMaxX(),
            realBounds.getMaxY()
        });

        auto minPoint = parent->convertToNodeSpace(bottomLeft);
        auto maxPoint = parent->convertToNodeSpace(topRight);

        float size = axis == Axis::Horizontal ? maxPoint.x - minPoint.x : maxPoint.y - minPoint.y;

        float available = bounds.max - bounds.min;

        return size <= available;
    }

    float getFurthestLeft(CCNode* node, float x) {
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

    std::string floatToString(float num, int precision) {
        double scale = std::pow(10.0, precision);
        auto rounded = std::round(num * scale) / scale;

        return numToString(rounded);
    }

    bool nodesIntersect(cocos2d::CCNode* a, cocos2d::CCNode* b) {
        if (!a || !b) return false;

        auto getWorldRect = [](cocos2d::CCNode* node) {
            auto size = node->getContentSize();

            CCPoint corners[4] = {
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

    CCPoint getEndPos(EffectGameObject* object) {
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

    EffectGameObject* getFurthestEndObject(const std::vector<EffectGameObject*>& objects, const CCPoint& unitRefDir) {

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

    geode::Result<std::pair<CCPoint, CCPoint>> getCenter(EditorUI* editorUI) {
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

    namespace color {
        float wrapDegrees(float h) {
            h = std::fmod(h, 360.f);
            if (h < 0.f) h += 360.f;
            return h;
        }

        HSV rgbToHsv(RGB in) {
            const float max = std::max({in.r, in.g, in.b});
            const float min = std::min({in.r, in.g, in.b});
            const float delta = max - min;

            HSV out{};
            out.v = max;

            if (max == 0.f) {
                out.s = 0.f;
                out.h = 0.f;
                return out;
            }

            out.s = delta / max;

            if (delta == 0.f) {
                out.h = 0.f;
            } 
            else if (max == in.r) {
                out.h = 60.f * std::fmod((in.g - in.b) / delta, 6.f);
            } 
            else if (max == in.g) {
                out.h = 60.f * (((in.b - in.r) / delta) + 2.f);
            } 
            else {
                out.h = 60.f * (((in.r - in.g) / delta) + 4.f);
            }

            if (out.h < 0.f) out.h += 360.f;
            return out;
        }

        RGB hsvToRgb(HSV in) {
            const float c = in.v * in.s;
            const float hPrime = in.h / 60.f;
            const float x = c * (1.f - std::fabs(std::fmod(hPrime, 2.f) - 1.f));
            const float m = in.v - c;

            float r1 = 0.f, g1 = 0.f, b1 = 0.f;

            if (hPrime < 1.f) { 
                r1 = c; g1 = x; 
            }
            else if (hPrime < 2.f) { 
                r1 = x; g1 = c; 
            }
            else if (hPrime < 3.f) { 
                g1 = c; b1 = x; 
            }
            else if (hPrime < 4.f) { 
                g1 = x; b1 = c; 
            }
            else if (hPrime < 5.f) { 
                r1 = x; b1 = c; 
            }
            else { 
                r1 = c; b1 = x; 
            }

            return {r1 + m, g1 + m, b1 + m};
        }

        ccColor3B getContrastingColor(const ccColor3B& color) {
            auto linearize = [](float c) {
                c /= 255.0f;
                return c <= 0.04045f
                    ? c / 12.92f
                    : std::pow((c + 0.055f) / 1.055f, 2.4f);
            };

            float r = linearize(color.r);
            float g = linearize(color.g);
            float b = linearize(color.b);

            float luminance = 0.2126f * r + 0.7152f * g + 0.0722f * b;

            return luminance > 0.179f
                ? ccColor3B{0, 0, 0}
                : ccColor3B{255, 255, 255};
        }

        cocos2d::ccColor4B hueShift(cocos2d::ccColor4B color, float shiftDegrees) {
            auto rgb = RGB{
                color.r / 255.f,
                color.g / 255.f,
                color.b / 255.f
            };

            auto hsv = rgbToHsv(rgb);
            hsv.h = wrapDegrees(hsv.h + shiftDegrees);

            RGB out = hsvToRgb(hsv);

            return cocos2d::ccColor4B{
                static_cast<std::uint8_t>(std::clamp(out.r, 0.f, 1.f) * 255.f),
                static_cast<std::uint8_t>(std::clamp(out.g, 0.f, 1.f) * 255.f),
                static_cast<std::uint8_t>(std::clamp(out.b, 0.f, 1.f) * 255.f),
                color.a
            };
        }
    }
}
