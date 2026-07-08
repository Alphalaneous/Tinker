#include "MainHooks.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include "module/ModuleRegistry.hpp"
#include "modules/ScrollableObjects.hpp"
#include "modules/UIScaling.hpp"
#include "Events.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include "../../include/UIScaling.hpp"

bool MainLevelEditorLayer::init(GJGameLevel* level, bool noUI) {
    auto fields = m_fields.self();

    for (const auto& createModule : ModuleRegistry<EditorModuleBase>::get()->m_modules) {
        fields->m_modules.push_back(createModule());
    }

    if (!LevelEditorLayer::init(level, noUI)) return false;

    return true;
}

std::vector<std::shared_ptr<EditorModuleBase>>* MainLevelEditorLayer::getModules() {
    return &m_fields->m_modules;
}

void MainLevelEditorLayer::forEachModule(geode::Function<void(EditorModuleBase*)> moduleCallback) {
    if (!moduleCallback) return;
    for (auto& module : *getModules()) {
        if (module->moduleEnabled()) {
            moduleCallback(module.get());
        }
    }
}

MainLevelEditorLayer* MainLevelEditorLayer::get() {
    return static_cast<MainLevelEditorLayer*>(LevelEditorLayer::get());
}

EditorUI* MainEditorUI::s_editorUI = nullptr;

MainEditorUI::Fields::~Fields() {
    s_editorUI = nullptr;
    for (const auto& [k, v] : ModuleRegistry<EditorModuleBase>::get()->m_hooks) {
        for (auto hook : v) {
            (void) hook->disable();
        }
    }
    EditorExitEvent().send();
};

bool MainEditorUI::init(LevelEditorLayer* editorLayer) {
    auto fields = m_fields.self();

    if (!EditorUI::init(editorLayer)) return false;
    s_editorUI = this;

    auto modules = static_cast<MainLevelEditorLayer*>(editorLayer)->getModules();

    for (const auto& module : *modules) {
        module->m_editorLayer = m_editorLayer;
        module->m_editorUI = this;
        if (module->moduleEnabled()) {
            module->onEditor();
        }
    }

    if (ScrollableObjects::isEnabled()) {
        ScrollableObjects::get()->setLoadBars();
    }

    if (UIScaling::isEnabled()) {
        UIScaling::get()->setScaling(UIScaling::getUIScale(), UIScaling::shouldScaleToolbar(), UIScaling::getSetting<bool, "top-align">(), true);
    }
    else {
        tinker::api::ui_scaling::UIScaleUpdated().send(1, true, false);
    }

    EditorEnterEvent().send(this);
    updateButtons();

    schedule(schedule_selector(MainEditorUI::mainUpdate));

    return true;
}

void MainEditorUI::showUI(bool show) {
    EditorUI::showUI(show);
    ShowUIEvent().send(show);
}

void MainEditorUI::mainUpdate(float dt) {
    auto fields = m_fields.self();

    checkObjectPlacement(fields);
    checkPlatformerState(fields);
    checkModifierState(fields);

    // hack to fix y positions of tabs being wrong for some people (???)
    fixTabPositions();
}

void MainEditorUI::checkPlatformerState(MainEditorUI::Fields* fields) {
    bool wasPlatformer = fields->m_wasPlatformer;
    bool isPlatformer = m_editorLayer->m_levelSettings->m_platformerMode;

    if (wasPlatformer != isPlatformer) {
        LevelTypeChangedEvent().send(isPlatformer);
    }

    fields->m_wasPlatformer = isPlatformer;
}

void MainEditorUI::checkObjectPlacement(MainEditorUI::Fields* fields) {
    int last = fields->m_lastObjectCount;
    int cur = m_editorLayer->m_objectCount;

    if (last != cur) {
        ObjectChangeEvent().send(m_editorLayer->getLastObjectX());
    }

    fields->m_lastObjectCount = cur;
}

void MainEditorUI::checkModifierState(MainEditorUI::Fields* fields) {
    auto dispatcher = CCKeyboardDispatcher::get();

    KeyboardModifier mods;

    if (dispatcher->getShiftKeyPressed()) mods |= KeyboardModifier::Shift;
    if (dispatcher->getControlKeyPressed()) mods |= KeyboardModifier::Control;
    if (dispatcher->getAltKeyPressed()) mods |= KeyboardModifier::Alt;

    if (fields->m_lastModifier != mods) {
        ModifierEvent().send(mods, fields->m_lastModifier);
    }
    fields->m_lastModifier = mods;
}

