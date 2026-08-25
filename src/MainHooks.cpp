#include "MainHooks.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include "modules/StatusBar.hpp"
#include "nodes/ShadowLabel.hpp"
#include "module/ModuleRegistry.hpp"
#include "modules/ScrollableObjects.hpp"
#include "modules/UIScaling.hpp"
#include "Events.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include "../../include/UIScaling.hpp"
#include "settings/SettingsPopup.hpp"
#include "utils/next-free/NextFreeProvider.hpp"
#include "utils/Utils.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>

bool MainLevelEditorLayer::init(GJGameLevel* level, bool noUI) {
    auto fields = m_fields.self();

    NextFreeProvider::get()->setEditorLayer(this);

    for (const auto& [k, v] : ModuleRegistry::get()->m_modules) {
        auto& data = ModuleRegistry::get()->getData(k);
        if (!data.isGlobal()) {
            fields->m_modules[k] = v();
        }
    }

    if (!LevelEditorLayer::init(level, noUI)) return false;

    return true;
}

StringMap<std::shared_ptr<ModuleBase>>* MainLevelEditorLayer::getModules() {
    return &m_fields->m_modules;
}

void MainLevelEditorLayer::forEachModule(geode::Function<void(ModuleBase*)> moduleCallback) {
    if (!moduleCallback) return;
    for (auto& [k, v] : *getModules()) {
        auto& data = ModuleRegistry::get()->getData(k);
        if (data.moduleEnabled()) {
            moduleCallback(v.get());
        }
    }
}

MainLevelEditorLayer* MainLevelEditorLayer::get() {
    return static_cast<MainLevelEditorLayer*>(LevelEditorLayer::get());
}

EditorUI* MainEditorUI::s_editorUI = nullptr;

