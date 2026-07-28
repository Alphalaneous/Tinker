#include "MainHooks.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include "module/ModuleRegistry.hpp"
#include "modules/ScrollableObjects.hpp"
#include "modules/UIScaling.hpp"
#include "Events.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include "../../include/UIScaling.hpp"
#include "settings/SettingsPopup.hpp"
#include "utils/NextFree/NextFreeProvider.hpp"
#include "utils/Utils.hpp"

bool MainLevelEditorLayer::init(GJGameLevel* level, bool noUI) {
    auto fields = m_fields.self();

    NextFreeProvider::get()->setEditorLayer(this);

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
        UIScaling::get()->setScaling(true);
    }
    else {
        UIScaleUpdated().send(1, true, true);
    }

    EditorEnterEvent().send(this);
    updateButtons();

    addEventListener(UpdateObjectLabel(), [this] () {
        float scale = 1.f;
        if (UIScaling::isEnabled()) {
            scale = UIScaling::get()->m_scale;
        }
        float x = std::max(tinker::utils::getFurthestLeft(m_objectInfoLabel, 150.f * scale), UIScaling::getSafeOffset().x);
        float offset = 10.f * scale;
        m_objectInfoLabel->setPositionX(x + offset);
    });

    schedule(schedule_selector(MainEditorUI::mainUpdate));

    return true;
}

void MainEditorUI::showUI(bool show) {
    EditorUI::showUI(show);
    m_fields->m_uiVisible = show;

    ShowUIEvent().send(show);
    UpdateObjectLabel().send();
}

void MainEditorUI::updateObjectInfoLabel() {
    EditorUI::updateObjectInfoLabel();
    UpdateObjectLabel().send();
}

bool MainEditorUI::isUIVisible() {
    return m_fields->m_uiVisible;
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
        if (tab->getID() == "all-objects-tab-bar"_spr) continue;

        float posY = tab->getScaledContentHeight() * tab->getAnchorPoint().y;
        if (tab->getScaledContentHeight() == 0) {
            posY = tinker::utils::getToolbarHeight() / 2.f;
        }
        tab->setPositionY(posY);
    }

    m_deleteMenu->setContentSize({0, 0});
    m_deleteMenu->setPositionY(tinker::utils::getToolbarHeight() / 2.f);
}

void MainEditorUI::updateButtons() {
    auto toolbarHeight = m_toolbarHeight;
    m_toolbarHeight = tinker::utils::getToolbarHeight();
    EditorUI::updateButtons();
    m_toolbarHeight = toolbarHeight;
    UpdateButtonsEvent().send();
    UpdateObjectLabel().send();
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

void MainEditorUI::updateZoom(float zoom) {
    EditorUI::updateZoom(zoom);
    EditorZoomEvent().send(zoom);
}

void MainEditorUI::moveObject(GameObject* object, cocos2d::CCPoint offset) {
    EditorUI::moveObject(object, offset);
    ObjectMovedEvent().send();
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

    /*auto spr2 = CCSprite::create("hammer-button.png"_spr);
    auto tinkerBtn2 = CCMenuItemExt::createSpriteExtra(spr2, [] (auto sender) {
        SettingsPopup::create()->show();
    });
    tinkerBtn2->setZOrder(1000);
    tinkerBtn2->setColor({255, 0, 255});
    tinkerBtn2->setID("tinker-settings-2"_spr);

    guidelinesMenu->addChild(tinkerBtn2);*/

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

$on_mod(Loaded) {
    UIScaleUpdated().listen([] (float scale, bool scaleToolbars, bool fullReload) {
        tinker::api::ui_scaling::UIScaleUpdated().send(scale, scaleToolbars, false);
    }).leak();
}