void MainEditorUI::fixTabPositions() {
    for (auto tab : alpha::editor_tabs::getAllTabs().unwrapOrDefault()) {
        if (!tab) continue;

        float posY = tab->getContentHeight() * tab->getAnchorPoint().y;
        if (tab->getContentHeight() == 0) {
            posY = m_toolbarHeight / 2;
        }
        tab->setPositionY(posY);
    }
}

void MainEditorUI::updateButtons() {
    EditorUI::updateButtons();
    UpdateButtonsEvent().send();
}

void MainEditorUI::deactivateScaleControl() {
    for (auto child : m_scaleControl->getChildrenExt()) {
        if (auto textInput = typeinfo_cast<geode::TextInput*>(child)) {
            textInput->defocus();
        }
    }
    EditorUI::deactivateScaleControl();
}

void MainEditorUI::updateCreateMenu(bool selectTab) {
    if (m_selectedMode != 2) {
        m_createButtonBar->setVisible(false);
        m_tabsMenu->setVisible(false);
        return;
    }

    m_createButtonBar->setVisible(true);
    m_tabsMenu->setVisible(true);

    for (auto item : m_createButtonArray->asExt<CreateMenuItem>()) {
        enableButton(item);
    }

    for (auto item : m_customObjectButtonArray->asExt<CreateMenuItem>()) {
        enableButton(item);
    }

    for (auto item : m_createButtonArray->asExt<CreateMenuItem>()) {
        if (item->m_objectID == m_selectedObjectIndex) {
            disableButton(item);
            if (!selectTab) {
                return;
            }
            selectBuildTab(item->m_tabIndex);
            m_createButtonBar->goToPage(item->m_pageIndex);
            return;
        }
    }

    for (auto item : m_customObjectButtonArray->asExt<CreateMenuItem>()) {
        if (item->m_objectID == m_selectedObjectIndex) {
            disableButton(item);
            if (!selectTab) {
                return;
            }
            selectBuildTab(item->m_tabIndex);
            m_createButtonBar->goToPage(item->m_pageIndex);
            return;
        }
    }
}

MainEditorUI* MainEditorUI::get() {
    return static_cast<MainEditorUI*>(s_editorUI);
}

bool MainSetGroupIDLayer::init(GameObject* obj, cocos2d::CCArray* objs) {
    if (!SetGroupIDLayer::init(obj, objs)) return false;

    SetGroupIDLayerOpenedEvent().send(this, obj, objs);

    return true;
}

EditorPauseLayer* MainEditorPauseLayer::s_editorPauseLayer = nullptr;

MainEditorPauseLayer::Fields::~Fields() {
    s_editorPauseLayer = nullptr;
    auto editorLayer = MainLevelEditorLayer::get();
    if (editorLayer) {
        if (!m_wasIgnored) {
            EditorUnpausedEvent().send();
        }
    }
}

EditorPauseLayer* MainEditorPauseLayer::get() {
    return s_editorPauseLayer;
}

bool MainEditorPauseLayer::init(LevelEditorLayer* layer) {
    if (!EditorPauseLayer::init(layer)) return false;
    auto fields = m_fields.self();

    if (getUserFlag("ignore"_spr) || !EditorUI::get()) {
        fields->m_wasIgnored = true;
        return true;
    }

    s_editorPauseLayer = this;
    EditorPausedEvent().send(this);

    auto guidelinesMenu = getChildByID("guidelines-menu");
    if (!guidelinesMenu) return true;

    auto spr = CCSprite::create("hammer-button.png"_spr);

    auto tinkerBtn = CCMenuItemExt::createSpriteExtra(spr, [] (auto sender) {
        geode::openSettingsPopup(Mod::get());
    });
    tinkerBtn->setZOrder(1000);
    tinkerBtn->setID("tinker-settings"_spr);

    guidelinesMenu->addChild(tinkerBtn);
    guidelinesMenu->updateLayout();

    return true;
}

void MainEditorPauseLayer::saveLevel() {
    LevelSavedEvent().send();
    EditorPauseLayer::saveLevel();
}

$on_game(ModsLoaded) {
    static std::vector<std::shared_ptr<GlobalModuleBase>> modules;
    for (const auto& module : ModuleRegistry<GlobalModuleBase>::get()->m_modules) {
        modules.push_back(module());
    }
}
