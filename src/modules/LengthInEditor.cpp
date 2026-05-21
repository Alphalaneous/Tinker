#include "LengthInEditor.hpp"
#include "UIScaling.hpp"

bool LengthInEditor::onToggled(bool state) {
    if (state) {
        onEditor();
        onObjectChange(m_editorLayer->getLastObjectX());
    }
    else {
        m_editorUI->m_uiItems->removeObject(m_lengthContainer);
        m_lengthContainer->removeFromParent();

        auto winSize = CCDirector::get()->getWinSize();
        auto undoMenu = m_editorUI->getChildByID("undo-menu");
        float scale = 1;
        if (undoMenu) {
            scale = undoMenu->getScale();
        }

        auto objectInfoLabel = m_editorUI->getChildByID("object-info-label");
        objectInfoLabel->setPosition({objectInfoLabel->getPositionX(), winSize.height - 50 * scale});
    }
    return true;
}

void LengthInEditor::onEditor() {
    auto winSize = CCDirector::get()->getWinSize();

    auto undoMenu = m_editorUI->getChildByID("undo-menu");
    auto playbackMenu = m_editorUI->getChildByID("playback-menu");

    if (!undoMenu || !playbackMenu) return;

    auto scale = playbackMenu->getScale();

    m_lengthContainer = CCNode::create();
    m_lengthContainer->setContentSize({80, 36});
    m_lengthContainer->setAnchorPoint({0.f, 1.f});
    m_lengthContainer->setID("length-container"_spr);

    updateScale(scale);

    auto lengthLabel = CCLabelBMFont::create("Length", "bigFont.fnt");
    lengthLabel->setAnchorPoint({0.5f, 1.f});
    lengthLabel->setPosition({m_lengthContainer->getContentWidth() / 2, m_lengthContainer->getContentHeight() - 2});
    lengthLabel->setScale(0.5f);
    lengthLabel->setID("length-label"_spr);

    m_timeLabel = CCLabelBMFont::create("1s", "chatFont.fnt");
    m_timeLabel->setAnchorPoint({0.5f, 0.f});
    m_timeLabel->setPosition({m_lengthContainer->getContentWidth() / 2, 2});

    m_timeLabel->setID("time-label"_spr);

    m_lengthContainer->addChild(lengthLabel);
    m_lengthContainer->addChild(m_timeLabel);

    if (!m_editorUI->m_editorLayer->m_levelSettings->m_platformerMode) {
        m_editorUI->addChild(m_lengthContainer);
        m_editorUI->m_uiItems->addObject(m_lengthContainer);
    }
}

void LengthInEditor::updateScale(float scale) {
    m_editorUI->runAction(CallFuncExt::create([this, scale] {
        auto undoMenu = m_editorUI->getChildByID("undo-menu");
        auto playbackMenu = m_editorUI->getChildByID("playback-menu");

        if (!undoMenu || !playbackMenu) return;
        m_lengthContainer->setScale(0.5f * scale);

        if (UIScaling::isEnabled()) {
            if (!UIScaling::getSetting<bool, "top-align">() && UIScaling::getSetting<float, "scale">() <= 0.85f) {
                m_lengthContainer->setPosition(CCPoint{6 * scale, undoMenu->getPositionY() - undoMenu->getScaledContentHeight() / 2 - 6 * scale} + UIScaling::getSafeOffset());
                return;
            }
        }

        m_lengthContainer->setPosition(CCPoint{playbackMenu->getPositionX() - 2 * scale, undoMenu->getPositionY() - undoMenu->getScaledContentHeight() / 2 - 6 * scale} + UIScaling::getSafeOffset());

        if (auto objectInfoLabel = m_editorUI->getChildByID("object-info-label")) {
            objectInfoLabel->setPositionY(m_lengthContainer->getPositionY() - m_lengthContainer->getScaledContentHeight() - 10 * scale);
        }
    }));
}

void LengthInEditor::onObjectChange(float lastObjectX) {
    m_timeLabel->setString(getTime(lastObjectX).c_str());
}

void LengthInEditor::onGameTypeChange(bool isPlatformer) {
    if (isPlatformer) {
        m_editorUI->removeChild(m_lengthContainer);
        m_editorUI->m_uiItems->removeObject(m_lengthContainer);
    }
    else {
        if (!m_lengthContainer->getParent()) {
            m_editorUI->addChild(m_lengthContainer);
            m_editorUI->m_uiItems->addObject(m_lengthContainer);
        }
    }
}

std::string LengthInEditor::getTime(float x) {
    auto point = CCPoint{x + 340, 0};
    
    int seconds = LevelTools::timeForPos(point, m_editorLayer->m_drawGridLayer->m_speedObjects, (int)m_editorLayer->m_levelSettings->m_startSpeed, 0, 0, 0, 0, 0, 0, 0);
    int timestamp = m_editorLayer->m_level->m_timestamp;
    float time = timestamp/240.0f;
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