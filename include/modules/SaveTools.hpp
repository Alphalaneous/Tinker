#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/GManager.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <hjfod.gmd-api/include/GMD.hpp>
#include <alphalaneous.alphas-ui-pack/include/nodes/RenderNode.hpp>

// heavily based off of BetterEdit to allow transferring and handling everything much easier

namespace tinker::ui {

class SaveAlert : public CCNodeRGBA {
public:
    static SaveAlert* create();

    void setText(ZStringView text);
    void setAlertColor(const ccColor3B& color);
    void show();
    void hide();
protected:
    bool init() override;

    CCSprite* m_saveIcon;
    geode::NineSlice* m_container;
    geode::Label* m_label;
    alpha::ui::RenderNode* m_render;
    std::string m_text;
};

}

class $module(SaveTools, true) {

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void onEditor();

    static std::filesystem::path getQuickSaveDir();
    static std::filesystem::path getAutoSaveDir();

    static void transferBetterEdit();
    static void restoreData(const std::filesystem::path& path);

    void createAutoSave();
    void setCreateAutoSave(bool create);
    bool isCreatingAutoSave();

    void skipLLMSave(bool skip);
    bool isSkipLLMSave();

    bool m_creatingAutoSave;
    bool m_skipLLMSave;

    tinker::utils::ScopedHookToggle<"hjfod.betteredit", SaveTools> m_toggledHooks = {
        "GManager::save",
        "EditorPauseLayer::saveLevel",
        "EditorPauseLayer::FLAlert_Clicked",
        "EditorPauseLayer::onExitNoSave"
    };
};

class $modify(STGManager, GManager) {
    $registerHooks(SaveTools)

    void save();
};

class $modify(STEditorPauseLayer, EditorPauseLayer) {
    $registerHooks(SaveTools)

    void saveLevel();
    void onExitNoSave(CCObject* sender);
};

class $modify(STLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(SaveTools)

    struct Fields {
        std::chrono::milliseconds m_lastAutosaveTime = std::chrono::milliseconds(0);
        Ref<tinker::ui::SaveAlert> m_saveAlert;
    };

    void autoSaveUpdate(float dt);
};