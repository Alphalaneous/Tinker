#include "nodes/StartPosOverlay.hpp"
#include "Events.hpp"
#include "modules/StartPosSwitcher.hpp"
#include "modules/UIScaling.hpp"
#include <Geode/binding/EditorUI.hpp>

using namespace geode::prelude;

namespace tinker::ui {

StartPosOverlay* StartPosOverlay::create() {
    auto ret = new StartPosOverlay();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool StartPosOverlay::init() {
    if (!CCNodeRGBA::init()) return false;

    schedule(schedule_selector(StartPosOverlay::updatePos));

    setContentSize({40.f, 40.f});

    setZOrder(10000);
    setAnchorPoint({0.5f, 0.f});
    setScale(0.8f);
    setVisible(false);

    m_background = geode::NineSlice::create("square02b_001.png");
    m_background->setOpacity(127);
    m_background->setColor({0, 0, 0});
    m_background->setContentSize(getContentSize());
    m_background->setPosition(getContentSize() / 2.f);
    m_background->setID("background"_spr);
    m_background->setLayout(SimpleRowLayout::create()
        ->setMainAxisScaling(AxisScaling::Fit)
        ->setCrossAxisScaling(AxisScaling::Fit)
        ->setGap(5.f)
        ->setPadding({3.f, 3.f, 3.f, 3.f})
    );

    addChild(m_background);

    auto playButton = geode::Button::createWithSpriteFrameName("GJ_playEditorBtn_001.png", [this] (auto sender) {
        StartPosSwitcher::get()->playFrom(m_startPos);
    });
    playButton->setScale(0.8f);
    playButton->setID("playtest-button"_spr);

    m_background->addChild(playButton);

    m_activeStartPosToggle = geode::Toggler::createWithStandardSprites(0.9f, [this] (geode::Toggler* sender, bool isToggled) {
        auto switcher = StartPosSwitcher::get();
        if (!isToggled) {
            switcher->m_startPosContainer->setStartPos(nullptr);
            return;
        }
        switcher->m_startPosContainer->setStartPos(m_startPos);
    });

    m_activeStartPosToggle->setID("active-startpos-toggle"_spr);

    m_background->addChild(m_activeStartPosToggle);

    m_background->updateLayout();

    return true;
}

void StartPosOverlay::setStartPos(StartPosObject* startPos) {
    if (startPos) {
        m_lastStartPos = startPos;
    }
    m_startPos = startPos;
    updateOverlay();
}

void StartPosOverlay::updateOverlay() {
    m_activeStartPosToggle->setToggled(m_startPos == StartPosSwitcher::get()->m_startPosContainer->getStartPos());

    if (!m_startPos) {
        setVisible(false);
        return;
    }
    setVisible(true);
}

void StartPosOverlay::updatePos(float dt) {
    if (!m_startPos) return;
    setPosition({m_startPos->getPositionX(), m_startPos->boundingBox().getMaxY() + 3.f});
    setScale((0.8f * UIScaling::getScale()) / LevelEditorLayer::get()->m_objectLayer->getScale());
}

void StartPosOverlay::checkDeletedObject(GameObject* object) {
    if (object == m_startPos) m_startPos = nullptr;
    if (object == m_lastStartPos) m_lastStartPos = nullptr;
}

void StartPosOverlay::reset() {
    m_startPos = nullptr;
    m_lastStartPos = nullptr;
}

StartPosObject* StartPosOverlay::getStartPos() {
    return m_startPos;
}

StartPosObject* StartPosOverlay::getLastStartPos() {
    return m_lastStartPos;
}

}