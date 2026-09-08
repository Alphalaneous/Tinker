#pragma once

#include "module/Module.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace alpha::prelude;

namespace tinker::ui {

class PlacePreviewNode : public CCNode, public TouchDelegate {
public:
    static PlacePreviewNode* create();

    void setObject(int id);
    void updateOpacity(float opacity);
protected:
    bool init() override;
    void update(float dt) override;

    void showAtPos(const CCPoint& pos);
    void updateObjectColor();

    bool mouseEntered(TouchEvent* touch) override;
    void mouseMoved(TouchEvent* touch) override;

    void onEnter() override;
    void onExit() override;

    void applyOffset(GameObject* object);
    void removeOffset(GameObject* object);

    CCPoint m_objectPos;
    bool m_objectVisible;
    Ref<GameObject> m_hoverObject;
    Ref<RenderNode> m_objectRender;

    bool m_wasDuplicated;
};

}

class $module(PlacePreview) {
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void onEditor();
    bool inValidTab();

    tinker::ui::PlacePreviewNode* m_hover;
    CCNode* m_feedbackContainer;
    bool m_callbacksSet;
};

class $modify(PPEditorUI, EditorUI) {
    $registerHooks(PlacePreview)

    void onCreateButton(cocos2d::CCObject* sender);
};
