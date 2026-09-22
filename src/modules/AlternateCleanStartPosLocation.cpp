#include "modules/AlternateCleanStartPosLocation.hpp"
#include "modules/LiveColors.hpp"
#include "modules/StartPosTools.hpp"
#include "modules/UIScaling.hpp"

bool AlternateCleanStartPosLocation::onToggled(bool state) {
    auto editor = getEditor();

    if (state) {
        onEditor();
    }
    else {
        auto playtestMenu = editor->getChildByID("playtest-menu");
        if (playtestMenu && m_originalButton) {
            playtestMenu->addChild(m_originalButton);
            playtestMenu->updateLayout();
        }

        m_originalButton = nullptr;

        if (m_container) {
            m_container->removeFromParent();
            m_container = nullptr;
        }

        removeEventListener("show-ui");
        removeEventListener("ui-scale");
    }
    return true;
}

void AlternateCleanStartPosLocation::onEditor() {
    auto editor = getEditor();
    auto playtestMenu = editor->getChildByID("playtest-menu");

    if (!playtestMenu) return;

    editor->runAction(CallFuncExt::create([this, playtestMenu, editor] {   
        m_originalButton = static_cast<CCMenuItemSpriteExtra*>(playtestMenu->getChildByID("blueblock6.cleanstartpos/create-startpos-button"));
        if (!m_originalButton) return;

        m_originalButton->removeFromParent();
        playtestMenu->updateLayout();

        auto winSize = CCDirector::get()->getWinSize();

        m_container = CCNode::create();
        m_container->setAnchorPoint({0.5f, 0.f});
        m_container->setPosition({winSize.width / 2.f, 10.f});
        m_container->setVisible(false);
        m_container->setScale(UIScaling::getScale());
        m_container->setLayout(SimpleRowLayout::create()
            ->setGap(5.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
        );

        m_container->addChild(geode::Button::createWithSprite("add-startpos.png"_spr, [this] (auto sender) {
            onStartPos();
        }));


        m_container->addChild(geode::Button::createWithSprite("remove-startpos.png"_spr, [this] (auto sender) {
            onRemoveStartPos();
        }));

        m_container->updateLayout();

        editor->addChild(m_container);

        addEventListener("show-ui", ShowUIEvent(), [this, editor] (bool show) {
            m_container->setVisible(editor->m_editorLayer->m_playbackMode == PlaybackMode::Playing);

            float y = 15.f * UIScaling::getScale();
            if (LiveColors::isEnabled()) {
                y = LiveColors::get()->m_colorsMenu->getScaledContentHeight() + 5.f * UIScaling::getScale();
            }

            m_container->setPositionY(y);
        });

        addEventListener("ui-scale", UIScaleUpdated(), [this] (float scale, bool scaleToolbars, bool fullReload) {
            m_container->setScale(scale);
        });
    }));
}

void AlternateCleanStartPosLocation::onStartPos() {
    auto btn = AlternateCleanStartPosLocation::get()->m_originalButton;
    if (!btn) return;

    btn->activate();

    if (m_lastStartPos2) {
        m_lastStartPos->setUserObject("paired"_spr, m_lastStartPos2);
    }

    m_startPositionsDuringPlaytest.push_back(m_lastStartPos);

    m_lastStartPos = nullptr;
    m_lastStartPos2 = nullptr;
}

void AlternateCleanStartPosLocation::onRemoveStartPos() {
    if (m_startPositionsDuringPlaytest.empty()) return;

    auto iter = m_startPositionsDuringPlaytest.end() - 1;
    auto startPos = *iter;
    auto startPos2 = static_cast<GameObject*>(startPos->getUserObject("paired"_spr));

    auto editor = getEditorLayer();

    if (startPos2) {
        editor->removeObject(startPos2, true);
    }

    editor->removeObject(startPos, true);

    if (startPos2) {
        auto undo2 = undoObjectForStartPos(startPos2);
        if (undo2) {
            editor->m_undoObjects->removeObject(undo2);
        }
    }

    auto undo = undoObjectForStartPos(startPos);
    if (undo) {
        editor->m_undoObjects->removeObject(undo);
    }

    m_startPositionsDuringPlaytest.erase(iter);
}

UndoObject* AlternateCleanStartPosLocation::undoObjectForStartPos(GameObject* object) {
    for (auto obj : getEditorLayer()->m_undoObjects->asExt<UndoObject>()) {
        if (obj->m_objects && obj->m_objects->containsObject(object)) {
            return obj;
        }
    }
    return nullptr;
}

void ACSPLLevelEditorLayer::addSpecial(GameObject* object) {
    LevelEditorLayer::addSpecial(object);

    auto acspl = AlternateCleanStartPosLocation::get();

    if (object && object->m_objectID == 31) {
        acspl->m_lastStartPos = object;
    }

    if (object && object->m_objectID == 34) {
        acspl->m_lastStartPos2 = object;
    }
}

void ACSPLLevelEditorLayer::removeSpecial(GameObject* object) {
    if (object && object->m_objectID == 31 && m_playbackMode == PlaybackMode::Paused) {
        auto acspl = AlternateCleanStartPosLocation::get();
        auto& vec = acspl->m_startPositionsDuringPlaytest;

        auto iter = std::find(vec.begin(), vec.end(), object);
        if (iter != vec.end()) {
            vec.erase(iter);
        }
    }

    LevelEditorLayer::removeSpecial(object);
}

void ACSPLLevelEditorLayer::onStopPlaytest() {
    if (StartPosTools::isEnabled() && StartPosTools::get()->isSwitching()) {
        LevelEditorLayer::onStopPlaytest();
        return;
    }

    auto acspl = AlternateCleanStartPosLocation::get();
    if (!acspl) return;

    acspl->m_startPositionsDuringPlaytest.clear();

    LevelEditorLayer::onStopPlaytest();
}