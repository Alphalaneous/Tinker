#include "CleanPause.hpp"
#include <Geode/ui/SliderNode.hpp>
#include <Geode/ui/NineSlice.hpp>
#include "modules/UIScaling.hpp"
#include "third-party/BlurAPI.hpp"

void CleanPause::onEditor() {
    addEventListener(EditorPausedEvent(), [this] (EditorPauseLayer* pauseLayer) {
        pauseLayer->setScale(0.925f);
        pauseLayer->setOpacity(0);

        float scale = 1.f;
        if (UIScaling::isEnabled()) {
            if (UIScaling::get()->m_scalePause) {
                scale = UIScaling::get()->m_scale;
            }
        }

        auto winSize = CCDirector::get()->getWinSize();

        auto bg = geode::NineSlice::create("square04_001.png");
        bg->setColor({0, 0, 0});
        bg->setOpacity(175);
        bg->setScaleMultiplier(0.8f);
        bg->setID("background-sprite"_spr);

        auto layerBG = CCLayerColor::create({0, 0, 0, 75});
        layerBG->setContentSize(winSize / pauseLayer->getScale());
        layerBG->setAnchorPoint({0.5f, 0.5f});
        layerBG->ignoreAnchorPointForPosition(false);
        layerBG->setZOrder(-1000);
        layerBG->setID("background"_spr);
        layerBG->setPosition(winSize / 2.f);
        layerBG->addChild(bg);

        if (getSetting<bool, "blur-behind">()) {
            BlurAPI::addBlur(layerBG);
        }

        bg->setPosition(layerBG->getContentSize() / 2.f);

        pauseLayer->addChild(layerBG);

        auto resumeMenu = pauseLayer->getChildByID("resume-menu");
        if (resumeMenu) {
            auto exitButton = resumeMenu->getChildByID("exit-button");
            exitButton->setZOrder(100);
            resizeButton(exitButton);

            auto saveButton = resumeMenu->getChildByID("save-button");
            saveButton->setZOrder(101);
            resizeButton(saveButton);

            resumeMenu->setLayout(RowLayout::create()
                ->setGap(9.5f)
                ->setAxisReverse(true)
                ->setAutoScale(true)
                ->setGrowCrossAxis(true)
                ->setCrossAxisOverflow(true),
            true);
        }

        auto fmod = FMODAudioEngine::get();

        auto musicLabelContainer = CCNode::create();
        musicLabelContainer->setAnchorPoint({0.5f, 0.5f});
        musicLabelContainer->setID("music-label-container"_spr);
        musicLabelContainer->setLayout(SimpleRowLayout::create()
            ->setGap(10.f)
            ->setMainAxisScaling(AxisScaling::Grow)
            ->setCrossAxisScaling(AxisScaling::Grow)
            ->setCrossAxisAlignment(CrossAxisAlignment::Start)
        );

        pauseLayer->addChild(musicLabelContainer);

        auto musicLabel = CCLabelBMFont::create("Music:", "bigFont.fnt");
        musicLabel->setID("music-label"_spr);
        musicLabelContainer->addChild(musicLabel);

        auto musicInput = geode::TextInput::create(50.f, "%");
        musicInput->setScale(1.2f);
        musicLabelContainer->addChild(musicInput);

        musicLabelContainer->updateLayout();

        auto musicSlider = SliderNode::create([fmod] (SliderNode* sender, float value) {
            fmod->m_musicVolume = value / 100.f;
            if (fmod->m_backgroundMusicChannel) {
                fmod->m_backgroundMusicChannel->setVolume(value / 100.f);
            }
        });
        musicSlider->setID("music-slider"_spr);
        musicSlider->linkTextInput(musicInput, 0);
        musicSlider->setValue(fmod->m_musicVolume * 100.f);
        pauseLayer->addChild(musicSlider);

        auto sfxLabelContainer = CCNode::create();
        sfxLabelContainer->setAnchorPoint({0.5f, 0.5f});
        sfxLabelContainer->setID("sfx-label-container"_spr);
        sfxLabelContainer->setLayout(SimpleRowLayout::create()
            ->setGap(10.f)
            ->setMainAxisScaling(AxisScaling::Grow)
            ->setCrossAxisScaling(AxisScaling::Grow)
            ->setCrossAxisAlignment(CrossAxisAlignment::Start)
        );

        pauseLayer->addChild(sfxLabelContainer);

        auto sfxLabel = CCLabelBMFont::create("SFX:", "bigFont.fnt");
        sfxLabel->setID("sfx-label"_spr);

        sfxLabelContainer->addChild(sfxLabel);

        auto sfxInput = geode::TextInput::create(50.f, "%");
        sfxInput->setScale(1.2f);

        sfxLabelContainer->addChild(sfxInput);

        sfxLabelContainer->updateLayout();

        auto sfxSlider = SliderNode::create([fmod] (SliderNode* sender, float value) {
            fmod->m_sfxVolume = value / 100.f;
            if (fmod->m_globalChannel) {
                fmod->m_globalChannel->setVolume(value / 100.f);
            }
        });
        sfxSlider->setID("sfx-slider"_spr);
        sfxSlider->linkTextInput(sfxInput, 0);
        sfxSlider->setValue(fmod->m_sfxVolume * 100.f);
        pauseLayer->addChild(sfxSlider);

        scaleAndPosition(pauseLayer, scale);
    });

    addEventListener(PauseUIScaleUpdated(), [this] (EditorPauseLayer* pauseLayer, float scale) {
        if (UIScaling::isEnabled() && !UIScaling::get()->m_scalePause) {
            scale = 1.f;
        }
        scaleAndPosition(pauseLayer, scale);
    });
}

