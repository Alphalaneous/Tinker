#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include "ModuleRegistry.hpp"
#include "Module.hpp"

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
            if (!module) continue;
            module->m_editorLayer = m_editorLayer;
            module->m_editorUI = this;
            module->onEditor();
            fields->m_modules.emplace_back(module);
        }

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
            moduleCallback(module.get());
        }
    }

    void addActiveAlert(FLAlertLayer* alert) {
		auto fields = m_fields.self();

        fields->m_activeAlerts.insert(alert);
    }

    void removeActiveAlert(FLAlertLayer* alert) {
		auto fields = m_fields.self();

        fields->m_activeAlerts.erase(alert);
    }

    void scrollWheel(float y, float x) {
		auto fields = m_fields.self();

        for (auto alert : fields->m_activeAlerts) {
            if (alert && alert->getParentByType<CCScene>() && nodeIsVisible(alert)) {
                return;
            }
        }

        EditorUI::scrollWheel(y, x);
    }

    static MainEditorUI* get() {
        return static_cast<MainEditorUI*>(s_editorUI);
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

    bool init(LevelEditorLayer* layer) {
        if (!EditorPauseLayer::init(layer)) return false;
        
        MainEditorUI::get()->forEachModule([this] (EditorModuleBase* module) {
            module->onEditorPauseLayer(this);
        });

        return true;
    }

    void saveLevel() {
        MainEditorUI::get()->forEachModule([this] (EditorModuleBase* module) {
            module->onSave();
        });
        EditorPauseLayer::saveLevel();
    }
};

class $baseModify(BlockingFLAlertLayer, FLAlertLayer) {
    struct Fields {
        FLAlertLayer* m_self;
        ~Fields() {
            auto editor = MainEditorUI::get();
            if (!editor) return;

            editor->removeActiveAlert(m_self);
        }
    };

    void modify() {
        auto editor = MainEditorUI::get();
        if (!editor) return;

        auto fields = m_fields.self();
        fields->m_self = this;
        editor->addActiveAlert(this);
    }
};

class $classModify(FieldsCCTouchDispatcher, CCTouchDispatcher) {
    struct Fields {
        int m_targetPrio;

        bool m_setPrio = false;
        void setPrio(int target) {
            if (!m_setPrio) {
                m_targetPrio = target;
                m_setPrio = true;
            }
        }
    };
};

class $modify(MainCCTouchDispatcher, CCTouchDispatcher) {
    void registerForcePrio(cocos2d::CCObject* obj, int value) {
        auto orig = m_pForcePrioDict->objectForKey(obj->m_uID);
        if (!orig) {
            auto fields = reinterpret_cast<FieldsCCTouchDispatcher*>(this)->m_fields.self();

            fields->setPrio(m_targetPrio);
            fields->m_targetPrio -= value;

            m_forcePrio += value;
            m_targetPrio = fields->m_targetPrio + (EditorUI::get() ? -1000 : 0);
            
            m_pForcePrioDict->setObject(CCInteger::create(value), obj->m_uID);
        }
    }

    void unregisterForcePrio(cocos2d::CCObject* obj) {
        auto orig = static_cast<CCInteger*>(m_pForcePrioDict->objectForKey(obj->m_uID));
        if (orig) {
            auto fields = reinterpret_cast<FieldsCCTouchDispatcher*>(this)->m_fields.self();

            m_forcePrio -= orig->getValue();
            fields->m_targetPrio += orig->getValue();
            m_targetPrio = fields->m_targetPrio;
        }
        m_pForcePrioDict->removeObjectForKey(obj->m_uID);
    }
};

$on_game(ModsLoaded) {
    static std::vector<std::shared_ptr<GlobalModuleBase>> modules;
    for (const auto& module : ModuleRegistry<GlobalModuleBase>::get()->m_modules) {
        modules.push_back(module());
    }
}
