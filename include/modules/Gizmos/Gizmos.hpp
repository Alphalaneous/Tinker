#pragma once

#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <Geode/utils/cocos.hpp>
#include "module/Module.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/CCTouchDispatcher.hpp>

class Gizmo;

class GizmoInterface {
public:
    virtual cocos2d::CCNode* setup() { return nullptr; }
    virtual void onEditObject() {}
    virtual void loadData(std::optional<const matjson::Value> data) {}
    virtual matjson::Value saveData() { return {}; }
    virtual void onRotate(float degrees) {}
    virtual void update(float dt) {}
    virtual void setupEditPopup(geode::Popup* popup) {}
    virtual void onEditPopupClose(geode::Popup* popup) {}

    void setObject(Gizmo* object);
    Gizmo* getObject();

    const cocos2d::CCSize& getEditPopupSize();
    geode::ZStringView getEditPopupTitle();

protected:
    Gizmo* m_object = nullptr;

    cocos2d::CCSize m_editPopupSize{};
    std::string m_editPopupTitle = "";
};

namespace tinker::ui {

class ScheduleNode : public cocos2d::CCNodeRGBA {
public:
    static ScheduleNode* create(geode::Function<void(float dt)> callback);
protected:
    bool init(geode::Function<void(float dt)> callback);
    void update(float dt) override;

    geode::Function<void(float dt)> m_callback = nullptr;
};

template<class T>
class EditGizmoPopup : public geode::Popup {
public:
    static EditGizmoPopup<T>* create(T* gizmo) {
        auto ret = new EditGizmoPopup<T>();
        if (ret->init(gizmo)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    virtual void setup() {}

protected:
    bool init(T* gizmo) {
        if (!geode::Popup::init(gizmo->getEditPopupSize())) return false;
        setTitle(gizmo->getEditPopupTitle(), "bigFont.fnt", 0.5f, 15.f);
        setOpacity(150);

        m_gizmo = gizmo;
        m_noElasticity = true;
        m_closeBtn->removeFromParent();
        m_mainLayer->setID("main-layer"_spr);
        m_title->setID("title-label"_spr);
        m_bgSprite->setID("background"_spr);

        auto okSpr = ButtonSprite::create("OK");
        okSpr->setID("ok-button-sprite"_spr);

        auto okBtn = geode::cocos::CCMenuItemExt::createSpriteExtra(okSpr, [this] (auto sender) {
            onClose(sender);
        });
        okBtn->setID("ok-button"_spr);

        m_buttonMenu->addChildAtPosition(okBtn, geode::Anchor::Bottom, {0.f, 8.f + okBtn->getContentHeight() / 2.f}, false);
        m_buttonMenu->setID("main-menu"_spr);

        addEventListener(listenForClose(), [this] {
            m_gizmo->onEditPopupClose(this);
        });

        gizmo->setupEditPopup(this);

        return true;
    }

    T* m_gizmo = nullptr;
};

class EditSpecialGizmoPopup : public geode::Popup {
public:
    static EditSpecialGizmoPopup* create(const std::set<Gizmo*>& gizmos);
protected:
    bool init(const std::set<Gizmo*>& gizmos);
    void forEachGizmo(geode::Function<bool(Gizmo* gizmo)> callback);

    std::set<Gizmo*> m_gizmos{};
};

}

class Gizmo : public GameObject {
public:
    static Gizmo* create();
    static Gizmo* createWithID(geode::ZStringView ID);
    void setChildNode(cocos2d::CCNode* node);
    void updateBounds();
    cocos2d::CCNode* getChildNode();

    void setGizmoID(geode::ZStringView ID);
    geode::ZStringView getGizmoID();

    void setCanEditObject(bool canEdit);
    bool canEditObject();

    void setVisibleDuringPlaytest(bool visible);
    bool isVisibleDuringPlaytest();

    void setInteractiveDuringPlaytest(bool interactive);
    bool isInteractiveDuringPlaytest();

    void setGizmoInterface(std::shared_ptr<GizmoInterface> gizmoInterface);

    void setOpacity(unsigned char opacity) override; 

    void onEditObject();

    void customObjectSetup(gd::vector<gd::string>& values, gd::vector<void*>& exists) override;
    gd::string getSaveString(GJBaseGameLayer* layer) override;

    void setRotation(float rotation) override;
protected:
    bool initWithID(geode::ZStringView ID);
    bool init() override;

    bool m_visibleDuringPlaytest = false;
    bool m_interactiveDuringPlaytest = false;
    bool m_canEditObject = false;
    std::string m_gizmoID = "";
    tinker::ui::ScheduleNode* m_scheduleNode = nullptr;
    cocos2d::CCNode* m_childNode = nullptr;
    std::shared_ptr<GizmoInterface> m_gizmoInterface = nullptr;
    alpha::ui::RenderNode* m_renderNode = nullptr;
};

class $module(Gizmos, true) {
    int m_unusedID = 0;
    geode::utils::StringMap<geode::Function<std::shared_ptr<GizmoInterface>()>> m_gizmoForID{};
    bool m_useSelectedGizmo = false;
    std::string m_selectedGizmo = "";
    std::set<cocos2d::CCTouchDelegate*> m_removedDelegates{};

    template <class T>
    void registerGizmo(geode::ZStringView ID) {
        m_gizmoForID[ID] = [] {
            return std::make_shared<T>();
        };
    }

    std::shared_ptr<GizmoInterface> interfaceForID(geode::ZStringView ID);

    void selectGizmo(geode::ZStringView ID);
    void unselectGizmo();
    geode::ZStringView getSelectedGizmo();
    bool useSelectedGizmo();

    void setupGizmos();
    void setGizmoID();
    int getUnusedID();
};

class $modify(GGameObject, GameObject) {
    $registerHooks(Gizmos)

    static GameObject* createWithKey(const int key);
};

class $modify(GCCTouchDispatcher, cocos2d::CCTouchDispatcher) {
    $registerHooks(Gizmos, true)

    static void _onModify(auto& self) {
        (void) self.setHookPriorityPre("CCTouchDispatcher::touches", geode::Priority::VeryEarlyPre);
    }

    void removeDelegate(cocos2d::CCTouchDelegate* delegate);
    void addTargetedDelegate(cocos2d::CCTouchDelegate* delegate, int priority, bool swallowsTouches);
    void touches(cocos2d::CCSet* touches, cocos2d::CCEvent* event, unsigned int index);
};

class $modify(GEditorPauseLayer, EditorPauseLayer) {
    $registerHooks(Gizmos)

    void saveLevel();
};

class $modify(GLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(Gizmos)

    void createObjectsFromSetup(gd::string& setup);
    bool init(GJGameLevel* level, bool noUI);
};

class $modify(GEditorUI, EditorUI) {
    $registerHooks(Gizmos)

    void placeGizmo();
    bool init(LevelEditorLayer* editorLayer);
    void editObject(cocos2d::CCObject* sender);
    void editObjectSpecial(int type);
};