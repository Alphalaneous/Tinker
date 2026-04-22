#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>
#include <Geode/modify/GJRotationControl.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include "ModuleRegistry.hpp"
#include "Module.hpp"
#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

class $modify(MainEditorUI, EditorUI) {

    static inline EditorUI* s_editorUI = nullptr;

    struct Fields {
        std::vector<std::shared_ptr<EditorModuleBase>> m_modules;

        int m_lastObjectCount;
        bool m_wasPlatformer;
        std::set<FLAlertLayer*> m_activeAlerts;

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
    }

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;
        s_editorUI = this;

        auto fields = m_fields.self();

        for (const auto& createModule : ModuleRegistry<EditorModuleBase>::get()->m_modules) {
            auto module = createModule();
            module->m_editorLayer = m_editorLayer;
            module->m_editorUI = this;
            if (module->isEnabled()) {
                module->onEditor();
            }
            fields->m_modules.emplace_back(module);
        }
        updateButtons();

        schedule(schedule_selector(MainEditorUI::checkForChange));

        return true;
    }

    void checkForChange(float dt) {
		auto fields = m_fields.self();

		int last = fields->m_lastObjectCount;
		int cur = m_editorLayer->m_objectCount;

		if (last != cur) {
            forEachModule([this] (EditorModuleBase* module) {
                module->onObjectChange(m_editorLayer->getLastObjectX());
            });
		}
	
		fields->m_lastObjectCount = cur;

        bool wasPlatformer = fields->m_wasPlatformer;
        bool isPlatformer = m_editorLayer->m_levelSettings->m_platformerMode;

        if (wasPlatformer != isPlatformer) {
            forEachModule([&isPlatformer] (EditorModuleBase* module) {
                module->onGameTypeChange(isPlatformer);
            });
        }

        fields->m_wasPlatformer = isPlatformer;
    }

    void updateButtons() {
        EditorUI::updateButtons();
		auto fields = m_fields.self();

        forEachModule([] (EditorModuleBase* module) {
            module->onUpdateButtons();
        });
    }

    void forEachModule(geode::Function<void(EditorModuleBase*)> moduleCallback) {
        if (!moduleCallback) return;
		auto fields = m_fields.self();

        for (auto& module : fields->m_modules) {
            if (module->isEnabled()) {
                moduleCallback(module.get());
            }
        }
    }

    void deactivateScaleControl() {
        for (auto child : m_scaleControl->getChildrenExt()) {
            if (auto textInput = typeinfo_cast<geode::TextInput*>(child)) {
                textInput->defocus();
            }
        }
        EditorUI::deactivateScaleControl();
    }
    
    static MainEditorUI* get() {
        return static_cast<MainEditorUI*>(s_editorUI);
    }
};

class $modify(MainGJRotationControl, GJRotationControl) {
    void finishTouch() {
        GJRotationControl::finishTouch();
        for (auto child : getChildrenExt()) {
            if (auto textInput = typeinfo_cast<geode::TextInput*>(child)) {
                textInput->defocus();
            }
        }
    }
};

class $modify(MainSetGroupIDLayer, SetGroupIDLayer) {

    bool init(GameObject* obj, cocos2d::CCArray* objs) {
        if (!SetGroupIDLayer::init(obj, objs)) return false;

        MainEditorUI::get()->forEachModule([this, obj, objs] (EditorModuleBase* module) {
            module->onSetGroupIDLayer(this, obj, objs);
        });

        return true;
    }
};

class $modify(MainEditorPauseLayer, EditorPauseLayer) {

    struct Fields {
        ~Fields() {
            if (MainEditorUI::get()) {
                MainEditorUI::get()->forEachModule([this] (EditorModuleBase* module) {
                    module->m_pauseLayer = nullptr;
                });
            }
        }
    };

    bool init(LevelEditorLayer* layer) {
        if (!EditorPauseLayer::init(layer)) return false;
        if (getUserFlag("ignore"_spr)) return true;

        auto fields = m_fields.self();
        
        MainEditorUI::get()->forEachModule([this] (EditorModuleBase* module) {
            module->m_pauseLayer = this;
            module->onEditorPauseLayer(this);
        });

        auto guidelinesMenu = getChildByID("guidelines-menu");
        if (!guidelinesMenu) return true;

        auto spr = CircleButtonSprite::createWithSprite("tinker-hammer.png"_spr, 1.f, CircleBaseColor::Green, CircleBaseSize::Small);

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
        MainEditorUI::get()->forEachModule([this] (EditorModuleBase* module) {
            module->onSave();
        });
        EditorPauseLayer::saveLevel();
    }
};

$on_game(ModsLoaded) {
    static std::vector<std::shared_ptr<GlobalModuleBase>> modules;
    for (const auto& module : ModuleRegistry<GlobalModuleBase>::get()->m_modules) {
        modules.push_back(module());
    }
}
