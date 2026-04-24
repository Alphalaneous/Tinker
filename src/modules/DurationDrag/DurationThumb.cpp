#include "DurationThumb.hpp"
#include "DurationControl.hpp"
#include "Utils.hpp"
#include "../../Utils.hpp"

using namespace tinker::ui;

DurationThumb* DurationThumb::create(CCNode* parent, EffectGameObject* object, ThumbType thumbType) {
    auto ret = new DurationThumb();
    if (ret->init(parent, object, thumbType)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool DurationThumb::init(CCNode* parent, EffectGameObject* object, ThumbType thumbType) {
    m_object = object;
    m_thumbType = thumbType;
    m_parent = parent;

    m_control = typeinfo_cast<DurationControl*>(parent);

    m_thumbContainer = CCNodeRGBA::create();
    m_thumbContainer->setCascadeColorEnabled(true);
    m_thumbContainer->setCascadeOpacityEnabled(true);
    m_thumbContainer->setAnchorPoint({0.5f, 0.5f});

    m_centerNode = CCNode::create();
    addChild(m_thumbContainer);
    m_thumbContainer->addChild(m_centerNode);

    if (thumbType == ThumbType::Default) {
        m_thumbSpr = CCSprite::create("sliderthumb.png");
        m_thumbSpr->setScale(0.75f);
        m_thumbContainer->setContentSize(m_thumbSpr->getScaledContentSize());
        m_centerNode->setPosition(m_thumbContainer->getContentSize() / 2);
    }
    else if (thumbType == ThumbType::Multi) {
        m_thumbSpr = CCSprite::create("sliderthumb.png");
        m_thumbContainer->setColor({50, 215, 245});
        m_thumbSpr->setScale(0.6f);
        m_thumbContainer->setContentSize(m_thumbSpr->getScaledContentSize());
        m_centerNode->setPosition(m_thumbContainer->getContentSize() / 2);
    }
    else {
        m_thumbSpr = CCSprite::create("drag-bar.png"_spr);
        const float contentW = m_thumbSpr->getScaledContentWidth();
        const float contentH = m_thumbSpr->getScaledContentHeight() + 18.0f;
        m_thumbContainer->setContentSize({contentW, contentH});

        switch (thumbType) {
            case ThumbType::FadeIn:
                m_centerNode->setPosition({contentW / 2, 8.f});
                break;
            case ThumbType::FadeOut:
                m_thumbSpr->setFlipY(true);
                m_centerNode->setPosition({contentW / 2, contentH - 8.f});
                break;
            default:
                break;
        }
    }

    setContentSize(m_thumbSpr->getScaledContentSize());
    setAnchorPoint({0.5f, 0.5f});
    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    m_thumbSpr->setPosition(m_thumbContainer->getContentSize() / 2);
    m_thumbContainer->setPosition(getContentSize() / 2);
    m_thumbContainer->addChild(m_thumbSpr);

    if (thumbType == ThumbType::Default || thumbType == ThumbType::Multi) {
        m_thumbSelectedSpr = CCSprite::create("sliderthumbsel.png");
        m_thumbSelectedSpr->setScale(m_thumbSpr->getScale());
        m_thumbSelectedSpr->setOpacity(175);
        m_thumbSelectedSpr->setVisible(false);
        m_thumbSelectedSpr->setPosition(getContentSize() / 2);
        m_thumbContainer->addChild(m_thumbSelectedSpr);
    }

    m_durationLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_durationLabel->setAnchorPoint({0.5f, 1.f});
    m_labelY = getContentHeight() + 10.f;

    switch (thumbType) {
        case ThumbType::Default:
        case ThumbType::Multi:
            setZOrder(1);
            break;
        case ThumbType::FadeIn:
            m_thumbSpr->setAnchorPoint({0.5f, 0.f});
            m_thumbSpr->setPosition({m_thumbContainer->getContentWidth() / 2, 0.f});
            m_labelPosMultiplier = -1;
            m_labelY = -10.f;
            m_durationLabel->setAnchorPoint({0.5f, 0.f});
            break;
        case ThumbType::FadeOut:
            m_thumbSpr->setAnchorPoint({0.5f, 1.f});
            m_thumbSpr->setPosition({m_thumbContainer->getContentWidth() / 2, m_thumbContainer->getContentHeight()});
            break;
    }

    resetText();
    addChild(m_durationLabel);
    scheduleUpdate();

    stopAction(m_fadeInAction);
    stopAction(m_fadeOutAction);

    m_durationLabel->setString(fmt::format("{:.2f}", m_object->m_duration).c_str());
    checkFreeMoving();

    m_object->m_endPosition = tinker::utils::duration_drag::getEndPos(m_object);
    
    switch (thumbType) {
        case ThumbType::Default: {
            setPosition(m_object->m_endPosition);
            break;
        }
        case ThumbType::Multi: {
            auto center = tinker::utils::duration_drag::getCenter(EditorUI::get());
            if (center) {
                setPosition(center.unwrap().second);
            }
            break;
        }
        default: {
            positionPulseSlider();
            break;
        }
    }

    m_overlapping = getTouchBounds().intersectsRect(m_object->boundingBox());
    setOpacity(m_overlapping ? 127 : 255);

    if (m_object->m_objectID == 1006 && thumbType != ThumbType::Default) {
        const float total = m_object->m_fadeInDuration + m_object->m_holdDuration + m_object->m_fadeOutDuration;
        setVisible(total > 0.003f);
    }

    return true;
}

void DurationThumb::resetText() {
    m_durationLabel->stopAllActions();
    m_durationLabel->setScale(0.2f);
    m_durationLabel->setPosition({getContentWidth()/2, m_labelY + 5 * m_labelPosMultiplier});
    m_durationLabel->setOpacity(0);
}

void DurationThumb::hideText(float dt) {
    m_durationLabel->stopAllActions();
    m_durationLabel->runAction(CCEaseOut::create(CCScaleTo::create(0.1f, 0.2f), 0.5));
    m_durationLabel->runAction(CCEaseOut::create(CCMoveTo::create(0.1f, {getContentWidth()/2, m_labelY + 5 * m_labelPosMultiplier}), 0.5));
    m_durationLabel->runAction(CCEaseOut::create(CCFadeTo::create(0.1f, 0), 0.5));
}

void DurationThumb::showText(float dt) {
    m_durationLabel->stopAllActions();
    m_durationLabel->runAction(CCEaseIn::create(CCScaleTo::create(0.1f, 0.4f), 0.5));
    m_durationLabel->runAction(CCEaseIn::create(CCMoveTo::create(0.1f, {getContentWidth()/2, m_labelY + 15 * m_labelPosMultiplier}), 0.5));
    m_durationLabel->runAction(CCEaseIn::create(CCFadeTo::create(0.1f, 255), 0.5));
}

CCRect DurationThumb::getTouchBounds() {
    auto size = CCSize{22, 22};
    return {
        m_parent->convertToNodeSpace(m_centerNode->convertToWorldSpace(CCPointZero)) - size/2, size
    };
}

void DurationThumb::positionPulseSlider() {
    const float total = m_object->m_fadeInDuration + m_object->m_holdDuration + m_object->m_fadeOutDuration;
    float percent = 1.0f;

    if (total > 0.f) {
        switch (m_thumbType) {
            case ThumbType::FadeIn:
                percent = m_object->m_fadeInDuration / total;
                break;
            case ThumbType::FadeOut:
                percent = (m_object->m_fadeInDuration + m_object->m_holdDuration) / total;
                break;
            default:
                break;
        }
    } else {
        percent = 0.f;
    }

    if (m_object->m_isDirty) {
        runAction(CallFuncExt::create([this] {
            if (!m_object) return;
            m_object->m_endPosition = tinker::utils::duration_drag::getEndPos(m_object);
        }));
    }

    CCPoint start = m_object->getPosition();
    if (start.x < 0 && !m_object->m_isSpawnTriggered)
        start.x = 0;

    CCPoint end = tinker::utils::duration_drag::getEndPos(m_object);

    const CCPoint newPos = {
        std::lerp(start.x, end.x, percent),
        std::lerp(start.y, end.y, percent)
    };

    setPosition(newPos);
}

void DurationThumb::update(float dt) {

    geode::Result<std::pair<CCPoint, CCPoint>> center = geode::Err("");

    if (m_thumbType == ThumbType::Multi) {

        if (m_object->m_endPosition == CCPointZero) {
            m_object->m_endPosition = tinker::utils::duration_drag::getEndPos(m_object);
        }

        center = tinker::utils::duration_drag::getCenter(EditorUI::get());
    }

    if (!m_dragging) {
        const bool isPulse = (m_object->m_objectID == 1006);
        float duration = 0.f;

        if (isPulse && m_thumbType != ThumbType::Multi) {
            const float total = m_object->m_fadeInDuration + m_object->m_holdDuration + m_object->m_fadeOutDuration;

            switch (m_thumbType) {
                case ThumbType::Multi:
                case ThumbType::Default:
                    duration = total;
                    break;
                case ThumbType::FadeIn:
                    duration = m_object->m_fadeInDuration;
                    setVisible(total > 0.00003f);
                    break;
                case ThumbType::FadeOut:
                    duration = m_object->m_fadeInDuration + m_object->m_holdDuration;
                    setVisible(total > 0.00003f);
                    break;
            }
        } else {
            duration = m_object->m_duration;
        }

        if (duration == 0.f) {
            if (m_thumbType != ThumbType::Multi) {
                CCPoint start = m_object->getPosition();
                if (start.x < 0 && !m_object->m_isSpawnTriggered)
                    start.x = 0;
                setPosition(start);
            }
            else {
                if (center) setPosition(center.unwrap().second);
            }
        } else {
            if (isPulse && m_thumbType != ThumbType::Multi)
                positionPulseSlider();
            else {
                if (m_thumbType == ThumbType::Multi) {
                    if (center) setPosition(center.unwrap().second);
                }
                else {
                    setPosition(tinker::utils::duration_drag::getEndPos(m_object));
                }
            }
        }
        setVisible(duration >= 0);
    } 
    else if (m_object->m_isDirty) {
        m_dragging = false;
    }

    const bool overlap = getTouchBounds().intersectsRect(m_object->boundingBox());
    if (overlap != m_overlapping) {
        stopAction(m_fadeInAction);
        stopAction(m_fadeOutAction);

        runAction(CallFuncExt::create([this, fadeTo = overlap ? 127 : 255] {
            runAction(
                fadeTo == 127
                    ? (m_fadeOutAction = CCFadeTo::create(0.2f, fadeTo))
                    : (m_fadeInAction  = CCFadeTo::create(0.2f, fadeTo))
            );
        }));

        m_overlapping = overlap;
    }

    checkFreeMoving();

    auto start = m_object->getPosition();
    auto end = m_object->m_endPosition;

    if (end == CCPointZero) end = start;

    bool isLesser = end.x < start.x;

    if (!m_object->m_isSpawnTriggered && !isLesser) {
        start.x = std::max(start.x, 0.f);
        end.x = std::max(end.x, 0.f);
    }

    float angle = CC_RADIANS_TO_DEGREES(std::atan2(end.y - start.y, end.x - start.x));
    if (angle < 0) angle += 360.f;
    bool flip = (angle > 90.f && angle < 270.f);

    if (flip) {
        angle = std::fmod(angle + 180.f, 360.f);
    }

    constexpr float offset = 20.f;

    if (m_control) {
        CCPoint pos;
        CCPoint anchor;
        
        if (m_thumbType == ThumbType::Multi) {
            
            if (center) {
                auto centerPoint = center.unwrap().second;
                if (isLesser) {
                    pos = CCPoint{centerPoint.x - offset, centerPoint.y};
                    anchor = CCPoint{1.f, 0.5f};
                }
                else {
                    pos = CCPoint{centerPoint.x + offset, centerPoint.y};
                    anchor = CCPoint{0.f, 0.5f};
                }
            }
        }
        else {
            if (m_thumbType == ThumbType::Default) {
                if ((angle >= 60.f && angle < 150.f) || (angle >= 240.f && angle < 330.f)) {
                    pos = CCPoint{end.x + offset, end.y};
                    anchor = CCPoint{0.f, 0.5f};
                }
                if ((angle >= 150.f && angle < 240.f) || (angle >= 330.f && angle < 60.f)) {
                    pos = CCPoint{end.x, end.y - offset};
                    anchor = CCPoint{0.5f, 1.f};
                }
            }
        }

        m_control->setMenuPosition(pos, anchor);
    }

    m_thumbContainer->setRotation(-angle);
}

void DurationThumb::checkFreeMoving() {
    auto editor = EditorUI::get();
    if ((CCKeyboardDispatcher::get()->getControlKeyPressed() || editor->m_freeMoveEnabled) && editor->m_selectedMode == 3) {
        setOpacity(100);
        setColor({100, 100, 100});
        m_disabled = true;
    }
    else {
        if (m_disabled) {
            if (m_overlapping) {
                setOpacity(127);
            }
            else {
                setOpacity(255);
            }
            m_disabled = false;
        }
        setColor({255, 255, 255});
    }
}

void DurationThumb::checkForNewLongest() {
    std::vector<EffectGameObject*> objects;
    objects.reserve(m_selectedStartingDurations.size());

    std::transform(m_selectedStartingDurations.begin(), m_selectedStartingDurations.end(), std::back_inserter(objects),
                    [](const auto& pair) { return pair.first; });

    CCPoint refStart = objects[0]->getPosition();
    CCPoint refEnd   = objects[0]->m_endPosition;
    CCPoint refDir   = refEnd - refStart;
    float refLen = std::sqrt(refDir.x * refDir.x + refDir.y * refDir.y);
    if (refLen == 0.f) refLen = 0.00001f;

    CCPoint unitRefDir = { refDir.x / refLen, refDir.y / refLen };

    m_object = tinker::utils::duration_drag::getFurthestEndObject(objects, unitRefDir);
}

void DurationThumb::select(bool select) {
    if (m_thumbSelectedSpr) {
        m_thumbSelectedSpr->setVisible(select);
        m_thumbSpr->setVisible(!select);
    }

    if (!select) {
        scheduleOnce(schedule_selector(DurationThumb::hideText), 1);

        if (m_thumbType == ThumbType::Multi) {
            checkForNewLongest();
            m_selectedStartingDurations.clear();
            auto center = tinker::utils::duration_drag::getCenter(EditorUI::get());
            if (center) {
                setPosition(center.unwrap().second);
            }
        }
    }
    else {
        unschedule(schedule_selector(DurationThumb::hideText));
    }
}

bool DurationThumb::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    auto winSize = CCDirector::get()->getWinSize();
    auto convertedPos = tinker::utils::rotatePointAroundPivot(touch->getLocation(), winSize/2, EditorUI::get()->m_editorLayer->m_gameState.m_cameraAngle);

    CCPoint touchPos = getParent()->convertToNodeSpace(convertedPos);
    if (!isVisible() || m_disabled || !getTouchBounds().containsPoint(touchPos))
        return false;

    m_dragging = true;
    bool skipSet = false;

    const auto type = m_thumbType;

    auto ensureNonZero = [](float& value) {
        if (value <= 0.f) value = 0.00001f;
    };

    if (m_object->m_objectID == 1006 && m_thumbType != ThumbType::Multi) {
        switch (type) {
            case ThumbType::Multi:
            case ThumbType::Default:
                m_startingDuration = m_object->m_fadeInDuration
                    + m_object->m_holdDuration
                    + m_object->m_fadeOutDuration;
                if (m_startingDuration <= 0.f) {
                    m_object->m_holdDuration = 0.00001f;
                    m_startingDuration = 0.00001f;
                }
                break;

            case ThumbType::FadeIn:
                skipSet = true;
                ensureNonZero(m_object->m_fadeInDuration);
                m_startingDuration = m_object->m_fadeInDuration;
                break;

            case ThumbType::FadeOut:
                skipSet = true;
                ensureNonZero(m_object->m_fadeOutDuration);
                m_startingDuration = m_object->m_fadeOutDuration;
                break;

            default:
                break;
        }

        if (m_startingDuration <= 0) m_object->m_holdDuration = 0.00001f;
        m_startingHoldDuration = m_object->m_holdDuration;
    }
    else {
        m_startingDuration = m_object->m_duration;
        if (m_startingDuration <= 0) m_object->m_duration = 0.00001f;
    }

    if (m_thumbType == ThumbType::Multi) {
        for (auto obj : CCArrayExt<GameObject*>(EditorUI::get()->m_selectedObjects)) {
            if (obj->m_dontIgnoreDuration) {
                auto effectObj = static_cast<EffectGameObject*>(obj);
                if (effectObj->m_objectID == 1006) {
                    m_selectedStartingDurations[effectObj] = effectObj->m_holdDuration;
                }
                else {
                    m_selectedStartingDurations[effectObj] = effectObj->m_duration;
                }
            }
        }
        checkForNewLongest();

        if (m_object->m_objectID == 1006) {
            m_startingDuration = m_object->m_fadeInDuration
                    + m_object->m_holdDuration
                    + m_object->m_fadeOutDuration;
            if (m_startingDuration <= 0) m_object->m_holdDuration = 0.00001f;
            m_startingHoldDuration = m_object->m_holdDuration;
        }
        else {
            m_startingDuration = m_object->m_duration;
            if (m_startingDuration <= 0) m_object->m_duration = 0.00001f;
        }

        auto center = tinker::utils::duration_drag::getCenter(EditorUI::get());
        if (center) {
            m_start = center.unwrap().first;
            m_startingEndPos = center.unwrap().second;
        }

        m_object->m_endPosition = tinker::utils::duration_drag::getEndPos(m_object);
        m_durationLabel->setString(fmt::format("{:+.2f}", 0.f).c_str());
    }
    else {
        m_startingEndPos = tinker::utils::duration_drag::getEndPos(m_object);
        if (skipSet)
            m_object->m_endPosition = m_startingEndPos;
        m_durationLabel->setString(fmt::format("{:.2f}", m_startingDuration).c_str());
    }

    ensureNonZero(m_startingDuration);

    showText(0);
    select(true);
    return true;
}

void DurationThumb::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    if (!m_dragging) return;

    auto winSize = CCDirector::get()->getWinSize();
    auto convertedPos = tinker::utils::rotatePointAroundPivot(touch->getLocation(), winSize/2, EditorUI::get()->m_editorLayer->m_gameState.m_cameraAngle);

    CCPoint start;

    if (m_thumbType == ThumbType::Multi) {
        start = m_start;
    }
    else {
        start = m_object->getPosition();
        if (start.x < 0 && !m_object->m_isSpawnTriggered)
            start.x = 0;
    }

    const CCPoint line = m_startingEndPos - start;
    const float lineLen = std::sqrt(line.x * line.x + line.y * line.y);
    if (lineLen == 0.f) return;

    const CCPoint touchPos = getParent()->convertToNodeSpace(convertedPos);

    const CCPoint ap = touchPos - start;
    float percent;

    if (m_thumbType == ThumbType::Multi) {
        percent = (ap.x * line.x + ap.y * line.y) / (lineLen * lineLen);
    }
    else {
        percent = std::max(0.f, (ap.x * line.x + ap.y * line.y) / (lineLen * lineLen));
    }

    float duration = 0.f;
    const bool isPulse = (m_object->m_objectID == 1006);

    if (isPulse && m_thumbType != ThumbType::Multi) {
        const float total = m_object->m_fadeInDuration + m_object->m_holdDuration + m_object->m_fadeOutDuration;

        switch (m_thumbType) {
            case ThumbType::Multi: 
            case ThumbType::Default: {
                float newHold = m_startingDuration * percent - m_object->m_fadeInDuration - m_object->m_fadeOutDuration;
                if (newHold < 0) newHold = 0;
                m_object->m_holdDuration = newHold;
                duration = m_object->m_fadeInDuration + m_object->m_holdDuration + m_object->m_fadeOutDuration;
                break;
            }

            case ThumbType::FadeIn: {
                float newDur = total * percent;
                float fadeOutStart = total - m_object->m_fadeOutDuration;
                newDur = std::clamp(newDur, 0.00001f, fadeOutStart - 0.00001f);
                percent = newDur / total;

                m_object->m_fadeInDuration = newDur;
                m_object->m_holdDuration = m_startingHoldDuration + (m_startingDuration - m_object->m_fadeInDuration);
                duration = m_object->m_fadeInDuration;
                break;
            }

            case ThumbType::FadeOut: {
                float newDur = total - total * percent;
                float fadeInEnd = total - m_object->m_fadeInDuration;
                newDur = std::clamp(newDur, 0.00001f, fadeInEnd - 0.00001f);
                percent = (total - newDur) / total;

                m_object->m_fadeOutDuration = newDur;
                m_object->m_holdDuration = m_startingHoldDuration + (m_startingDuration - m_object->m_fadeOutDuration);
                duration = m_object->m_fadeOutDuration;
                break;
            }
        }

        if (m_thumbType == ThumbType::Default) {
            m_object->m_endPosition = tinker::utils::duration_drag::getEndPos(m_object);
            setPosition(m_object->m_endPosition);
        }
        else {
            setPosition(start + line * percent);
        }
    } 
    else {
        if (m_thumbType == ThumbType::Default) {
            m_object->m_duration = m_startingDuration * percent;
            duration = m_object->m_duration;
            m_object->m_endPosition = tinker::utils::duration_drag::getEndPos(m_object);
            setPosition(m_object->m_endPosition);
        }
        else if (m_thumbType == ThumbType::Multi) {
            if (isPulse) {
                m_object->m_holdDuration = m_startingDuration * percent - m_object->m_fadeInDuration - m_object->m_fadeOutDuration;
                duration = m_object->m_holdDuration - m_startingHoldDuration;
                if (m_object->m_holdDuration < 0) m_object->m_holdDuration = 0.00001f;
            }
            else {
                m_object->m_duration = m_startingDuration * percent;
                duration = m_object->m_duration - m_startingDuration;
                if (m_object->m_duration < 0) m_object->m_duration = 0.00001f;
            }

            m_object->m_endPosition = tinker::utils::duration_drag::getEndPos(m_object);
            auto center = tinker::utils::duration_drag::getCenter(EditorUI::get());
            if (center) {
                setPosition(center.unwrap().second);
            }
        }
    }

    if (m_thumbType == ThumbType::Multi) {
        m_durationLabel->setString(fmt::format("{:+.2f}", duration).c_str());
        for (auto obj : CCArrayExt<GameObject*>(EditorUI::get()->m_selectedObjects)) {
            if (obj->m_dontIgnoreDuration) {
                auto effectObj = static_cast<EffectGameObject*>(obj);
                if (effectObj->m_objectID == 1006) {
                    effectObj->m_holdDuration = m_selectedStartingDurations[effectObj] + duration;
                    if (effectObj->m_holdDuration <= 0) effectObj->m_holdDuration = 0.00001;
                }
                else {
                    effectObj->m_duration = m_selectedStartingDurations[effectObj] + duration;
                    if (effectObj->m_duration <= 0) effectObj->m_duration = 0.00001;
                }
                effectObj->m_endPosition = tinker::utils::duration_drag::getEndPos(effectObj);
            }
        }
    }
    else {
        m_durationLabel->setString(fmt::format("{:.2f}", duration).c_str());
    }
}

void DurationThumb::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    m_dragging = false;
    select(false);
}

void DurationThumb::ccTouchCancelled(CCTouch* touch, CCEvent* event) {
    m_dragging = false;
    select(false);
}

void DurationThumb::registerWithTouchDispatcher() {
    CCDirector::get()
        ->getTouchDispatcher()
        ->addTargetedDelegate(this, -1000, true);
}

void DurationThumb::onEnter() {
    CCNode::onEnter();
    registerWithTouchDispatcher();
}

void DurationThumb::onExit() {
    CCNode::onExit();
    CCDirector::get()->getTouchDispatcher()->removeDelegate(this);
}