MainEditorUI::Fields::~Fields() {
    s_editorUI = nullptr;
    for (auto& [k, v] : ModuleRegistry::get()->m_data) {
        if (v.isGlobal()) continue;
        for (auto hook : v.getHooks()) {
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

    for (const auto& [k, v] : *modules) {
        auto& data = ModuleRegistry::get()->getData(k);
        if (!data.moduleEnabled()) continue;
        
        data.onEditor();
    }
    for (const auto& [k, v] : ModuleRegistry::get()->m_modules) {
        auto& data = ModuleRegistry::get()->getData(k);
        if (!data.isGlobal() || !data.moduleEnabled()) continue;
        
        data.onEditor();
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

    editorLayer->schedule(schedule_selector(MainLevelEditorLayer::mainUpdate), 1.f / 60.f);

    runAction(CallFuncExt::create([this, editorLayer] {
        m_toolbarHeight = tinker::utils::getToolbarHeight(false);
        EditorZoomEvent().send(editorLayer->m_objectLayer->getScale());
    }));

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

void MainLevelEditorLayer::mainUpdate(float dt) {
    auto editor = MainEditorUI::get();
    auto fields = editor->m_fields.self();

    editor->checkObjectPlacement(fields);
    editor->checkPlatformerState(fields);
    editor->checkModifierState(fields);
    editor->checkEditorPosition(fields);
    editor->checkActiveObjectCount(fields);

    // hack to fix y positions of tabs being wrong for some people (???)
    editor->fixTabPositions();
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

void MainEditorUI::checkEditorPosition(MainEditorUI::Fields* fields) {
    auto last = fields->m_lastPosition;
    auto cur = m_editorLayer->m_objectLayer->getPosition();

    if (last != cur) {
        EditorMoveEvent().send(cur);
    }

    fields->m_lastPosition = cur;
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

void MainEditorUI::checkActiveObjectCount(MainEditorUI::Fields* fields) {
    auto last = fields->m_lastActiveObjectCount;
    auto cur = tinker::utils::getActiveObjectCount(m_editorLayer);

    if (last != cur) {
        ActiveObjectsChangedEvent().send(cur);
    }

    fields->m_lastActiveObjectCount = cur;
}

void MainEditorUI::fixTabPositions() {
    float toolbarOffset = 0.f;
    if (StatusBar::isEnabled()) {
        toolbarOffset = StatusBar::get()->m_toolbarOffset;
    } 
    for (auto tab : alpha::editor_tabs::getAllTabs().unwrapOrDefault()) {
        if (!tab) continue;
        if (tab->getID() == "all-objects-tab-bar"_spr) continue;

        float posY = tab->getScaledContentHeight() * tab->getAnchorPoint().y + toolbarOffset;
        if (tab->getScaledContentHeight() == 0) {
            posY = tinker::utils::getToolbarHeight(false) / 2.f;
        }
        tab->setPositionY(posY);
    }

    m_deleteMenu->setContentSize({0, 0});
    m_deleteMenu->setPositionY(tinker::utils::getToolbarHeight(false) / 2.f + toolbarOffset / 2.f);
}

void MainEditorUI::updateButtons() {
    auto toolbarHeight = m_toolbarHeight;
    m_toolbarHeight = tinker::utils::getToolbarHeight(false);
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

void MainEditorUI::enableButton_(CreateMenuItem* button) {
    auto sprite = static_cast<ButtonSprite*>(button->getNormalImage());
    if (sprite->m_subBGSprite) {
        sprite->m_subBGSprite->setColor({255, 255, 255});
    }
    else if (sprite->m_BGSprite) {
        sprite->m_BGSprite->setColor({255, 255, 255});
    }

    if (button->m_objectID < 0) return;

    auto object = static_cast<EffectGameObject*>(sprite->m_subSprite);
    if (object->m_classType == GameObjectClassType::Effect && object->isColorObject() && object->m_shouldPreview) {
        object->setObjectColor({200, 200, 255});
    }
    else {
        auto objectID = object->m_objectID;
        if (objectID == 918 || objectID == 919 || (objectID != 1584 && objectID != 2012) && object->getMainColorMode() != 1010) {
            object->setObjectColor({255, 255, 255});
        }
        else {
            object->setObjectColor({0, 0, 0});
        }
    }
    object->setChildColor({200, 200, 255});
}

void MainEditorUI::disableButton_(CreateMenuItem* button) {
    auto sprite = static_cast<ButtonSprite*>(button->getNormalImage());
    if (sprite->m_subBGSprite) {
        sprite->m_subBGSprite->setColor({127, 127, 127});
    }

    else if (sprite->m_BGSprite) {
        sprite->m_BGSprite->setColor({127, 127, 127});
    }
    if (button->m_objectID < 0) return;

    auto object = static_cast<EffectGameObject*>(sprite->m_subSprite);
    if (object->m_classType == GameObjectClassType::Effect && object->isColorObject() && object->m_shouldPreview) {
        object->setObjectColor({100, 100, 127});
    }
    else {
        object->setObjectColor({127, 127, 127});
    }
    object->setChildColor({100, 100, 127});
}

void MainEditorUI::updateCreateMenu(bool selectTab) {
    if (m_selectedMode != 2) {
        m_createButtonBar->setVisible(false);
        m_tabsMenu->setVisible(false);
        return;
    }

    if (m_selectedObjectIndex == 749) {
        m_selectedObjectIndex = 0;
    }

    m_createButtonBar->setVisible(true);
    m_tabsMenu->setVisible(true);

    for (auto item : m_createButtonArray->asExt<CreateMenuItem>()) {
        enableButton_(item);
    }

    for (auto item : m_customObjectButtonArray->asExt<CreateMenuItem>()) {
        enableButton_(item);
    }

    for (auto item : m_createButtonArray->asExt<CreateMenuItem>()) {
        if (item->m_objectID == m_selectedObjectIndex) {
            disableButton_(item);
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
            disableButton_(item);
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
    auto fields = m_fields.self();
    if (fields->m_moveEventLocked) return;
    ObjectMovedEvent().send();

    fields->m_moveEventLocked = true;
    runAction(CallFuncExt::create([fields] {
        fields->m_moveEventLocked = false;
    }));
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
        tinker::ui::SettingsPopup::create(false)->show();
    });
    tinkerBtn->setZOrder(1000);
    tinkerBtn->setID("tinker-settings"_spr);

    guidelinesMenu->addChild(tinkerBtn);

    guidelinesMenu->updateLayout();

    auto versionLabel = tinker::ui::ShadowLabel::create(fmt::format("Tinker: {}", Mod::get()->getVersion().toVString()), "chatFont.fnt");
    versionLabel->setAnchorPoint({1.f, 1.f});
    versionLabel->setOpacity(125);
    versionLabel->setScale(0.4f);
    versionLabel->setID("version-label"_spr);

    auto winSize = CCDirector::get()->getWinSize();

    versionLabel->setPosition(convertToNodeSpace({winSize.width - 2.f, winSize.height - 2.f}));

    addChild(versionLabel);

    return true;
}

void MainEditorPauseLayer::saveLevel() {
    LevelSavedEvent().send();
    EditorPauseLayer::saveLevel();
}

class $classModify(UnloadedEditLevelLayer, EditLevelLayer) {

    void modify() {
        if (tinker::utils::shouldLoadTinker()) return;

        auto editBtn = static_cast<CCMenuItemSpriteExtra*>(getChildByIDRecursive("edit-button"));
        if (!editBtn) return;

        tinker::utils::hijackButton(editBtn, [] (std::function<void(CCObject* sender)> orig, CCObject* sender) {
            createQuickPopup("Tinker Not Loaded!", "Tinker will not load with Multiplayer Edit enabled. I do not wish to recieve Tinker bug reports for issues caused by Multiplayer Edit, so I have chosen to make it not load.", "OK", nullptr, [orig, sender] (auto alert, bool selected) {
                orig(sender);
            });
        });
    }

};

$on_game(ModsLoaded) {
    if (!tinker::utils::shouldLoadTinker()) return;

    static std::vector<std::shared_ptr<ModuleBase>> modules;
    for (auto& [k, v] : ModuleRegistry::get()->m_modules) {
        auto& data = ModuleRegistry::get()->getData(k);
        if (data.isGlobal()) {
            modules.push_back(v());
        }
    }
}

$on_mod(Loaded) {
    UIScaleUpdated().listen([] (float scale, bool scaleToolbars, bool fullReload) {
        tinker::api::ui_scaling::UIScaleUpdated().send(scale, scaleToolbars, false);
    }).leak();

    PauseUIScaleUpdated().listen([] (EditorPauseLayer* pauseLayer, float scale) {
        tinker::api::ui_scaling::PauseUIScaleUpdated().send(scale);
    }).leak();
}