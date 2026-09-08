#include "modules/SaveTools.hpp"
#include "MainHooks.hpp"
#include "actions/CCValueTo.hpp"
#include "modules/UIScaling.hpp"
#include "utils/Utils.hpp"

namespace tinker::ui {

SaveAlert* SaveAlert::create() {
    auto ret = new SaveAlert();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SaveAlert::init() {
    if (!CCNodeRGBA::init()) return false;

    setContentSize({30.f, 30.f});
    setZOrder(9999);

    m_container = geode::NineSlice::create("simple-popup-square.png"_spr);
    m_container->setContentSize(getContentSize());
    m_container->setOpacity(100);
    m_container->setID("background"_spr);

    float padding = 6.f;

    m_label = geode::Label::create("bigFont.fnt");
    m_label->setAnchorPoint({0.f, 0.5f});
    m_label->setScale(0.3f);
    m_label->setID("time-label"_spr);

    m_saveIcon = CCSprite::create("save-anim.gif"_spr);
    m_saveIcon->setAnchorPoint({0.f, 0.5f});
    m_saveIcon->setScale((getContentHeight() - padding * 2.f) / m_saveIcon->getContentHeight());
    m_saveIcon->setPosition({padding, getContentHeight() / 2.f});

    m_label->setPosition({m_saveIcon->boundingBox().getMaxX() + padding, getContentHeight() / 2.f});

    m_container->addChild(m_saveIcon);
    m_container->addChild(m_label);

    m_render = alpha::ui::RenderNode::create(m_container, false);

    m_render->setPosition(getContentSize() / 2.f);
    m_render->setOpacity(0);
    m_render->setID("content-render"_spr);

    m_render->setContentSize(getContentSize());
    m_container->setContentSize(getContentSize());
    m_container->setPosition(getContentSize() / 2.f);

    addChild(m_render);

    return true;
}

void SaveAlert::setAlertColor(const ccColor3B& color) {
    m_label->setColor(color);
    m_container->setColor(color);
}

void SaveAlert::setText(ZStringView text) {
    if (m_text == text) return;

    m_text = text;
    m_label->setText(text);
    float padding = 6.f;

    setContentSize({padding + m_saveIcon->getScaledContentWidth() + padding + m_label->getScaledContentWidth() + padding + 2.f, 30.f});

    m_container->stopAllActions();
    auto contentWidthAction = CCEaseOut::create(CCValueTo<float>::create(0.1f, m_container->getContentWidth(), getContentWidth(), [this] (float t, float start, float end, float& width) {
        width = start + (end - start) * t;
        m_container->setContentWidth(width);
    }), 1.2f);

    m_container->runAction(contentWidthAction);

    m_container->setPosition(getContentSize() / 2.f);

    m_render->setContentSize(getContentSize());
    m_render->setPosition(getContentSize() / 2.f);
}

void SaveAlert::show() {
    removeFromParent();
    EditorUI::get()->addChild(this);
    m_render->runAction(CCFadeTo::create(0.25f, 255));
}

void SaveAlert::hide() {
    m_render->runAction(CCSequence::create(
        CCDelayTime::create(1.f), 
        CCFadeTo::create(0.2f, 0), 
        CallFuncExt::create([this] {
            // fixes a weird leak
            m_saveIcon->removeFromParent();
            removeFromParent();
        }),
        nullptr
    ));
}

}

bool SaveTools::onToggled(bool state) {
    auto editor = static_cast<STLevelEditorLayer*>(getEditorLayer());
    auto fields = editor->m_fields.self();

    if (state) {
        editor->schedule(schedule_selector(STLevelEditorLayer::autoSaveUpdate));
    }
    else {
        editor->unschedule(schedule_selector(STLevelEditorLayer::autoSaveUpdate));
        if (fields->m_saveAlert) {
            fields->m_saveAlert->removeFromParent();
            fields->m_saveAlert = nullptr;
        }
    }
    return true;
}

bool SaveTools::onSettingChanged(std::string_view key, const matjson::Value& value) {
    auto editor = static_cast<STLevelEditorLayer*>(getEditorLayer());
    auto fields = editor->m_fields.self();

    auto state = value.asBool().unwrapOrDefault();

    if (key == "auto-save-enabled") {
        if (state) {
            editor->schedule(schedule_selector(STLevelEditorLayer::autoSaveUpdate));
        }
        else {
            editor->unschedule(schedule_selector(STLevelEditorLayer::autoSaveUpdate));
            if (fields->m_saveAlert) {
                fields->m_saveAlert->removeFromParent();
                fields->m_saveAlert = nullptr;
            }
        }
    }
    return true;
}

void SaveTools::onEditor() {
    getEditorLayer()->schedule(schedule_selector(STLevelEditorLayer::autoSaveUpdate));
}

std::filesystem::path SaveTools::getQuickSaveDir() {
    return Mod::get()->getSaveDir() / "quicksave";
}

std::filesystem::path SaveTools::getAutoSaveDir() {
    return Mod::get()->getSaveDir() / "autosave";
}

void SaveTools::transferBetterEdit() {
    std::error_code ec;
    auto be = tinker::utils::getInstalledMod<"hjfod.betteredit">();
    if (!be) return;

    std::filesystem::rename(be->getSaveDir() / "autosave", Mod::get()->getSaveDir() / "autosave", ec);
    std::filesystem::rename(be->getSaveDir() / "quicksave", Mod::get()->getSaveDir() / "quicksave", ec);
}

void SaveTools::restoreData(const std::filesystem::path& path) {
    for (const auto& file : file::readDirectory(path).unwrapOrDefault()) {
        auto levelRes = gmd::importGmdAsLevel(file);
        if (!levelRes) continue;

        auto level = levelRes.unwrap();

        auto num = numFromString<int>(string::pathToString(file.stem()));
        if (!num) continue;

        auto levelByID = EditorIDs::getLevelByID(num.unwrap());

        if (levelByID) {
            levelByID->m_levelString = level->m_levelString;
            continue;
        }

        LocalLevelManager::get()->m_localLevels->insertObject(level, 0);
    }
}

void SaveTools::createAutoSave() {
    m_creatingAutoSave = true;
    tinker::utils::fakeEditorPauseLayer()->saveLevel();
    m_creatingAutoSave = false;
}

void SaveTools::setCreateAutoSave(bool create) {
    m_creatingAutoSave = create;
}

bool SaveTools::isCreatingAutoSave() {
    return m_creatingAutoSave;
}

void SaveTools::skipLLMSave(bool skip) {
    m_skipLLMSave = skip;
}

bool SaveTools::isSkipLLMSave() {
    return m_skipLLMSave;
}

void STGManager::save() {
    bool isLLM = typeinfo_cast<LocalLevelManager*>(this);
    if (isLLM && SaveTools::get()->isSkipLLMSave()) {
        SaveTools::get()->skipLLMSave(false);
        return;
    } 
    GManager::save();
    if (isLLM) {
        std::error_code ec;
        std::filesystem::remove_all(SaveTools::getQuickSaveDir(), ec);
        std::filesystem::remove_all(SaveTools::getAutoSaveDir(), ec);
    }
}

void STEditorPauseLayer::saveLevel() {
    SaveTools::get()->skipLLMSave(SaveTools::getSetting<bool, "quick-save">() || SaveTools::get()->isCreatingAutoSave());
    EditorPauseLayer::saveLevel();

    auto dir = SaveTools::get()->isCreatingAutoSave() ? SaveTools::getAutoSaveDir() : SaveTools::getQuickSaveDir();
    SaveTools::get()->setCreateAutoSave(false);

    (void) file::createDirectoryAll(dir);

    auto res = gmd::exportLevelAsGmd(m_editorLayer->m_level, dir / fmt::format("{}.gmd", EditorIDs::getID(m_editorLayer->m_level)));
    
    if (!res) {
        log::error("Unable to save: \"{}\": {}", m_editorLayer->m_level->m_levelName, res.unwrapErr());
    }
}

// betteredit has some view only editor stuff for backups, not doing backups yet
void STEditorPauseLayer::onExitNoSave(CCObject* sender) {
    createQuickPopup("Exit", "<cy>Exit</c> without saving? All unsaved changes will be <cr>lost</c>!\n<co>All auto-saved changes will also be discarded!</c>", "Cancel", "Exit", 300.f, [this] (auto alert, bool btn2) {
        if (btn2) {
            std::error_code ec;
            std::filesystem::remove_all(SaveTools::getAutoSaveDir() / fmt::format("{}.gmd", EditorIDs::getID(m_editorLayer->m_level)), ec);
            onExitEditor(alert);
        }
    });
}

void STLevelEditorLayer::autoSaveUpdate(float dt) {
    if (!SaveTools::getSetting<bool, "auto-save-enabled">()) return;

    constexpr auto Countdown = std::chrono::milliseconds(5000);

    auto fields = m_fields.self();

    if (fields->m_saveAlert) {
        fields->m_saveAlert->setVisible(m_playbackMode == PlaybackMode::Not && MainEditorUI::get()->isUIVisible());
    }

    if (m_playbackMode != PlaybackMode::Not) {
        return;
    }

    auto interval = std::chrono::minutes(SaveTools::getSetting<int, "auto-save-interval">());    
    fields->m_lastAutosaveTime += std::chrono::milliseconds(static_cast<int>(dt * 1000));

    if (fields->m_lastAutosaveTime > interval - Countdown) {
        if (!fields->m_saveAlert) {
            fields->m_saveAlert = tinker::ui::SaveAlert::create();
            fields->m_saveAlert->setAnchorPoint({0.5f, 1.f});
            fields->m_saveAlert->setID("auto-save-popup"_spr);
            fields->m_saveAlert->setScale(UIScaling::getScale());
            fields->m_saveAlert->setPosition({m_editorUI->m_positionSlider->getPositionX(), m_editorUI->m_positionSlider->boundingBox().getMinY() - 3.f * UIScaling::getScale()});
            fields->m_saveAlert->show();
            fields->m_saveAlert->addEventListener("ui-scale"_spr, UIScaleUpdated(), [this, fields] (float scale, bool scaleToolbars, bool fullReload) {
                fields->m_saveAlert->setScale(UIScaling::getScale());
                fields->m_saveAlert->setPosition({m_editorUI->m_positionSlider->getPositionX(), m_editorUI->m_positionSlider->boundingBox().getMinY() - 3.f * UIScaling::getScale()});
            });
        }
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(interval - fields->m_lastAutosaveTime).count() + 1;

        std::string secondsText;
        if (seconds == 1) {
            secondsText = "second";
        }
        else {
            secondsText = "seconds";
        }

        fields->m_saveAlert->setText(fmt::format("Saving in {} {}", seconds, secondsText));
    }

    if (fields->m_lastAutosaveTime > interval) {
        fields->m_saveAlert->setText("Saving...");

        fields->m_lastAutosaveTime = std::chrono::milliseconds(0);
        
        runAction(CallFuncExt::create([this, fields] {
            if (m_playbackMode != PlaybackMode::Not) {
                m_editorUI->onStopPlaytest(nullptr);
            }

            SaveTools::get()->createAutoSave();
        }));

        runAction(CCSequence::createWithTwoActions(
            CCDelayTime::create(0.5f), 
            CallFuncExt::create([this, fields] {
                fields->m_saveAlert->setAlertColor({128, 255, 128});
                fields->m_saveAlert->setText("Saved!");

                fields->m_saveAlert->hide();
                fields->m_saveAlert = nullptr;
            }
        )));
    }
}

// not alongside SaveTools so recovery can always happen as needed
$on_game(Loaded) {
    SaveTools::transferBetterEdit();

    SaveTools::restoreData(SaveTools::getAutoSaveDir());

    // Quick save data is always up to date so load after autosave
    SaveTools::restoreData(SaveTools::getQuickSaveDir());

    auto be = tinker::utils::getMod<"hjfod.betteredit">();
    if (be) {
        bool hasAutoSave = be->getSettingValue<bool>("enable-auto-save");

        Mod::get()->setSavedValue("betteredit-auto-save", hasAutoSave);
        be->setSettingValue<bool>("enable-auto-save", SaveTools::isEnabled() && SaveTools::getSetting<bool, "auto-save-enabled">() ? false : hasAutoSave);

        static bool skipSet = false;
        static bool skipPopup = false;

        listenForSettingChanges<bool>("enable-auto-save", [be] (bool value) {
            if (skipSet) return;
            Mod::get()->setSavedValue("betteredit-auto-save", value);
            skipSet = true;
            be->setSettingValue<bool>("enable-auto-save", SaveTools::isEnabled() && SaveTools::getSetting<bool, "auto-save-enabled">() ? false : value);
            skipSet = false;

            if (skipPopup) return;
            createQuickPopup("Oops!", "You cannot use BetterEdit's auto save alongside Tinker's auto save. Disable Tinker's auto save if you wish to use BetterEdit's instead.", "OK", nullptr, nullptr);
        }, be);

        listenForSettingChanges<bool>("SaveTools-auto-save-enabled", [be] (bool value) {
            auto state = Mod::get()->getSavedValue<bool>("betteredit-auto-save", be->getSettingValue<bool>("enable-auto-save"));
            skipPopup = true;
            be->setSettingValue<bool>("enable-auto-save", value ? false : state);
            skipPopup = false;
        });
    }
}