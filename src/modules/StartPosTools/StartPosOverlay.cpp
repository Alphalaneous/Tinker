#include "StartPosOverlay.hpp"

using namespace tinker::ui;

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

    setContentSize({40, 40});

    setZOrder(10000);
    setAnchorPoint({0.5f, 0.f});
    setScale(0.8f);
    setVisible(false);

    m_background = geode::NineSlice::create("square02b_001.png");
    m_background->setOpacity(127);
    m_background->setColor({0, 0, 0});
    m_background->setContentSize(getContentSize());
    m_background->setPosition(getContentSize() / 2);
    m_background->setID("background"_spr);

    addChild(m_background);

    m_button = geode::Button::createWithSpriteFrameName("GJ_playEditorBtn_001.png", [this] (auto sender) {
        EditorUI::get()->onPlaytest(sender);
    });
    m_button->setTag(1);
    m_button->setScale(0.8f);
    m_button->setID("playtest-button"_spr);

    m_background->addChild(m_button);
    m_button->setPosition(getContentSize() / 2);

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
    if (!m_startPos) {
        setVisible(false);
        return;
    }
    setVisible(true);
}

void StartPosOverlay::updatePos(float dt) {
    if (!m_startPos) return;
    setPosition({m_startPos->getPositionX(), m_startPos->boundingBox().getMaxY() + 3});
    setScale(0.8f / LevelEditorLayer::get()->m_objectLayer->getScale());
}

void StartPosOverlay::checkDeletedObject(GameObject* object) {
    if (object == m_startPos) m_startPos = nullptr;
    if (object == m_lastStartPos) m_lastStartPos = nullptr;
}

StartPosObject* StartPosOverlay::getStartPos() {
    return m_startPos;
}

StartPosObject* StartPosOverlay::getLastStartPos() {
    return m_lastStartPos;
}