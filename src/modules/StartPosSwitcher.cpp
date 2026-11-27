#include "modules/StartPosSwitcher.hpp"
#include <alphalaneous.editortab_api/API.hpp>
#include <alphalaneous.level-storage-api/LevelStorageAPI.hpp>
#include "modules/UIScaling.hpp"
#include "utils/Constants.hpp"
#include "utils/Objects.hpp"

using namespace geode::prelude;

namespace tinker::ui {

StartPosContainer* StartPosContainer::create(bool live) {
    auto ret = new StartPosContainer();
    if (ret->init(live)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool StartPosContainer::init(bool live) {
    setContentSize({315.f, 90.f});
    setLayout(SimpleColumnLayout::create()
        ->setMainAxisScaling(AxisScaling::Fit)
        ->setCrossAxisScaling(AxisScaling::Fit)
        ->setGap(10.f)
    );

    m_live = live;

    if (!live) {
        m_controls = CCNode::create();
        m_controls->setLayout(SimpleRowLayout::create()
            ->setGap(10.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
        );
        setAnchorPoint({0.5f, 0.5f});
        addChild(m_controls);
    }
    else {
        setAnchorPoint({0.5f, 0.f});

        auto bg = NineSlice::create("square02b_001.png");
        bg->setScaleMultiplier(1.2f);
        bg->setColor({0, 0, 0});
        bg->setOpacity(127);
        
        m_controls = bg;
        m_controls->setLayout(SimpleRowLayout::create()
            ->setGap(10.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
            ->setPadding({5.f, 0.f, 5.f, 0.f})
        );

        m_render = alpha::ui::RenderNode::create(m_controls, true);
        m_render->setID("switcher-render"_spr);
        addChild(m_render);
    }

    m_controls->setID("start-pos-controls"_spr);
    m_controls->setAnchorPoint({0.5f, 0.5f});

    m_controls->setContentSize({315.f, 30.f});

    m_titleLabel = geode::Label::create("StartPos:", "goldFont.fnt");
    m_titleLabel->setID("start-pos-label"_spr);

    if (live) {
        m_titleLabel->setScale(0.375f);
        m_titleLabel->setVisible(false);
        addChild(m_titleLabel);
    }
    else {
        m_titleLabel->setScale(0.5f);
        m_titleLabel->setPosition({32.f, getContentHeight() / 2.f});
        m_controls->addChild(m_titleLabel);
    }

    m_switcherContainer = CCNode::create();
    m_switcherContainer->setID("switcher-container"_spr);
    m_switcherContainer->setAnchorPoint({0.5f, 0.5f});
    m_switcherContainer->setContentSize({150.f, 30.f});

    m_prevButton = geode::Button::createWithSpriteFrameName("GJ_arrow_01_001.png", [this] (auto sender) {
        prevStartPos();
    });
    m_prevButton->setScale(0.55f);
    m_prevButton->setID("left-button"_spr);
    m_prevButton->setPosition({m_prevButton->getScaledContentWidth() / 2.f, m_switcherContainer->getContentHeight() / 2.f});

    m_switcherContainer->addChild(m_prevButton);

    m_currentLabel = geode::Label::create("None", "bigFont.fnt");
    m_currentLabel->setID("current-label"_spr);
    m_currentLabel->setScale(0.5f);
    m_currentLabel->setPosition({m_switcherContainer->getContentWidth() / 2.f, m_switcherContainer->getContentHeight() / 2.f});
    m_currentLabel->setLimitLabelWidth(100.f, 0.5f, 0.f);

    m_switcherContainer->addChild(m_currentLabel);

    m_nextButton = geode::Button::createWithSpriteFrameName("GJ_arrow_01_001.png", [this] (auto sender) {
        nextStartPos();
    });
    static_cast<cocos2d::CCSprite*>(m_nextButton->getDisplayNode())->setFlipX(true);
    m_nextButton->setScale(0.55f);
    m_nextButton->setID("right-button"_spr);
    m_nextButton->setPosition({m_switcherContainer->getContentWidth() - m_nextButton->getScaledContentWidth() / 2.f, m_switcherContainer->getContentHeight() / 2.f});

    m_switcherContainer->addChild(m_nextButton);

    m_controls->addChild(m_switcherContainer);

    if (!live) {
        auto buttonsContainer = CCNode::create();
        buttonsContainer->setID("button-container"_spr);
        buttonsContainer->setAnchorPoint({0.5f, 0.5f});
        buttonsContainer->setContentSize({30.f, m_switcherContainer->getContentHeight()});
        buttonsContainer->setLayout(SimpleRowLayout::create()
            ->setGap(5.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
        );

        m_editButton = geode::Button::createWithSpriteFrameName("GJ_optionsBtn02_001.png", [this] (auto sender) {
            auto switcher = StartPosSwitcher::get();
            auto editor = LevelEditorLayer::get();
            if (!m_startPos) return;

            auto settings = m_startPos->m_startSettings;
            settings->m_platformerMode = editor->m_levelSettings->m_platformerMode;
            StartPosLayer::create(m_startPos)->show();
        });
        m_editButton->setScale(0.61f);
        m_editButton->setID("edit-button"_spr);
        m_editButton->setEnabled(false);
        m_editButton->setColor({150, 150, 150});

        buttonsContainer->addChild(m_editButton);

        m_findButton = geode::Button::createWithSpriteFrameName("sideBtn_search_001.png", [this] (auto sender) {
            auto switcher = StartPosSwitcher::get();
            tinker::utils::objects::moveCameraToObject(m_startPos);
        });
        m_findButton->setScale(0.89f);
        m_findButton->setID("find-button"_spr);
        m_findButton->setEnabled(false);
        m_findButton->setColor({150, 150, 150});

        buttonsContainer->addChild(m_findButton);

        auto selectButton = geode::Button::createWithSpriteFrameName("GJ_menuBtn_001.png", [this] (auto sender) {
            auto switcher = StartPosSwitcher::get();
            
            auto arr = CCArray::create();
            for (const auto& startPos : switcher->m_startPosObjects) {
                arr->addObject(startPos);
            }

            auto startPosBrowser = StartPosBrowser::create(arr, switcher->indexForStartPos(m_startPos));
            startPosBrowser->m_delegate = this;
            startPosBrowser->show();
        });
        selectButton->setScale(0.385f);
        selectButton->setID("select-button"_spr);

        buttonsContainer->addChild(selectButton);

        m_createButton = geode::Button::createWithSpriteFrameName("GJ_plusBtn_001.png", [this] (auto sender) {
            tinker::ui::CreateStartPosPopup::create()->show();
        });
        m_createButton->setScale(0.495f);
        m_createButton->setID("create-button"_spr);
        m_createButton->setEnabled(false);
        m_createButton->setColor({150, 150, 150});

        buttonsContainer->addChild(m_createButton);

        buttonsContainer->updateLayout();

        m_controls->addChild(buttonsContainer);

        auto toggleContainer = CCNode::create();
        toggleContainer->setID("toggle-container"_spr);
        toggleContainer->setAnchorPoint({0.5f, 0.5f});
        toggleContainer->setLayout(SimpleRowLayout::create()
            ->setGap(10.f)
            ->setMainAxisScaling(AxisScaling::Fit)
            ->setCrossAxisScaling(AxisScaling::Fit)
        );
        toggleContainer->setContentSize({315.f, 30.f});
        toggleContainer->setZOrder(1);

        addToggle("Farthest\nPosition", "farthest-position", StartPosSwitcher::PlaytestStart::Farthest, toggleContainer);
        addToggle("Latest\nPlaced", "latest-placed", StartPosSwitcher::PlaytestStart::Latest, toggleContainer);
        addToggle("Near\nCamera", "near-camera", StartPosSwitcher::PlaytestStart::NearCamera, toggleContainer);
        addToggle("Use\nSwitcher", "use-switcher", StartPosSwitcher::PlaytestStart::Switcher, toggleContainer);

        toggleContainer->updateLayout();

        addChild(toggleContainer);
    }

    m_controls->updateLayout();

    if (m_live) {
        m_render->setOpacity(0);
        m_render->setContentSize(m_controls->getContentSize());
        m_render->resetFBO();
        m_controls->setPosition(m_render->getContentSize() / 2.f);
    }

    updateLayout();

    if (m_live) {
        m_titleLabel->setOpacity(0);
        m_titleLabel->setPosition({m_controls->getContentWidth() / 2.f, m_controls->getContentHeight()});
        m_titleLabel->setVisible(true);
    }

    addEventListener(SPSStartPosLayer::ClosedEvent(), [this] (StartPosLayer* self) {
        updateUI();
    });

    updateUI();

    return true;
}

void StartPosContainer::tempShow() {
    if (!m_live) return;
    
    m_render->stopAllActions();
    m_titleLabel->stopAllActions();

    auto sequence = [] {
        return CCSequence::create(
            CCShow::create(),
            CCFadeTo::create(0.05f, 255),
            CCDelayTime::create(1.5f),
            CCFadeTo::create(0.2f, 0),
            CCHide::create(),
            nullptr
        );
    };

    m_render->runAction(sequence());
    m_titleLabel->runAction(sequence());
}

void StartPosContainer::show() {
    if (!m_live) return;
    
    m_render->stopAllActions();
    m_titleLabel->stopAllActions();

    m_render->setOpacity(255);
    m_render->setVisible(true);
    m_titleLabel->setOpacity(255);
    m_titleLabel->setVisible(true);
}

void StartPosContainer::hide() {
    if (!m_live) return;
    
    m_render->stopAllActions();
    m_titleLabel->stopAllActions();

    m_render->setOpacity(0);
    m_render->setVisible(false);
    m_titleLabel->setOpacity(0);
    m_titleLabel->setVisible(false);
}

void StartPosContainer::showArrows(bool show) {
    if (!m_live) return;

    m_prevButton->setVisible(show);
    m_nextButton->setVisible(show);
    m_titleLabel->setVisible(false);

    m_switcherContainer->setContentWidth(show ? 150.f : 110.f);
    m_currentLabel->setPosition({m_switcherContainer->getContentWidth() / 2.f, m_switcherContainer->getContentHeight() / 2.f});

    m_controls->updateLayout();

    m_render->setContentSize(m_controls->getContentSize());
    m_render->resetFBO();
    m_controls->setPosition(m_render->getContentSize() / 2.f);

    m_render->setVisible(true);
    updateLayout();
    m_render->setVisible(false);

    m_titleLabel->setPosition({m_controls->getContentWidth() / 2.f, m_controls->getContentHeight()});
}

void StartPosContainer::setStartPos(StartPosObject* startPos) {
    auto switcher = StartPosSwitcher::get();
    m_startPos = startPos;
    if (!m_live) {
        StartPosSwitcher::setSaved<Save::Level, "start-position-index">(switcher->indexForStartPos(m_startPos));
    }

    updateUI();
}

StartPosObject* StartPosContainer::getStartPos() {
    return m_startPos;
}

void StartPosContainer::addToggle(std::string text, geode::ZStringView id, StartPosSwitcher::PlaytestStart type, CCNode* parent) {
    auto container = CCNode::create();
    container->setAnchorPoint({0.5f, 0.5f});
    container->setContentSize({70.f, 30.f});
    container->setID(fmt::format("{}-container"_spr, id));
    container->setLayout(geode::SimpleRowLayout::create()
        ->setMainAxisScaling(geode::AxisScaling::None)
        ->setCrossAxisScaling(geode::AxisScaling::Fit)
        ->setGap(6.f)
    );

    auto switcher = StartPosSwitcher::get();
    
    auto toggle = geode::Toggler::createWithStandardSprites(1.f, [this, switcher, parent, type] (geode::Toggler* sender, bool isToggled) {
        switcher->setSaved<Save::Level, "playtest-start">(static_cast<int>(type));
        for (auto child : parent->getChildrenExt()) {
            auto toggle = child->getChildByType<geode::Toggler>();
            toggle->setToggled(sender == toggle);
        }
    });

    toggle->setScale(0.65f);
    toggle->setID(fmt::format("{}-toggle"_spr, id));
    toggle->setToggled(switcher->getSaved<Save::Level, int, "playtest-start">(static_cast<int>(StartPosSwitcher::PlaytestStart::Latest)) == static_cast<int>(type));

    container->addChild(toggle);

    auto label = geode::Label::create(std::move(text), "bigFont.fnt");
    label->setScale(0.275f);
    label->setID(fmt::format("{}-label"_spr, id));

    container->addChild(label);

    container->updateLayout();
    
    parent->addChild(container);
}

void StartPosContainer::startPosSelected(StartPosBrowser* p0, int p1, StartPosObject* p2) {
    updateStartPos(p1);
}

void StartPosContainer::updateStartPos(int idx) {
    auto switcher = StartPosSwitcher::get();
    setStartPos(switcher->startPosForIndex(idx));

    if (m_live) {
        switcher->switchLive();
    }
}

void StartPosContainer::updateUI() {
    auto switcher = StartPosSwitcher::get();

    if (!m_live) {
        m_editButton->setEnabled(m_startPos);
        m_editButton->setOpacity(m_startPos ? 255 : 150);
        m_editButton->setColor(m_startPos ? ccColor3B{255, 255, 255} : ccColor3B{150, 150, 150});

        m_findButton->setEnabled(m_startPos);
        m_findButton->setOpacity(m_startPos ? 255 : 150);
        m_findButton->setColor(m_startPos ? ccColor3B{255, 255, 255} : ccColor3B{150, 150, 150});

        auto pausedPlaytest = LevelEditorLayer::get()->m_playbackMode == PlaybackMode::Paused;

        m_createButton->setEnabled(pausedPlaytest);
        m_createButton->setOpacity(pausedPlaytest ? 255 : 150);
        m_createButton->setColor(pausedPlaytest ? ccColor3B{255, 255, 255} : ccColor3B{150, 150, 150});
    }
    if (m_startPos) {
        auto idx = switcher->indexForStartPos(m_startPos);
        m_currentLabel->setText(fmt::format("{}: {}", idx, m_startPos->getDisplayName()));
    }
    else {
        m_currentLabel->setText("None");
    }
}

void StartPosContainer::nextStartPos() {
    auto editor = LevelEditorLayer::get();
    auto switcher = StartPosSwitcher::get();

    auto idx = switcher->indexForStartPos(m_startPos) + 1;
    if (idx >= switcher->m_startPosObjects.size() + 1) {
        idx = 0;
    }

    updateStartPos(idx);
}

void StartPosContainer::prevStartPos() {
    auto editor = LevelEditorLayer::get();
    auto switcher = StartPosSwitcher::get();

    auto idx = switcher->indexForStartPos(m_startPos) - 1;
    if (idx <= -1) {
        idx = switcher->m_startPosObjects.size();
    }

    updateStartPos(idx);
}

CreateStartPosPopup* CreateStartPosPopup::create() {
    auto ret = new CreateStartPosPopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool CreateStartPosPopup::init() {
    if (!geode::Popup::init({300.f, 180.f})) return false;
    setTitle("Create StartPos", "bigFont.fnt", 0.5f, 12.f);
    m_noElasticity = true;

    auto input = geode::TextInput::create(136.f, "StartPos Name", "bigFont.fnt");
    input->setPosition(m_size / 2.f + CCPoint{0.f, 15.f});
    input->setMaxCharCount(23);
    input->setPlaceholderColor({120, 170, 240, 255});
    input->setPlaceholderScale(0.4f);
    input->setFilter(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,-!?:;)(/\\\"'`*=+-_%[]<>|@&^#{}%$~");

    input->setZOrder(20);
    m_mainLayer->addChild(input);

    auto label = geode::Label::create("Name:", "goldFont.fnt");
    label->setScale(0.49f);
    label->setPosition(input->getPosition() + CCPoint(0.f, 20.f));
    label->setAnchorPoint(CCPoint(0.5f, 0.f));
    m_mainLayer->addChild(label);

    auto buttonContainer = CCNode::create();
    buttonContainer->setLayout(SimpleRowLayout::create()
        ->setMainAxisScaling(AxisScaling::Fit)
        ->setCrossAxisScaling(AxisScaling::Fit)
        ->setGap(15.f)
    );
    buttonContainer->setAnchorPoint({0.5f, 0.f});
    buttonContainer->setPosition({m_size.width / 2.f, 15.f});

    m_mainLayer->addChild(buttonContainer);

    auto cancelSpr = ButtonSprite::create("Cancel", 88, true, "goldFont.fnt", "GJ_button_04.png", 28.f, 0.6f);
    auto cancelBtn = geode::Button::createWithNode(cancelSpr, [this] (auto sender) {
        onClose(nullptr);
    });

    buttonContainer->addChild(cancelBtn);

    auto createSpr = ButtonSprite::create("Create", 88, true, "goldFont.fnt", "GJ_button_01.png", 28.f, 0.6f);
    auto createBtn = geode::Button::createWithNode(createSpr, [this, input] (auto sender) {
        auto name = input->getString();
        if (name.size() > 20) name = name.substr(0, 20);
        if (!name.empty()) name = ZipUtils::base64URLEncode(name);
        
        auto editor = LevelEditorLayer::get();
        auto start = static_cast<StartPosObject*>(editor->createObject(31, editor->m_player1->getPosition(), false));

        auto player = editor->m_player1;
        start->setStartPos(player->getPosition());

        auto settings = start->m_startSettings;

        settings->m_startMode = 0;

        if (player->m_isShip) {
            settings->m_startMode = 1;
        }
        else if (player->m_isBall) {
            settings->m_startMode = 2;
        }
        else if (player->m_isBird) {
            settings->m_startMode = 3;
        }
        else if (player->m_isDart) {
            settings->m_startMode = 4;
        }
        else if (player->m_isRobot) {
            settings->m_startMode = 5;
        }
        else if (player->m_isSpider) {
            settings->m_startMode = 6;
        }
        else if (player->m_isSwing) {
            settings->m_startMode = 7;
        }

        float speed = player->m_playerSpeed;
        
        settings->m_startSpeed = Speed::Normal;

        if (speed == 0.7f) {
            settings->m_startSpeed = Speed::Slow;
        }
        else if (speed == 1.1f) {
            settings->m_startSpeed = Speed::Fast;
        }
        else if (speed == 1.3f) {
            settings->m_startSpeed = Speed::Faster;
        }
        else if (speed == 1.6f) {
            settings->m_startSpeed = Speed::Fastest;
        }

        settings->m_isFlipped = player->m_isUpsideDown;
        settings->m_startMini = player->m_vehicleSize < 1.f;
        settings->m_startDual = editor->m_gameState.m_isDualMode;
        settings->m_mirrorMode = editor->m_gameState.m_isFlipped;
        settings->m_reverseGameplay = player->m_isGoingLeft;
        settings->m_rotateGameplay = player->m_isSideways;
        settings->m_targetChannel = editor->m_gameState.m_currentChannel;
        settings->m_yVelocity = player->m_yVelocity;
        settings->m_freeMode = editor->m_gameState.m_isFreeMode;
        settings->m_startPosName = name;

        auto switcher = StartPosSwitcher::get();
        switcher->m_startPosContainer->updateUI();

        onClose(nullptr);
    });

    buttonContainer->addChild(createBtn);

    buttonContainer->updateLayout();

    auto info = InfoAlertButton::create("Create Start Pos",
        "Creates a start position here with the current settings.\n<cy>Dual mode and camera settings may not be exact.</c>",
        0.9f);

    info->setPosition({m_size.width - 15.f, m_size.height - 15.f});

    m_buttonMenu->addChild(info);
    
    return true;
}

};

// todo fix removing tabs not updating the mode to have no tab buttons when there is only 1 tab left in Editor Tab API
bool StartPosSwitcher::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        m_tab->removeSelf();
        m_tab = nullptr;

        m_liveStartPosContainer->removeFromParent();
        m_liveStartPosContainer = nullptr;

        removeEventListener("editor-move");
        removeEventListener("ui-scale");
        removeEventListener("show-ui");
    }
    return true;
}

bool StartPosSwitcher::onSettingChanged(std::string_view key, const matjson::Value& value) {
    if (key == "auto-hide-switcher") {
        auto state = value.asBool().unwrapOrDefault();
        m_liveStartPosContainer->showArrows(!state);
    }

    return true;
}

void StartPosSwitcher::onEditor() {
    auto mode = alpha::editor_tabs::getMode(alpha::editor_tabs::View);
    if (!mode) return;

    auto editor = getEditor();
    auto editorLayer = getEditorLayer();

    m_startPosContainer = tinker::ui::StartPosContainer::create(false);
    m_startPosContainer->setID("start-pos-container"_spr);

    m_liveStartPosContainer = tinker::ui::StartPosContainer::create(true);
    m_liveStartPosContainer->setPosition({editor->getContentWidth() / 2.f, 20.f * UIScaling::getScale()});
    m_liveStartPosContainer->setVisible(false);
    m_liveStartPosContainer->setID("live-start-pos-switcher"_spr);

    editor->addChild(m_liveStartPosContainer);

    m_overlay = tinker::ui::StartPosOverlay::create();
	m_overlay->setID("start-pos-controls"_spr);
	editorLayer->m_objectLayer->addChild(m_overlay);

    auto node = alpha::editor_tabs::EditorTab::create();
    node->addChild(m_startPosContainer);

    m_tab = mode->createTab("start-pos-switcher"_spr, node, CCSprite::create("start-pos-tab.png"_spr));

    editorLayer->runAction(CallFuncExt::create([this, editorLayer] {
        auto idx = getSaved<Save::Level, int, "start-position-index">();
        if (idx != 0) {
            auto startPos = startPosForIndex(idx);
            m_startPosContainer->setStartPos(startPos);
        }
    }));

    addEventListener("editor-move", EditorMoveEvent(), [this, editorLayer] (cocos2d::CCPoint position) {
        if (editorLayer->m_playbackMode != PlaybackMode::Not) return;
        m_cameraDirty++;
    });

    addEventListener("ui-scale", UIScaleUpdated(), [this] (float scale, bool scaleToolbars) {
        float y = 20.f * scale;

        m_liveStartPosContainer->setScale(scale);
        m_liveStartPosContainer->setPositionY(y);
    });

    addEventListener("show-ui", ShowUIEvent(), [this, editorLayer] (bool show) {
        m_liveStartPosContainer->setVisible(editorLayer->m_playbackMode == PlaybackMode::Playing && !m_startPosObjects.empty());
    });

    float y = 20.f * UIScaling::getScale();

    m_liveStartPosContainer->setScale(UIScaling::getScale());
    m_liveStartPosContainer->setPositionY(y);

    m_liveStartPosContainer->showArrows(!getSetting<bool, "auto-hide-switcher">());
}

int StartPosSwitcher::indexForStartPos(StartPosObject* object) {
    int idx = 0;
    for (const auto& obj : m_startPosObjects) {
        if (obj == object) {
            return idx + 1;
        }
        idx++;
    }
    return 0;
}

StartPosObject* StartPosSwitcher::startPosForIndex(int index) {
    if (index == 0 || index >= m_startPosObjects.size() + 1) {
        return nullptr;
    }
    return m_startPosObjects[index - 1];
}

void StartPosSwitcher::addStartPos(StartPosObject* object) {
    m_startPosObjects.push_back(object);
    m_startPosObjectsUnsorted.push_back(object);

    sortStartPositions();

    if (m_startPosContainer) {
        m_startPosContainer->updateUI();
    }
    if (m_liveStartPosContainer) {
        m_liveStartPosContainer->updateUI();
    }
}

void StartPosSwitcher::removeStartPos(StartPosObject* object) {
    if (m_startPosContainer && m_startPosContainer->getStartPos() == object) {
        m_startPosContainer->setStartPos(nullptr);
    }

    m_startPosObjects.erase(std::find(m_startPosObjects.begin(), m_startPosObjects.end(), object));
    m_startPosObjectsUnsorted.erase(std::find(m_startPosObjectsUnsorted.begin(), m_startPosObjectsUnsorted.end(), object));

    sortStartPositions();

    if (m_startPosContainer) {
        m_startPosContainer->updateUI();
    }
    if (m_liveStartPosContainer) {
        m_liveStartPosContainer->updateUI();
    }
}

void StartPosSwitcher::switchLive() {
    auto editor = getEditor();

    m_switched = true;
    editor->onStopPlaytest(nullptr);
    editor->onPlaytest(nullptr);
    m_switched = false;
}

void StartPosSwitcher::playFrom(StartPosObject* object) {
    auto editor = getEditor();

    editor->onStopPlaytest(nullptr);
    m_tempStartPos = object;
    editor->onPlaytest(nullptr);
    m_tempStartPos = nullptr;
}

StartPosObject* StartPosSwitcher::farthestStartPos(const std::vector<Ref<StartPosObject>>& positions) {
    StartPosObject* best = nullptr;
    float bestX = 0.f;
    int bestOrder = -1;

    for (const auto& start : positions) {        
        float x = start->getPositionX();
        auto settings = start->m_startSettings;

        int order = settings->m_targetOrder;
        bool take = false;
        
        if (bestOrder < order) {
            take = true;
        } 
        else if (bestOrder <= order) {
            bool behind = settings->m_reverseGameplay ? bestX < x : x < bestX;
            take = !behind && x != bestX;
        }
        if (take) {
            best = start;
            bestX = x;
            bestOrder = order;
        }
    }
    return best;
}

StartPosObject* StartPosSwitcher::nearestStartPos(cocos2d::CCPoint center, const std::vector<geode::Ref<StartPosObject>>& positions) {
    if (m_cameraDirty < 5 && m_cameraStartPos) {
        return m_cameraStartPos;
    }

    auto editor = LevelEditorLayer::get();
    auto worldCenter = editor->m_objectLayer->convertToWorldSpace(center);

    float quarterWidth = (CCDirector::get()->getWinSize().width / editor->m_objectLayer->getScale()) / 4.f;

    StartPosObject* best = nullptr;
    StartPosObject* fallback = nullptr;

    float bestDistance = std::numeric_limits<float>::max();
    float fallbackDistance = std::numeric_limits<float>::max();

    for (const auto& start : positions) {
        auto settings = start->m_startSettings;
        if (settings->m_disableStartPos) continue;

        auto worldPos = editor->m_objectLayer->convertToWorldSpace(start->getPosition());

        bool reverse = settings->m_reverseGameplay;
        bool preferred = reverse ? worldPos.x >= worldCenter.x - quarterWidth : worldPos.x <= worldCenter.x + quarterWidth;

        float distance = worldCenter.getDistanceSq(worldPos);

        if (preferred && distance < bestDistance) {
            best = start;
            bestDistance = distance;
        }

        if (distance < fallbackDistance) {
            fallback = start;
            fallbackDistance = distance;
        }
    }

    m_cameraDirty = 0;
    m_cameraStartPos = best ? best : fallback;

    return m_cameraStartPos;
}

void StartPosSwitcher::sortStartPositions() {
    if (m_startPosObjects.size() < 2) return;

    auto remaining = m_startPosObjects;
    m_startPosObjects.clear();

    while (auto start = farthestStartPos(remaining)) {
        m_startPosObjects.insert(m_startPosObjects.begin(), start);
        remaining.erase(std::find(remaining.begin(), remaining.end(), start));
    }
}

void StartPosSwitcher::updateOverlay() {
    if (!m_overlay) return;

    auto editor = getEditor();

    if (!editor->m_selectedObject || (editor->m_selectedObjects && editor->m_selectedObjects->count() > 1)) {
        m_overlay->setStartPos(nullptr);
        return;
    }
    if (editor->m_selectedObject && editor->m_selectedObject->m_objectID == tinker::constants::objects::StartPosition) {
        auto startPos = static_cast<StartPosObject*>(editor->m_selectedObject);
        m_overlay->setStartPos(startPos);
    }
}

void SPSStartPosLayer::onClose(cocos2d::CCObject* sender) {
    Ref self = this;
    StartPosLayer::onClose(sender);
    ClosedEvent().send(this);
}

void SPSLevelEditorLayer::onPlaytest() {
    auto switcher = StartPosSwitcher::get();
    auto type = static_cast<StartPosSwitcher::PlaytestStart>(switcher->getSaved<Save::Level, int, "playtest-start">(static_cast<int>(StartPosSwitcher::PlaytestStart::Latest)));

    StartPosObject* startPos = nullptr;

    if (switcher->m_switched) {
        startPos = switcher->m_liveStartPosContainer->getStartPos();
        if (type == StartPosSwitcher::PlaytestStart::Switcher) {
            switcher->m_startPosContainer->setStartPos(startPos);
        }
    }
    else if (switcher->m_tempStartPos) {
        startPos = switcher->m_tempStartPos;
    }
    else {
        auto findStartPos = [] (const std::vector<geode::Ref<StartPosObject>>& vec) -> StartPosObject* {
            if (vec.empty()) return nullptr;

            for (int i = vec.size() - 1; i >= 0; i--) {
                auto pos = vec[i];
                if (!pos->m_startSettings->m_disableStartPos) {
                    return pos;
                }
            }

            return nullptr;
        };

        switch (type) {
            case StartPosSwitcher::PlaytestStart::Switcher: {
                startPos = switcher->m_startPosContainer->getStartPos();
                break;
            }
            case StartPosSwitcher::PlaytestStart::Farthest: {
                startPos = findStartPos(switcher->m_startPosObjects);
                break;
            }
            case StartPosSwitcher::PlaytestStart::Latest: {
                startPos = findStartPos(switcher->m_startPosObjectsUnsorted);
                break;
            }
            case StartPosSwitcher::PlaytestStart::NearCamera: {
                startPos = switcher->nearestStartPos(tinker::utils::objects::getCameraCenter(), switcher->m_startPosObjects);
                break;
            }
            default: break;
        }

        switcher->m_liveStartPosContainer->setStartPos(startPos);
    }

    bool wasDisabled = false;

    if (startPos) {
        wasDisabled = startPos->m_startSettings->m_disableStartPos;
        startPos->m_startSettings->m_disableStartPos = false;
    }

    for (const auto& pos : switcher->m_startPosObjects) {
        if (pos == startPos) continue;
        pos->m_objectID = -1;
    }

    LevelEditorLayer::onPlaytest();

    if (StartPosSwitcher::getSetting<bool, "auto-hide-switcher">()) {
        switcher->m_liveStartPosContainer->tempShow();
    }
    else {
        switcher->m_liveStartPosContainer->show();
    }

    if (startPos) {
        startPos->m_startSettings->m_disableStartPos = wasDisabled;
    }

    for (const auto& pos : switcher->m_startPosObjects) {
        pos->m_objectID = tinker::constants::objects::StartPosition;
    }
}

void SPSLevelEditorLayer::addSpecial(GameObject* object) {
    LevelEditorLayer::addSpecial(object);

    if (object->m_objectID == tinker::constants::objects::StartPosition) {
        auto switcher = StartPosSwitcher::get();
        switcher->addStartPos(static_cast<StartPosObject*>(object));
    }
}

void SPSLevelEditorLayer::removeSpecial(GameObject* object) {
    LevelEditorLayer::removeSpecial(object);

    if (object->m_objectID == tinker::constants::objects::StartPosition) {
        auto switcher = StartPosSwitcher::get();
        switcher->removeStartPos(static_cast<StartPosObject*>(object));
    }
}

void SPSLevelEditorLayer::onStopPlaytest() {
    LevelEditorLayer::onStopPlaytest();
    auto switcher = StartPosSwitcher::get();
    if (!switcher->m_switched && switcher->m_liveStartPosContainer) {
        switcher->m_liveStartPosContainer->hide();
    }
}

bool SPSEditorUI::isStartPosTab() {
    auto mode = alpha::editor_tabs::getMode(alpha::editor_tabs::View);
    auto tab = StartPosSwitcher::get()->m_tab;
    return alpha::editor_tabs::getCurrentMode() == mode && mode->getCurrentTab() == tab;
}

void SPSEditorUI::onPlaytest(cocos2d::CCObject* sender) {
    auto fields = m_fields.self();
    if (isStartPosTab()) {
        fields->m_dontToggle = true;
    }
    EditorUI::onPlaytest(sender);
    fields->m_dontToggle = false;
}

void SPSEditorUI::toggleMode(cocos2d::CCObject* sender) {
    auto fields = m_fields.self();
    if (fields->m_dontToggle) return;
    EditorUI::toggleMode(sender);
}

void SPSEditorUI::clickOnPosition(cocos2d::CCPoint position) {
    if (isStartPosTab()) {
        m_clickAtPosition = m_editorLayer->m_objectLayer->convertToNodeSpace(position);

        auto objects = m_editorLayer->objectsAtPosition(m_clickAtPosition);
        for (auto object : objects->asExt<GameObject>()) {
            if (object->m_objectID == tinker::constants::objects::StartPosition && (!m_selectedObject || m_selectedObject->m_uniqueID != object->m_uniqueID)) {
                createUndoSelectObject(false);
                selectObject(object, true);
                EditorSFXManager::sharedState()->playAction(121);
                break;
            }
        }
        updateEditMenu();
        updateSpecialUIElements();
        updateButtons();
        return;
    }
    EditorUI::clickOnPosition(position);
}

void SPSEditorUI::deselectAll() {
	EditorUI::deselectAll();
	StartPosSwitcher::get()->updateOverlay();
};

void SPSEditorUI::deselectObject(GameObject* object) {
	EditorUI::deselectObject(object);
	StartPosSwitcher::get()->updateOverlay();
}

void SPSEditorUI::selectObject(GameObject* object, bool ignoreFilter) {
	EditorUI::selectObject(object, ignoreFilter);
	StartPosSwitcher::get()->updateOverlay();
}

void SPSEditorUI::selectObjects(CCArray* objects, bool ignoreFilter) {
	EditorUI::selectObjects(objects, ignoreFilter);
	StartPosSwitcher::get()->updateOverlay();
}

bool SPSEditorUI::handleAction(int action, bool down, double timestamp) {
    if (down && m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
        auto switcher = StartPosSwitcher::get();

        switch (action) {
            case 10016: {
                switcher->m_liveStartPosContainer->prevStartPos();
                return true;
            }
            case 10017: {
                switcher->m_liveStartPosContainer->nextStartPos();
                return true;
            }
            default: break;
        }
    }

    return EditorUI::handleAction(action, down, timestamp);
}