void CleanPause::scaleAndPosition(EditorPauseLayer* pauseLayer, float scale) {
    auto winSize = CCDirector::get()->getWinSize();

    auto resumeMenu = pauseLayer->getChildByID("resume-menu");
    if (resumeMenu) {
        resumeMenu->setPosition(winSize / 2 + CCPoint{0.f, 20.f * scale});
        resumeMenu->setScale(0.9f * scale);
    }

    auto infoMenu = pauseLayer->getChildByID("info-menu");
    if (infoMenu) {
        infoMenu->setScale(scale);
    }

    bool isNewNodeIDs = Loader::get()->getInstalledMod("geode.node-ids")->getVersion() > VersionInfo{1, 23, 3};
    auto actionsMenu = pauseLayer->getChildByID("actions-menu");

    if (actionsMenu) {
        actionsMenu->setScale(scale);
        actionsMenu->setAnchorPoint({0.5f, 0.f});
        actionsMenu->setPosition(CCPoint{winSize.width - 10.f * scale - actionsMenu->getScaledContentWidth() / 2.f, 10.f * scale} - UIScaling::getSafeOffset());
        
        if (auto smallActionsMenu = pauseLayer->getChildByID("small-actions-menu")) {
            smallActionsMenu->setScale(scale);
            smallActionsMenu->setAnchorPoint({0.5f, 0.f});
            smallActionsMenu->setPosition(CCPoint{actionsMenu->getPositionX() - actionsMenu->getScaledContentWidth() / 2 - 6.f * scale - smallActionsMenu->getScaledContentWidth() / 2, 10.f * scale});
        }
    }

    auto optionsMenu = pauseLayer->getChildByID("options-menu");
    if (optionsMenu) {
        if (tinker::utils::getMod<"razoom.improved_playtest">() && !isNewNodeIDs) {
            optionsMenu->setScale(scale * 0.925f);
        }
        else {
            optionsMenu->setScale(scale);
        }
        optionsMenu->setAnchorPoint({0.f, 0.f});
        optionsMenu->setPosition(CCPoint{8.5f * scale, 14.5f * scale} + UIScaling::getSafeOffset());
        if (isNewNodeIDs) {
            optionsMenu->setContentSize({ 120.f, (winSize.height - 62.f) / scale});
            optionsMenu->updateLayout();
        }
    }

    auto settingsMenu = pauseLayer->getChildByID("settings-menu");
    if (settingsMenu) {
        settingsMenu->setScale(scale);

        if (isNewNodeIDs) {
            if (actionsMenu) {
                settingsMenu->setAnchorPoint({0.5f, 1.f});
                settingsMenu->setPosition({actionsMenu->getPositionX(), winSize.height - 5.f * scale});
            }
            else {
                settingsMenu->setAnchorPoint({0.5f, 0.5f});
                settingsMenu->setPosition(CCPoint{winSize.width - 2.f * scale - settingsMenu->getScaledContentWidth() / 2.f, winSize.height - 34.f * scale - settingsMenu->getScaledContentHeight() / 2.f});
            }
        }
        else {
            settingsMenu->setAnchorPoint({0.5f, 0.5f});
            if (actionsMenu) {
                settingsMenu->setPosition(CCPoint{actionsMenu->getPositionX(), winSize.height - 34.f * scale - settingsMenu->getScaledContentHeight() / 2.f});
            }
            else {
                settingsMenu->setPosition(CCPoint{winSize.width - 2.f * scale - settingsMenu->getScaledContentWidth() / 2.f, winSize.height - 34.f * scale - settingsMenu->getScaledContentHeight() / 2.f} - UIScaling::getSafeOffset());
            }
        }
    }

    auto bottomMenu = pauseLayer->getChildByID("guidelines-menu");
    if (bottomMenu) {
        bottomMenu->setScale(0.9f * scale);
        bottomMenu->setPosition({winSize.width / 2.f, (28.f * 0.9f) * scale});
    }

    auto bg = pauseLayer->getChildByID("background"_spr);
    if (bg) {
        auto bgSprite = bg->getChildByID("background-sprite"_spr);
        bgSprite->setContentSize(winSize + CCSize{20.f, 0.f} - UIScaling::getSafeOffset() * 2.f);
    }

    auto musicSlider = pauseLayer->getChildByID("music-slider"_spr);
    if (musicSlider) {
        musicSlider->setScale(0.65f * scale);
        musicSlider->setPosition({winSize.width / 2.f - musicSlider->getScaledContentWidth() / 2.f - 8.f * scale, 70.f * scale});
        
        auto musicLabelContainer = pauseLayer->getChildByID("music-label-container"_spr);
        if (musicLabelContainer) {
            musicLabelContainer->setScale(0.4f * scale);
            musicLabelContainer->setPosition({musicSlider->getPositionX(), 88.f * scale});

            if (resumeMenu) {
                auto maxY = musicLabelContainer->boundingBox().getMaxY() + 10.f * scale;
                if (resumeMenu->boundingBox().getMinY() < maxY) {
                    resumeMenu->setPositionY(maxY + resumeMenu->getScaledContentHeight() / 2.f);
                }
            }
        }

    }

    auto sfxSlider = pauseLayer->getChildByID("sfx-slider"_spr);
    if (sfxSlider) {
        sfxSlider->setScale(0.65f * scale);
        sfxSlider->setPosition({winSize.width / 2.f + sfxSlider->getScaledContentWidth() / 2.f + 8.f * scale, 70.f * scale});

        auto sfxLabelContainer = pauseLayer->getChildByID("sfx-label-container"_spr);
        if (sfxLabelContainer) {
            sfxLabelContainer->setScale(0.4f * scale);
            sfxLabelContainer->setPosition({sfxSlider->getPositionX(), 88.f * scale});
        }
    }
}

void CleanPause::resizeButton(CCNode* button) {
    constexpr float width = 94.f;
    button->setContentWidth(width);

    auto btnSprite = button->getChildByType<ButtonSprite>(0);
    if (btnSprite) {
        btnSprite->setContentWidth(width);
        btnSprite->setPositionX(button->getContentWidth() / 2.f);
        auto scale9 = btnSprite->getChildByType<CCScale9Sprite>(0);

        if (scale9) {
            scale9->setContentWidth(width);
            scale9->setPositionX(btnSprite->getContentWidth() / 2.f);
        }

        auto label = btnSprite->getChildByType<CCLabelBMFont>(0);
        if (label) {
            label->setPositionX(btnSprite->getContentWidth() / 2.f);
        }
    }
}