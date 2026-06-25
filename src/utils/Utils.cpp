#include "Utils.hpp"

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
        if (split.size() < 2) return {key, ""};
        auto value = str.substr(key.size() + 1);

        return {key, value};
    }

    void forEachObject(GJBaseGameLayer const* game, std::function<void(GameObject*)> const& callback) {
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

    bool isColorable(GameObject* object) {
        const auto type = object->m_objectType;
        const auto id = object->m_objectID;

        static const std::unordered_set<int> allowedIDs = {
            3027, 1594
        };

        static const std::unordered_set<int> disallowedIDs = {
            2069, 3645, 3032, 2016, 1816, 3642, 3643, 2064
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

        if (allowedIDs.contains(id)) return true;
        if (disallowedIDs.contains(id)) return false;
        if (object->isTrigger() || object->isSpeedObject()) return false;
        if (disallowedTypes.contains(type)) return false;

        return true;
    }

    ColorData getActiveColor(LevelEditorLayer* editorLayer, int colorID) {
        for (ColorActionSprite* action : editorLayer->m_effectManager->m_colorActionSpriteVector) {
            if (!action) continue;
            if (action->m_colorID != colorID || action->m_colorID <= 0) continue;

            ccColor3B color = action->m_color;

            if (colorID == 1005) color = GameManager::get()->colorForIdx(GameManager::get()->m_playerColor.value());
            if (colorID == 1006) color = GameManager::get()->colorForIdx(GameManager::get()->m_playerColor2.value());

            if (colorID == 1010) color = {0, 0, 0};
            if (colorID == 1011) color = {255, 255, 255};

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

            return {color, blending, opacity};
        }
        return {{255, 255, 255}, false, 255};
    }

    void updateCreateButtonColor(LevelEditorLayer* levelEditorLayer, CCNode* btn, int color1ID, int color2ID, const cocos2d::ccHSVValue& hsv1, const cocos2d::ccHSVValue& hsv2) {
        auto baseColorData = getActiveColor(levelEditorLayer, color1ID);
        auto detailColorData = getActiveColor(levelEditorLayer, color2ID);

        if (auto btnSprite = btn->getChildByType<ButtonSprite>(0)) {
            for (auto child : btnSprite->getChildrenExt()) {
                if (auto gameObject = typeinfo_cast<GameObject*>(child)) {
                    if (!isColorable(gameObject)) return;

                    auto baseColorDataObj = baseColorData;
                    auto detailColorDataObj = detailColorData;

                    if (auto baseColor = gameObject->m_baseColor) {
                        if (color1ID == 0) {
                            baseColor->m_colorID = baseColor->m_defaultColorID;
                            baseColorDataObj = getActiveColor(levelEditorLayer, baseColor->m_colorID);
                        }
                        else {
                            baseColor->m_colorID = color1ID;
                        }

                        baseColor->m_hsv = hsv1;

                        auto color = baseColorDataObj.color;
                        bool blending = baseColorDataObj.blending;
                        baseColor->m_opacity = baseColorDataObj.opacity / 255.f;

                        if (color1ID == 0 && baseColor->m_colorID != 1010) {
                            color = ccColor3B{255, 255, 255};
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

                        const auto& id = gameObject->m_objectID;
                        if (id == 1701 || id == 1702 || id == 1703) {
                            for (auto child : gameObject->getChildrenExt()) {
                                if (child->getChildrenCount() == 0) {
                                    if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                        spr->setBlendFunc(blend);
                                    }
                                }
                            }
                        }

                        gameObject->setBlendFunc(blend);

                        if (baseColor->m_usesHSV) color = GameToolbox::transformColor(color, baseColor->m_hsv);
                        gameObject->updateMainColor(color);
                    }
                    if (auto detailColor = gameObject->m_detailColor) {
                        if (color2ID == 0) {
                            detailColor->m_colorID = detailColor->m_defaultColorID;
                            detailColorDataObj = getActiveColor(levelEditorLayer, detailColor->m_colorID);
                        }
                        else {
                            detailColor->m_colorID = color2ID;
                        }

                        detailColor->m_hsv = hsv2;

                        auto color = detailColorDataObj.color;
                        bool blending = detailColorDataObj.blending;
                        detailColor->m_opacity = detailColorDataObj.opacity / 255.f;

                        if (color2ID == 0 && detailColor->m_colorID != 1010) {
                            color = ccColor3B{200, 200, 255};
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
                            const auto& id = gameObject->m_objectID;
                            if (id == 1701 || id == 1702 || id == 1703) {
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

                        if (detailColor->m_usesHSV) color = GameToolbox::transformColor(color, detailColor->m_hsv);
                        gameObject->updateSecondaryColor(color);
                    }
                    gameObject->setOpacity(255);
                }
            }
        }
    }

    namespace color {
        constexpr float clamp01(float v) {
            return std::clamp(v, 0.0f, 1.0f);
        }

        float wrapDegrees(float h) {
            h = std::fmod(h, 360.0f);
            if (h < 0.0f) h += 360.0f;
            return h;
        }

        HSV rgbToHsv(RGB in) {
            const float max = std::max({in.r, in.g, in.b});
            const float min = std::min({in.r, in.g, in.b});
            const float delta = max - min;

            HSV out{};
            out.v = max;

            if (max == 0.0f) {
                out.s = 0.0f;
                out.h = 0.0f;
                return out;
            }

            out.s = delta / max;

            if (delta == 0.0f) {
                out.h = 0.0f;
            } else if (max == in.r) {
                out.h = 60.0f * std::fmod((in.g - in.b) / delta, 6.0f);
            } else if (max == in.g) {
                out.h = 60.0f * (((in.b - in.r) / delta) + 2.0f);
            } else {
                out.h = 60.0f * (((in.r - in.g) / delta) + 4.0f);
            }

            if (out.h < 0.0f) out.h += 360.0f;
            return out;
        }

        RGB hsvToRgb(HSV in) {
            const float c = in.v * in.s;
            const float hPrime = in.h / 60.0f;
            const float x = c * (1.0f - std::fabs(std::fmod(hPrime, 2.0f) - 1.0f));
            const float m = in.v - c;

            float r1 = 0.0f, g1 = 0.0f, b1 = 0.0f;

            if (hPrime < 1.0f)       { r1 = c; g1 = x; }
            else if (hPrime < 2.0f)  { r1 = x; g1 = c; }
            else if (hPrime < 3.0f)  { g1 = c; b1 = x; }
            else if (hPrime < 4.0f)  { g1 = x; b1 = c; }
            else if (hPrime < 5.0f)  { r1 = x; b1 = c; }
            else                     { r1 = c; b1 = x; }

            return { r1 + m, g1 + m, b1 + m };
        }

        cocos2d::ccColor4B hueShift(cocos2d::ccColor4B color, float shiftDegrees) {
            RGB rgb {
                color.r / 255.0f,
                color.g / 255.0f,
                color.b / 255.0f
            };

            HSV hsv = rgbToHsv(rgb);
            hsv.h = wrapDegrees(hsv.h + shiftDegrees);

            RGB out = hsvToRgb(hsv);

            return cocos2d::ccColor4B{
                static_cast<std::uint8_t>(clamp01(out.r) * 255.0f),
                static_cast<std::uint8_t>(clamp01(out.g) * 255.0f),
                static_cast<std::uint8_t>(clamp01(out.b) * 255.0f),
                color.a
            };
        }
    }
}