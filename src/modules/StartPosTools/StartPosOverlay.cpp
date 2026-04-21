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
    m_button->setScale(0.8f);
    m_button->setID("playtest-button"_spr);

    m_background->addChild(m_button);
    m_button->setPosition(getContentSize() / 2);

    return true;
}

void StartPosOverlay::setStartPos(StartPosObject* startPos) {
    if (auto startPos = getStartPos()) {
        m_lastStartPos = startPos;
    }
    m_startPos = startPos;
    updateOverlay();
}

void StartPosOverlay::updateOverlay() {
    if (!getStartPos()) {
        setVisible(false);
        return;
    }
    setVisible(true);
}

void StartPosOverlay::updatePos(float dt) {
    if (auto startPos = getStartPos()) {
        setPosition({startPos->getPositionX(), startPos->boundingBox().getMaxY() + 3});
        setScale(0.8f / LevelEditorLayer::get()->m_objectLayer->getScale());
    }
}

void StartPosOverlay::checkDeletedObject(GameObject* object) {
    if (auto startPos = m_startPos.lock()) {
        if (object == startPos) m_startPos = nullptr;
    }
    if (auto startPos = m_lastStartPos.lock()) {
        if (object == startPos) m_lastStartPos = nullptr;
    }
}

StartPosObject* StartPosOverlay::getStartPos() {
    return m_startPos.lock();
}

StartPosObject* StartPosOverlay::getLastStartPos() {
    return m_lastStartPos.lock();
}