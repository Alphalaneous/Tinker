#include "modules/LengthInEditor.hpp"
#include "modules/UIScaling.hpp"

bool LengthInEditor::onToggled(bool state) {
    auto editor = getEditor();
    if (state) {
        onEditor();
        m_timeLabel->setString(getTime(getEditorLayer()->getLastObjectX()).c_str());
        updateUI(UIScaling::getScale());
    }
    else {
        editor->m_uiItems->removeObject(m_lengthContainer);
        m_lengthContainer->removeFromParent();
        m_lengthContainer = nullptr;

        removeEventListener("level-type-changed-event");
        removeEventListener("object-change-event");
        removeEventListener("object-moved-event");
        removeEventListener("ui-scale");

        auto winSize = CCDirector::get()->getWinSize();
        auto scale = UIScaling::getScale();

        if (editor->m_objectInfoLabel) {
            editor->m_objectInfoLabel->setPosition(CCPoint{52.f * scale, winSize.height - 50.f * scale} + UIScaling::getSafeOffset());        
        }
        
        UpdateObjectLabel().send();
    }
    return true;
}

void LengthInEditor::onEditor() {
    auto winSize = CCDirector::get()->getWinSize();
    auto editor = getEditor();

    auto undoMenu = editor->getChildByID("undo-menu");
    auto playbackMenu = editor->getChildByID("playback-menu");

    if (!undoMenu || !playbackMenu) return;

    auto scale = playbackMenu->getScale();

    m_lengthContainer = CCNode::create();
    m_lengthContainer->setContentSize({80.f, 36.f});
    m_lengthContainer->setAnchorPoint({0.f, 0.5f});
    m_lengthContainer->setID("length-container"_spr);

    auto lengthLabel = geode::Label::create("Length", "bigFont.fnt");
    lengthLabel->setAnchorPoint({0.5f, 1.f});
    lengthLabel->setPosition({m_lengthContainer->getContentWidth() / 2, m_lengthContainer->getContentHeight() - 2.f});
    lengthLabel->setScale(0.5f);
    lengthLabel->setID("length-label"_spr);

    m_timeLabel = geode::Label::create("1s", "chatFont.fnt");
    m_timeLabel->setAnchorPoint({0.5f, 0.f});
    m_timeLabel->setPosition({m_lengthContainer->getContentWidth() / 2.f, 2.f});

    m_timeLabel->setID("time-label"_spr);

    m_lengthContainer->addChild(lengthLabel);
    m_lengthContainer->addChild(m_timeLabel);

    if (!getEditorLayer()->m_levelSettings->m_platformerMode) {
        editor->addChild(m_lengthContainer);
        editor->m_uiItems->addObject(m_lengthContainer);
    }

    addEventListener("level-type-changed-event", LevelTypeChangedEvent(), [this, editor] (bool isPlatformer) {
        if (isPlatformer) {
            editor->removeChild(m_lengthContainer);
            editor->m_uiItems->removeObject(m_lengthContainer);
        }
        else {
            if (!m_lengthContainer->getParent()) {
                editor->addChild(m_lengthContainer);
                editor->m_uiItems->addObject(m_lengthContainer);
            }
        }
    });

    addEventListener("object-change-event", ObjectChangeEvent(), [this] (float lastObjectX) {
        m_timeLabel->setString(getTime(lastObjectX).c_str());
    });

    addEventListener("object-moved-event", ObjectMovedEvent(), [this] () {
        m_timeLabel->setString(getTime(getEditorLayer()->getLastObjectX()).c_str());
    });

    addEventListener("ui-scale", UIScaleUpdated(), [this, editor] (float scale, bool scaleToolbars, bool fullReload) {
        editor->runAction(CallFuncExt::create([this, scale] {
            updateUI(scale);
        }));
    });
}

void LengthInEditor::updateUI(float scale) {
    auto editor = getEditor();

    auto undoMenu = editor->getChildByID("undo-menu");
    auto playbackMenu = editor->getChildByID("playback-menu");

    if (!undoMenu || !playbackMenu) return;
    m_lengthContainer->setScale(0.5f * scale);

    auto available = tinker::utils::getAvailableSpace(undoMenu, playbackMenu, tinker::utils::Axis::Vertical);

    float y = (available.min + available.max) / 2.f;
    m_lengthContainer->setPositionY(y);

    if (!tinker::utils::nodeFits(m_lengthContainer, available, tinker::utils::Axis::Vertical)) {
        m_lengthContainer->setPositionY(available.max - m_lengthContainer->getScaledContentHeight() / 2.f - 6.f * scale);
        float x = std::max(tinker::utils::getFurthestLeft(m_lengthContainer, 150.f * scale), UIScaling::getSafeOffset().x);
        m_lengthContainer->setPositionX(x + 5.f * scale);

        if (editor->m_objectInfoLabel) {
            editor->m_objectInfoLabel->setPositionY(m_lengthContainer->getPositionY() - m_lengthContainer->getScaledContentHeight() / 2.f - 5.f * scale);
        }
    }
    else {
        m_lengthContainer->setPositionX(UIScaling::getSafeOffset().x + 5.f * scale);
    }

    UpdateObjectLabel().send();
}

std::string LengthInEditor::getTime(float x) {
    auto point = CCPoint{x + 340.f, 0.f};
    auto editorLayer = LevelEditorLayer::get();
    
    int seconds = LevelTools::timeForPos(point, editorLayer->m_drawGridLayer->m_speedObjects, (int)editorLayer->m_levelSettings->m_startSpeed, 0, 0, 0, 0, 0, 0, 0);
    int timestamp = editorLayer->m_level->m_timestamp;
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