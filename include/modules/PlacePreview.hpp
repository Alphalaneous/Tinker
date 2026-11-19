#pragma once

#include "module/Module.hpp"
#include <alphalaneous.alphas-ui-pack/API.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace alpha::prelude;

namespace tinker::ui {

class PlacePreviewNode : public cocos2d::CCNode, public TouchDelegate {
public:
    static PlacePreviewNode* create();

    void setObject(int id);
    void updateOpacity(float opacity);
protected:
    bool init() override;
    void update(float dt) override;

    void showAtPos(const cocos2d::CCPoint& pos, const cocos2d::CCPoint& world);
    void updateObjectColor();

    bool hoverEntered(Hover* touch) override;
    void hoverMoved(Hover* touch) override;

    void onEnter() override;
    void onExit() override;

    void applyOffset(GameObject* object);
    void removeOffset(GameObject* object);

    cocos2d::CCPoint m_objectPos{};
    bool m_objectVisible = false;
    geode::Ref<GameObject> m_hoverObject = nullptr;
    geode::Ref<RenderNode> m_objectRender = nullptr;

    bool m_wasDuplicated = false;
};

}

class $module(PlacePreview) {
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void onEditor();
    bool inValidTab();

    tinker::ui::PlacePreviewNode* m_hover = nullptr;
    cocos2d::CCNode* m_feedbackContainer = nullptr;
    bool m_callbacksSet = false;
};

class $modify(PPEditorUI, EditorUI) {
    $registerHooks(PlacePreview)

    void updateCreateMenu(bool selectTab);
    void onCreateButton(cocos2d::CCObject* sender);
};
