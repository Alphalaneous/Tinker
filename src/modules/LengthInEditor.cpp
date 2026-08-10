#include "modules/LengthInEditor.hpp"
#include "modules/UIScaling.hpp"

bool LengthInEditor::onToggled(bool state) {
    if (state) {
        onEditor();
        m_timeLabel->setString(getTime(getEditorLayer()->getLastObjectX()).c_str());
    }
    else {
        getEditor()->m_uiItems->removeObject(m_lengthContainer);
        m_lengthContainer->removeFromParent();

        auto winSize = CCDirector::get()->getWinSize();
        auto undoMenu = getEditor()->getChildByID("undo-menu");
        float scale = 1.f;
        if (undoMenu) {
            scale = undoMenu->getScale();
        }

        auto objectInfoLabel = getEditor()->getChildByID("object-info-label");
        objectInfoLabel->setPosition({objectInfoLabel->getPositionX(), winSize.height - 50.f * scale});
    }
    return true;
}

void LengthInEditor::onEditor() {
    auto winSize = CCDirector::get()->getWinSize();

    auto undoMenu = getEditor()->getChildByID("undo-menu");
    auto playbackMenu = getEditor()->getChildByID("playback-menu");

    if (!undoMenu || !playbackMenu) return;

    auto scale = playbackMenu->getScale();

    m_lengthContainer = CCNode::create();
    m_lengthContainer->setContentSize({80.f, 36.f});
    m_lengthContainer->setAnchorPoint({0.f, 1.f});
    m_lengthContainer->setID("length-container"_spr);

    auto lengthLabel = CCLabelBMFont::create("Length", "bigFont.fnt");
    lengthLabel->setAnchorPoint({0.5f, 1.f});
    lengthLabel->setPosition({m_lengthContainer->getContentWidth() / 2, m_lengthContainer->getContentHeight() - 2.f});
    lengthLabel->setScale(0.5f);
    lengthLabel->setID("length-label"_spr);

    m_timeLabel = CCLabelBMFont::create("1s", "chatFont.fnt");
    m_timeLabel->setAnchorPoint({0.5f, 0.f});
    m_timeLabel->setPosition({m_lengthContainer->getContentWidth() / 2.f, 2.f});

    m_timeLabel->setID("time-label"_spr);

    m_lengthContainer->addChild(lengthLabel);
    m_lengthContainer->addChild(m_timeLabel);

    if (!getEditorLayer()->m_levelSettings->m_platformerMode) {
        getEditor()->addChild(m_lengthContainer);
        getEditor()->m_uiItems->addObject(m_lengthContainer);
    }

    addEventListener(LevelTypeChangedEvent(), [this] (bool isPlatformer) {
        if (isPlatformer) {
            getEditor()->removeChild(m_lengthContainer);
            getEditor()->m_uiItems->removeObject(m_lengthContainer);
        }
        else {
            if (!m_lengthContainer->getParent()) {
                getEditor()->addChild(m_lengthContainer);
                getEditor()->m_uiItems->addObject(m_lengthContainer);
            }
        }
    });

    addEventListener(ObjectChangeEvent(), [this] (float lastObjectX) {
        m_timeLabel->setString(getTime(lastObjectX).c_str());
    });

    addEventListener(UIScaleUpdated(), [this] (float scale, bool scaleToolbars, bool fullReload) {
        getEditor()->runAction(CallFuncExt::create([this, scale] {
            auto undoMenu = getEditor()->getChildByID("undo-menu");
            auto playbackMenu = getEditor()->getChildByID("playback-menu");

            if (!undoMenu || !playbackMenu) return;
            m_lengthContainer->setScale(0.5f * scale);
            m_lengthContainer->setPosition(CCPoint{playbackMenu->getPositionX() - 2.f * scale, undoMenu->getPositionY() - undoMenu->getScaledContentHeight() / 2.f - 6.f * scale} + UIScaling::getSafeOffset());

            if (getEditor()->m_objectInfoLabel) {
                getEditor()->m_objectInfoLabel->setPositionY(m_lengthContainer->getPositionY() - m_lengthContainer->getScaledContentHeight() - 10.f * scale);
            }

            UpdateObjectLabel().send();
        }));
    });
}

std::string LengthInEditor::getTime(float x) {
    auto point = CCPoint{x + 340.f, 0.f};
    
    int seconds = LevelTools::timeForPos(point, getEditorLayer()->m_drawGridLayer->m_speedObjects, (int)getEditorLayer()->m_levelSettings->m_startSpeed, 0, 0, 0, 0, 0, 0, 0);
    int timestamp = getEditorLayer()->m_level->m_timestamp;
    float time = timestamp / 240.0f;
    if (timestamp > 0 && seconds < time) {
        seconds = time;
    }

    auto duration = std::chrono::seconds(seconds);
    auto formattedTime = std::chrono::hh_mm_ss(duration);

    std::string timeString;

    if (formattedTime.hours().count() > 0) {
        timeString = fmt::format("{}h {}m {}s", 
            formattedTime.hours().count(), 
            formattedTime.minutes().count(), 
            formattedTime.seconds().count());
    } else if (formattedTime.minutes().count() > 0) {
        timeString = fmt::format("{}m {}s", 
            formattedTime.minutes().count(), 
            formattedTime.seconds().count());
    } else {
        timeString = fmt::format("{}s", formattedTime.seconds().count());
    }

    return timeString;
}