#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/ButtonSprite.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include "module/ModuleBase.hpp"
#include "module/ModuleRegistry.hpp"
#include "module/Module.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include "Events.hpp"
#include "modules/ScrollableObjects.hpp"
#include "modules/UIScaling.hpp"

using namespace geode::prelude;

class $modify(MainLevelEditorLayer, LevelEditorLayer) {

    struct Fields {
        std::vector<std::shared_ptr<EditorModuleBase>> m_modules;
    };

    bool init(GJGameLevel* level, bool noUI) {

        auto fields = m_fields.self();

        for (const auto& createModule : ModuleRegistry<EditorModuleBase>::get()->m_modules) {
            fields->m_modules.push_back(createModule());
        }

        if (!LevelEditorLayer::init(level, noUI)) return false;

        return true;
    }

    std::vector<std::shared_ptr<EditorModuleBase>>* getModules() {
        return &m_fields->m_modules;
    }

    void forEachModule(geode::Function<void(EditorModuleBase*)> moduleCallback) {
        if (!moduleCallback) return;
        for (auto& module : *getModules()) {
            if (module->isEnabled()) {
                moduleCallback(module.get());
            }
        }
    }

    static MainLevelEditorLayer* get() {
        return static_cast<MainLevelEditorLayer*>(LevelEditorLayer::get());
    }
};

class $modify(MainEditorUI, EditorUI) {

    static inline EditorUI* s_editorUI = nullptr;

    struct Fields {
        int m_lastObjectCount;
        bool m_wasPlatformer;

        ~Fields() {
            s_editorUI = nullptr;
            for (const auto& [k, v] : ModuleRegistry<EditorModuleBase>::get()->m_hooks) {
                for (auto hook : v) {
                    (void) hook->disable();
                }
            }
            EditorExitEvent().send();
        };
    };

    static void onModify(auto& self) {
        (void) self.setHookPriority("EditorUI::init", Priority::Late);
        (void) self.setHookPriorityPre("EditorUI::scrollWheel", Priority::EarlyPre - 1);
        (void) self.setHookPriority("EditorUI::updateCreateMenu", Priority::Replace);
    }

    bool init(LevelEditorLayer* editorLayer) {
        auto fields = m_fields.self();

        if (!EditorUI::init(editorLayer)) return false;
        s_editorUI = this;

        auto modules = static_cast<MainLevelEditorLayer*>(editorLayer)->getModules();

        for (const auto& module : *modules) {
            module->m_editorLayer = m_editorLayer;
            module->m_editorUI = this;
            if (module->isEnabled()) {
                module->onEditor();
            }
        }

        if (ScrollableObjects::isEnabled()) {
            ScrollableObjects::get()->setLoadBars();
        }

        if (UIScaling::isEnabled()) {
            UIScaling::get()->setScaling(UIScaling::getUIScale(), UIScaling::shouldScaleToolbar(), UIScaling::getSetting<bool, "top-align">(), true);
        }

        EditorEnterEvent().send(this);
        updateButtons();

        schedule(schedule_selector(MainEditorUI::checkForChange));

        return true;
    }

    void checkForChange(float dt) {
		auto fields = m_fields.self();

		int last = fields->m_lastObjectCount;
		int cur = m_editorLayer->m_objectCount;

		if (last != cur) {
            ObjectChangeEvent().send(m_editorLayer->getLastObjectX());
		}
	
		fields->m_lastObjectCount = cur;

        bool wasPlatformer = fields->m_wasPlatformer;
        bool isPlatformer = m_editorLayer->m_levelSettings->m_platformerMode;

        if (wasPlatformer != isPlatformer) {
            LevelTypeChangedEvent().send(isPlatformer);
        }

        fields->m_wasPlatformer = isPlatformer;
    }

    void updateButtons() {
        EditorUI::updateButtons();
        UpdateButtonsEvent().send();
    }

    void deactivateScaleControl() {
        for (auto child : m_scaleControl->getChildrenExt()) {
            if (auto textInput = typeinfo_cast<geode::TextInput*>(child)) {
                textInput->defocus();
            }
        }
        EditorUI::deactivateScaleControl();
    }

    void updateCreateMenu(bool selectTab) {
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
    
    static MainEditorUI* get() {
        return static_cast<MainEditorUI*>(s_editorUI);
    }
};

class $modify(MainSetGroupIDLayer, SetGroupIDLayer) {

    bool init(GameObject* obj, cocos2d::CCArray* objs) {
        if (!SetGroupIDLayer::init(obj, objs)) return false;

        SetGroupIDLayerOpenedEvent().send(this, obj, objs);

        return true;
    }
};

class $modify(MainEditorPauseLayer, EditorPauseLayer) {

    struct Fields {
        bool m_wasIgnored = false;
        ~Fields() {
            if (MainEditorUI::get()) {
                auto editorLayer = MainLevelEditorLayer::get();
                if (editorLayer) {
                    editorLayer->forEachModule([this] (EditorModuleBase* module) {
                        module->m_pauseLayer = nullptr;
                    });
                    
                    if (!m_wasIgnored) {
                        EditorUnpausedEvent().send();
                    }
                }
            }
        }
    };

    bool init(LevelEditorLayer* layer) {
        if (!EditorPauseLayer::init(layer)) return false;
        auto fields = m_fields.self();

        if (getUserFlag("ignore"_spr) || !EditorUI::get()) {
            fields->m_wasIgnored = true;
            return true;
        }

        MainLevelEditorLayer::get()->forEachModule([this] (EditorModuleBase* module) {
            module->m_pauseLayer = this;
        });
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

    void saveLevel() {
        LevelSavedEvent().send();
        EditorPauseLayer::saveLevel();
    }
};

$on_game(ModsLoaded) {
    static std::vector<std::shared_ptr<GlobalModuleBase>> modules;
    for (const auto& module : ModuleRegistry<GlobalModuleBase>::get()->m_modules) {
        modules.push_back(module());
    }
}
