#pragma once

#include <Geode/modify/StartPosLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/ui/Label.hpp>
#include <Geode/ui/Button.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/Toggler.hpp>
#include <alphalaneous.alphas-ui-pack/nodes/RenderNode.hpp>
#include <alphalaneous.editortab_api/api/Tab.hpp>
#include "module/Module.hpp"
#include "nodes/StartPosOverlay.hpp"

namespace tinker::ui {

class StartPosContainer;

}

class $module(StartPosSwitcher) {
    enum class PlaytestStart {
        Farthest = 0,
        Latest = 1,
        Switcher = 2,
        NearCamera = 3
    };

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void onEditor();
    int indexForStartPos(StartPosObject* object);
    StartPosObject* startPosForIndex(int index);
    void addStartPos(StartPosObject* object);
    void removeStartPos(StartPosObject* object);
    void switchLive();

    void playFrom(StartPosObject* object);

    StartPosObject* nearestStartPos(cocos2d::CCPoint center, const std::vector<geode::Ref<StartPosObject>>& positions);
    StartPosObject* farthestStartPos(const std::vector<geode::Ref<StartPosObject>>& positions);
    void sortStartPositions();

    void updateOverlay();

    std::vector<geode::Ref<StartPosObject>> m_startPosObjects{};
    std::vector<geode::Ref<StartPosObject>> m_startPosObjectsUnsorted{};

    geode::Ref<StartPosObject> m_cameraStartPos = nullptr;
    unsigned int m_cameraDirty = 0;

    tinker::ui::StartPosContainer* m_startPosContainer = nullptr;
    tinker::ui::StartPosContainer* m_liveStartPosContainer = nullptr;

    geode::Ref<tinker::ui::StartPosOverlay> m_overlay = nullptr;
    alpha::editor_tabs::Tab* m_tab = nullptr;

    StartPosObject* m_tempStartPos = nullptr;

    bool m_switched = false;
};


namespace tinker::ui {

class StartPosContainer : public cocos2d::CCNode, public StartPosBrowserDelegate {
public:
    static StartPosContainer* create(bool live);

    void updateStartPos(int idx);
    void updateUI();
    void nextStartPos();
    void prevStartPos();
    void setStartPos(StartPosObject* startPos);
    StartPosObject* getStartPos();
    void startPosSelected(StartPosBrowser* p0, int p1, StartPosObject* p2);
    void showArrows(bool show);
    void tempShow();
    void show();
    void hide();

protected:
    bool init(bool live);

    void addToggle(std::string text, geode::ZStringView id, StartPosSwitcher::PlaytestStart type, CCNode* parent);

    bool m_live = false;
    geode::Button* m_editButton = nullptr;
    geode::Button* m_findButton = nullptr;
    geode::Button* m_createButton = nullptr;
    geode::Button* m_prevButton = nullptr;
    geode::Button* m_nextButton = nullptr;

    geode::Label* m_currentLabel = nullptr;
    geode::Label* m_titleLabel = nullptr;

    cocos2d::CCNode* m_switcherContainer = nullptr;
    cocos2d::CCNode* m_controls = nullptr;

    alpha::ui::RenderNode* m_render = nullptr;
    geode::Ref<StartPosObject> m_startPos = nullptr;
};

class CreateStartPosPopup : public geode::Popup {
public:
    static CreateStartPosPopup* create();
protected:
    bool init();
};

};

class $modify(SPSStartPosLayer, StartPosLayer) {
    $registerHooks(StartPosSwitcher)

    class ClosedEvent : public geode::Event<ClosedEvent, bool(StartPosLayer* self)> {
    public:
        using Event::Event;
    };

    void onClose(cocos2d::CCObject* sender);
};

class $modify(SPSLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(StartPosSwitcher)

    void onPlaytest();
    void onStopPlaytest();
    void addSpecial(GameObject* object);
    void removeSpecial(GameObject* object);
};

class $modify(SPSEditorUI, EditorUI) {
    $registerHooks(StartPosSwitcher)

    struct Fields {
        bool m_dontToggle = false;
    };

    bool isStartPosTab();

    void onPlaytest(cocos2d::CCObject* sender);
    void toggleMode(cocos2d::CCObject* sender);
    void clickOnPosition(cocos2d::CCPoint position);
    
    void deselectAll();
    void deselectObject(GameObject* object);
    void selectObject(GameObject* object, bool ignoreFilter);
    void selectObjects(cocos2d::CCArray* objects, bool ignoreFilter);

    bool handleAction(int action, bool down, double timestamp);
};