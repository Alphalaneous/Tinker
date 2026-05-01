#include "LengthInEditor.hpp"

bool LengthInEditor::onToggled(bool state) {
    if (state) {
        onEditor();
        onObjectChange(m_editorLayer->getLastObjectX());
    }
    else {
        m_editorUI->m_uiItems->removeObject(m_lengthContainer);
        m_lengthContainer->removeFromParent();

        auto objectInfoLabel = m_editorUI->getChildByID("object-info-label");
        objectInfoLabel->setPosition({objectInfoLabel->getPositionX(), objectInfoLabel->getPositionY() + 25});
    }
    return true;
}

void LengthInEditor::onEditor() {
    auto winSize = CCDirector::get()->getWinSize();
    auto objectInfoLabel = m_editorUI->getChildByID("object-info-label");

    m_lengthContainer = CCNode::create();

    m_lengthContainer->setPosition({objectInfoLabel->getPositionX() + 20, objectInfoLabel->getPositionY() - 10});
    m_lengthContainer->setScale(0.5f);
    m_lengthContainer->setID("length-container"_spr);
    objectInfoLabel->setPosition({objectInfoLabel->getPositionX(), objectInfoLabel->getPositionY() - 25});

    auto lengthLabel = CCLabelBMFont::create("Length", "bigFont.fnt");
    lengthLabel->setPositionY(8);
    lengthLabel->setScale(0.5f);
    lengthLabel->setID("length-label"_spr);

    m_timeLabel = CCLabelBMFont::create("1s", "chatFont.fnt");
    m_timeLabel->setPositionY(-8);
    m_timeLabel->setID("time-label"_spr);

    m_lengthContainer->addChild(lengthLabel);
    m_lengthContainer->addChild(m_timeLabel);

    if (!m_editorUI->m_editorLayer->m_levelSettings->m_platformerMode) {
        m_editorUI->addChild(m_lengthContainer);
        m_editorUI->m_uiItems->addObject(m_lengthContainer);
    }
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