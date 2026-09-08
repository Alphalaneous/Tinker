#include "modules/Gizmos/Gizmos.hpp"
#include "InputsHandler.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>
#include <Geode/utils/base64.hpp>
#include "modules/Gizmos/ColorChannelGizmo.hpp"
#include "modules/Gizmos/TestGizmo.hpp"
#include "nodes/LabelledToggle.hpp"
#include "utils/Constants.hpp"

namespace tinker::ui {

ScheduleNode* ScheduleNode::create(geode::Function<void(float dt)> callback) {
    auto ret = new ScheduleNode();
    if (ret->init(std::move(callback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ScheduleNode::init(geode::Function<void(float dt)> callback) {
    m_callback = std::move(callback);
    scheduleUpdate();

    return true;
}

void ScheduleNode::update(float dt) {
    if (!m_callback) return;
    m_callback(dt);
}

}

void Gizmos::setGizmoID() {
    // I only do this so just in case someone else does it or even rob in a future update, I always just grab whatever is available
    auto& keys = ObjectToolbox::sharedState()->m_allKeys;
    if (m_unusedID != -1) {
        keys.erase(m_unusedID);
    }
    
    int available = -1;

    for (int i = 1; i < 100000; i++) {
        if (!keys.contains(i)) {
            available = i;
            break;
        }
    }

    if (available != -1) {
        m_unusedID = available;
        keys.emplace(m_unusedID, "geode.loader/settings.png");
    }
}

int Gizmos::getUnusedID() {
    return m_unusedID;
}

void Gizmos::selectGizmo(ZStringView ID) {
    m_useSelectedGizmo = true;
    m_selectedGizmo = ID;
}

void Gizmos::unselectGizmo() {
    m_useSelectedGizmo = false;
    m_selectedGizmo = "";
}

bool Gizmos::useSelectedGizmo() {
    return m_useSelectedGizmo;
}

ZStringView Gizmos::getSelectedGizmo() {
    return m_selectedGizmo;
}

std::shared_ptr<GizmoInterface> Gizmos::interfaceForID(ZStringView ID) {
    auto iter = m_gizmoForID.find(ID);
    if (iter != m_gizmoForID.end()) {
        return iter->second();
    }
    return nullptr;
}

void Gizmos::setupGizmos() {
    registerGizmo<TestGizmo>("test");
    registerGizmo<ColorChannelGizmo>("color-channel-gizmo");
}

void GizmoInterface::setObject(Gizmo* object) {
    m_object = object;
}

const CCSize& GizmoInterface::getEditPopupSize() {
    return m_editPopupSize;
}

ZStringView GizmoInterface::getEditPopupTitle() {
    return m_editPopupTitle;
}

Gizmo* GizmoInterface::getObject() {
    return m_object;
}

Gizmo* Gizmo::create() {
    auto ret = new Gizmo();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

Gizmo* Gizmo::createWithID(ZStringView ID) {
    auto ret = new Gizmo();
    if (ret->initWithID(ID)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool Gizmo::init() {
    if (!CCSpritePlus::init()) return false;
    commonSetup();
    m_bUnkBool2 = true;

    m_addToNodeContainer = true;
    m_objectType = static_cast<GameObjectType>(-1);
    m_objectID = Gizmos::get()->getUnusedID();
    m_hasCustomChild = true;
    m_baseColor->m_defaultColorID = tinker::constants::color_channels::White;
    
    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);
    
    m_scheduleNode = tinker::ui::ScheduleNode::create([this] (float dt) {
        if (m_gizmoInterface) {
            m_gizmoInterface->update(dt);
        }
    });
    m_scheduleNode->setID("schedule-node"_spr);
    addChild(m_scheduleNode);

    // another unexplainable leak unless I do this
    addOnExitCallback([this] {
        removeAllChildren();
    });

    return true;
}

bool Gizmo::initWithID(ZStringView ID) {
    if (!Gizmo::init()) return false;

    m_gizmoID = ID;
    setGizmoInterface(Gizmos::get()->interfaceForID(m_gizmoID));

    if (m_gizmoInterface) {
        m_gizmoInterface->loadData(std::nullopt);
        setChildNode(m_gizmoInterface->setup());
    }

    return true;
}

void Gizmo::customObjectSetup(gd::vector<gd::string>& values, gd::vector<void*>& exists) {
    matjson::Value propertyJson;

    GameObject::customObjectSetup(values, exists);

    for (int i = 0; i < values.size() && i < exists.size(); i++) {
        if (exists[i] && i == 400) {
            m_gizmoID = utils::base64::decodeString(values[i]).unwrapOrDefault();
        }
        if (exists[i] && i == 401) {
            auto decoded = utils::base64::decodeString(values[i]).unwrapOrDefault();
            propertyJson = matjson::parse(decoded).unwrapOrDefault();

            m_interactiveDuringPlaytest = propertyJson["is-interactive-during-playtest"].asBool().unwrapOrDefault();
            m_visibleDuringPlaytest = propertyJson["is-visible-during-playtest"].asBool().unwrapOrDefault();
        }
    }

    setGizmoInterface(Gizmos::get()->interfaceForID(m_gizmoID));

    if (m_gizmoInterface) {
        m_gizmoInterface->loadData(propertyJson);
        setChildNode(m_gizmoInterface->setup());
    }
}

gd::string Gizmo::getSaveString(GJBaseGameLayer* layer) {
    auto saveString = GameObject::getSaveString(layer);

    geode::utils::StringBuffer buffer;

    buffer.append(saveString);
    buffer.append(",400,{}", utils::base64::encode(m_gizmoID));

    matjson::Value saved;
    if (m_gizmoInterface) {
        saved = m_gizmoInterface->saveData();
    }
    saved["is-interactive-during-playtest"] = isInteractiveDuringPlaytest();
    saved["is-visible-during-playtest"] = isVisibleDuringPlaytest();

    buffer.append(",401,{}", utils::base64::encode(saved.dump(0)));

    return buffer.str();
}

void Gizmo::onEditObject() {
    if (m_gizmoInterface) {
        m_gizmoInterface->onEditObject();
    }
}

void Gizmo::setGizmoInterface(std::shared_ptr<GizmoInterface> gizmoInterface) {
    m_gizmoInterface = gizmoInterface;
    m_gizmoInterface->setObject(this);
}

void Gizmo::setCanEditObject(bool canEdit) {
    m_canEditObject = canEdit;
}

bool Gizmo::canEditObject() {
    return m_canEditObject;
}

void Gizmo::updateBounds() {
    if (!m_childNode) return;

    if (!m_renderNode) {
        m_renderNode = alpha::ui::RenderNode::create(m_childNode, true);
        addChild(m_renderNode);
    }
    else {
        m_renderNode->resetFBO();
    }
    
    m_childNode->setPosition(m_renderNode->getContentSize() / 2.f);

    setContentSize(m_renderNode->getContentSize());
    m_renderNode->setPosition(getContentSize() / 2.f);

    m_width = m_renderNode->getContentWidth();
    m_height = m_renderNode->getContentWidth();

    updateOrientedBox();

    m_renderNode->updateDisplayedColor(getDisplayedColor());
    m_renderNode->updateDisplayedOpacity(getDisplayedOpacity());
}

void Gizmo::setRotation(float rotation) {
    GameObject::setRotation(rotation);
    if (m_gizmoInterface) {
        m_gizmoInterface->onRotate(rotation);
    }
}

void Gizmo::setChildNode(CCNode* node) {
    m_childNode = node;
    updateBounds();
}

CCNode* Gizmo::getChildNode() {
    return m_childNode;
}

void Gizmo::setGizmoID(ZStringView ID) {
    m_gizmoID = ID;
}

ZStringView Gizmo::getGizmoID() {
    return m_gizmoID;
}

void Gizmo::setVisibleDuringPlaytest(bool visible) {
    m_visibleDuringPlaytest = visible;
}

bool Gizmo::isVisibleDuringPlaytest() {
    return m_visibleDuringPlaytest;
}

void Gizmo::setInteractiveDuringPlaytest(bool interactive) {
    m_interactiveDuringPlaytest = interactive;
}

bool Gizmo::isInteractiveDuringPlaytest() {
    return m_interactiveDuringPlaytest;
}

void Gizmo::setOpacity(unsigned char opacity) {
    if (!m_visibleDuringPlaytest && LevelEditorLayer::get()->m_playbackMode == PlaybackMode::Playing) {
        opacity = 0;
    }
    GameObject::setOpacity(opacity);
}

GameObject* GGameObject::createWithKey(const int key) {
    if (key == Gizmos::get()->getUnusedID()) {
        if (Gizmos::get()->useSelectedGizmo()) {
            return Gizmo::createWithID(Gizmos::get()->getSelectedGizmo());
        }
        else {
            return Gizmo::create();
        }
    } 
    else {
        return GameObject::createWithKey(key);
    }
}

void GCCTouchDispatcher::removeDelegate(CCTouchDelegate* delegate) {
    Gizmos::get()->m_removedDelegates.insert(delegate); 
    CCTouchDispatcher::removeDelegate(delegate);
}

// evil I know
void GCCTouchDispatcher::touches(CCSet* touches, CCEvent* event, unsigned int index) {        
    auto handlersToReadd = CCArray::create();

    auto handlers = m_pTargetedHandlers->asExt<CCTargetedTouchHandler>();
    for (int i = handlers.size() - 1; i >= 0; i--) {
        auto node = typeinfo_cast<CCNode*>(handlers[i]->getDelegate());
        if (!node) continue;

        auto gizmo = node->getParentByType<Gizmo>();
        if (!gizmo) continue;

        bool blockTouch = false;
        bool playing = LevelEditorLayer::get()->m_playbackMode == PlaybackMode::Playing;

        auto firstTouch = static_cast<CCTouch*>(touches->anyObject());
        if (firstTouch) {
            auto mainPos = InputEditorUI::getTouchLocation(firstTouch);
            if (mainPos.y <= tinker::utils::getToolbarHeight()) {
                blockTouch = true;
            }
        }

        auto editor = LevelEditorLayer::get();
        if (!editor->validGroup(gizmo, false) || blockTouch || (playing && !gizmo->isInteractiveDuringPlaytest())) {
            for (auto claimed : *handlers[i]->getClaimedTouches()->m_pSet) {
                handlers[i]->getDelegate()->ccTouchCancelled(static_cast<CCTouch*>(claimed), event);
            }
            handlers[i]->getClaimedTouches()->m_pSet->clear();
            handlersToReadd->addObject(handlers[i]);
        }
    }
    
    m_pTargetedHandlers->removeObjectsInArray(handlersToReadd);

    CCTouchDispatcher::touches(touches, event, index);

    for (auto handler : handlersToReadd->asExt<CCTouchHandler>()) {
        if (!Gizmos::get()->m_removedDelegates.contains(handler->getDelegate())) {
            m_pTargetedHandlers->addObject(handler);
        }
    }
    Gizmos::get()->m_removedDelegates.clear();
}

void GEditorPauseLayer::saveLevel() {
    auto oldObjects = m_editorLayer->m_objects;
    std::vector<Ref<Gizmo>> gizmos;

    m_editorLayer->m_objects = oldObjects->shallowCopy();

    auto objects = m_editorLayer->m_objects->asExt<GameObject>();

    for (int i = objects.size() - 1; i >= 0; i--) {
        if (auto gizmo = typeinfo_cast<Gizmo*>(objects[i])) {
            gizmos.push_back(gizmo);
            m_editorLayer->m_objects->removeObjectAtIndex(i);
        }
    }

    std::vector<std::string> arr;
    for (const auto& gizmo : gizmos) {
        auto origID = gizmo->m_objectID;
        gizmo->m_objectID = 0;
        auto save = gizmo->getSaveString(m_editorLayer);
        auto saveView = std::string_view(save);

        arr.push_back(std::string(saveView.substr(4)));
        gizmo->m_objectID = origID;
    }

    alpha::level_storage::setSavedValue(m_editorLayer, "gizmos", arr);

    EditorPauseLayer::saveLevel();

    m_editorLayer->m_objects = oldObjects;
}

void GLevelEditorLayer::createObjectsFromSetup(gd::string& setup) {
    auto saved = alpha::level_storage::getSavedValue<matjson::Value>(this, "gizmos");
    auto arrRes = saved.asArray();
    StringBuffer<> strBuffer;
    
    if (arrRes) {
        auto arr = arrRes.unwrap();
        for (const auto& val : arr) {
            auto strRes = val.asString();
            if (strRes) {
                strBuffer.append("1,{},{};", Gizmos::get()->getUnusedID(), strRes.unwrap());
            }
        }
    }

    // android sucks
    gd::string newSetup = fmt::format("{}{}", setup, strBuffer);
    LevelEditorLayer::createObjectsFromSetup(newSetup);
}

bool GLevelEditorLayer::init(GJGameLevel* level, bool noUI) {
    Gizmos::get()->setupGizmos();
    Gizmos::get()->setGizmoID();
    return LevelEditorLayer::init(level, noUI);
}

void GEditorUI::placeGizmo() {
    auto winSize = CCDirector::get()->getWinSize();
    auto camCenter = m_editorLayer->m_objectLayer->convertToNodeSpace(winSize / 2.f + CCPoint{0.f, tinker::utils::getToolbarHeight() / 2.f});

    auto oldClick = m_clickAtPosition;
    
    m_clickAtPosition = camCenter;
    Gizmos::get()->selectGizmo("color-channel-gizmo");
    onCreateObject(Gizmos::get()->getUnusedID());
    Gizmos::get()->unselectGizmo();
    m_clickAtPosition = oldClick;
}

bool GEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) return false;

    addEventListener(MouseInputEvent(), [this] (MouseInputData& data) {
        if (data.button == MouseInputData::Button::Button4 && data.action == MouseInputData::Action::Press) {
            placeGizmo();
        }
    });

    addEventListener(UpdateButtonsEvent(), [this] {
        bool selected = m_selectedObject || m_selectedObjects->count() != 0;
        bool canEdit = selected;

        std::set<Gizmo*> gizmos;

        if (m_selectedObject) {
            auto gizmo = typeinfo_cast<Gizmo*>(m_selectedObject);
            if (gizmo) {
                canEdit = gizmo->canEditObject();
                gizmos.insert(gizmo);
            }
        }
        else {
            for (auto obj : m_selectedObjects->asExt<GameObject*>()) {
                auto gizmo = typeinfo_cast<Gizmo*>(obj);
                if (gizmo) {
                    gizmos.insert(gizmo);
                }
            }
        }
        
        bool matchesSize = !m_selectedObject && m_selectedObjects->count() == gizmos.size();

        if (m_selectedObject && gizmos.size() == 1 || matchesSize) {
            m_editSpecialBtn->setColor(selected ? ccColor3B{255, 255, 255} : ccColor3B{166, 166, 166});
            m_editSpecialBtn->setOpacity(selected ? 255 : 175);
            m_editSpecialBtn->setEnabled(selected);
            m_editSpecialBtn->m_animationEnabled = selected;
        }

        if (matchesSize && gizmos.size() > 1) {
            canEdit = false;
        }

        m_editObjectBtn->setColor(canEdit ? ccColor3B{255, 255, 255} : ccColor3B{166, 166, 166});
        m_editObjectBtn->setOpacity(canEdit ? 255 : 175);
        m_editObjectBtn->setEnabled(canEdit);
        m_editObjectBtn->m_animationEnabled = canEdit;
    });

    return true;
}

void GEditorUI::editObject(cocos2d::CCObject* sender) {
    std::set<Ref<Gizmo>> gizmos;

    if (m_selectedObject) {
        auto gizmo = typeinfo_cast<Gizmo*>(m_selectedObject);
        if (gizmo) {
            gizmo->onEditObject();
            return;
        }
    }

    for (auto obj : m_selectedObjects->asExt<GameObject*>()) {
        auto gizmo = typeinfo_cast<Gizmo*>(obj);
        if (gizmo) {
            gizmos.insert(gizmo);
            m_selectedObjects->removeObject(gizmo);
        }
    }
    
    EditorUI::editObject(sender);

    for (auto& gizmo : gizmos) {
        m_selectedObjects->addObject(gizmo);
    }
}

void GEditorUI::editObjectSpecial(int type) {
    std::set<Gizmo*> gizmos;

    if (m_selectedObject) {
        auto gizmo = typeinfo_cast<Gizmo*>(m_selectedObject);
        if (gizmo) {
            gizmos.insert(gizmo);
        }
    }
    else {
        for (auto obj : m_selectedObjects->asExt<GameObject*>()) {
            auto gizmo = typeinfo_cast<Gizmo*>(obj);
            if (gizmo) {
                gizmos.insert(gizmo);
            }
        }
    }

    if (m_selectedObject && gizmos.size() == 1 || (!m_selectedObject && m_selectedObjects->count() == gizmos.size())) {
        tinker::ui::EditSpecialGizmoPopup::create(gizmos)->show();
        return;
    }
    
    EditorUI::editObjectSpecial(type);
}

namespace tinker::ui {

EditSpecialGizmoPopup* EditSpecialGizmoPopup::create(const std::set<Gizmo*>& gizmos) {
    auto ret = new EditSpecialGizmoPopup();
    if (ret->init(gizmos)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool EditSpecialGizmoPopup::init(const std::set<Gizmo*>& gizmos) {
    if (!geode::Popup::init({240.f, 145.f})) return false;
    setTitle("Edit Gizmo Settings", "bigFont.fnt", 0.5f, 15.f);
    setOpacity(150);

    m_gizmos = gizmos;

    m_noElasticity = true;
    m_closeBtn->removeFromParent();
    m_mainLayer->setID("main-layer"_spr);
    m_title->setID("title-label"_spr);
    m_bgSprite->setID("background"_spr);

    auto okSpr = ButtonSprite::create("OK");
    okSpr->setID("ok-button-sprite"_spr);

    auto okBtn = CCMenuItemExt::createSpriteExtra(okSpr, [this] (auto sender) {
        onClose(sender);
    });
    okBtn->setID("ok-button"_spr);

    m_buttonMenu->addChildAtPosition(okBtn, Anchor::Bottom, {0.f, 8.f + okBtn->getContentHeight() / 2.f}, false);
    m_buttonMenu->setID("main-menu"_spr);

    auto togglerContainer = CCNode::create();
    togglerContainer->setAnchorPoint({0.5f, 0.5f});
    togglerContainer->ignoreAnchorPointForPosition(false);
    togglerContainer->setLayout(ColumnLayout::create()
        ->setAutoGrowAxis(0.f)
        ->setGap(10.f)
        ->setAxisReverse(true)
    );
    togglerContainer->setID("toggler-container"_spr);

    auto visibleInPlaytest = tinker::ui::LabelledToggle::create("Visible in Playtest", [this] (bool state) {
        forEachGizmo([this, state] (Gizmo* gizmo) {
            gizmo->setVisibleDuringPlaytest(state);
            return false;
        });
    });
    visibleInPlaytest->setID("visible-in-playtest-toggle"_spr);
    
    bool allVisible = (*m_gizmos.begin())->isVisibleDuringPlaytest();
    forEachGizmo([this, allVisible] (Gizmo* gizmo) mutable {
        if (!gizmo->isVisibleDuringPlaytest()) {
            allVisible = false;
        }
        return !allVisible;
    });

    visibleInPlaytest->toggle(allVisible);
    togglerContainer->addChild(visibleInPlaytest);

    auto interactiveInPlaytest = tinker::ui::LabelledToggle::create("Interactive in Playtest", [this] (bool state) {
        forEachGizmo([this, state] (Gizmo* gizmo) {
            gizmo->setInteractiveDuringPlaytest(state);
            return false;
        });
    });
    interactiveInPlaytest->setID("interactive-in-playtest-toggle"_spr);
    
    bool allInteractive = (*m_gizmos.begin())->isInteractiveDuringPlaytest();
    forEachGizmo([this, allInteractive] (Gizmo* gizmo) mutable {
        if (!gizmo->isVisibleDuringPlaytest()) {
            allInteractive = false;
        }
        return !allInteractive;
    });

    interactiveInPlaytest->toggle(allInteractive);
    togglerContainer->addChild(interactiveInPlaytest);

    togglerContainer->updateLayout();

    m_mainLayer->addChildAtPosition(togglerContainer, Anchor::Center, {0.f, 6.f}, false);

    /*auto infoBtn = InfoAlertButton::create("Help", "", 0.7f);
    m_buttonMenu->addChildAtPosition(infoBtn, Anchor::TopLeft, {infoBtn->getContentWidth() / 2.f + 7.5f, -infoBtn->getContentHeight() / 2.f - 7.5f}, false);*/

    return true;
}

void EditSpecialGizmoPopup::forEachGizmo(geode::Function<bool(Gizmo* gizmo)> callback) {
    for (auto gizmo : m_gizmos) {
        if (callback(gizmo)) break;
    }
}

}