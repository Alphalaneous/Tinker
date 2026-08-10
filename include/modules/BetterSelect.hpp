#pragma once

#include "module/Module.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

using namespace alpha::prelude;

namespace tinker::ui {

class ObjectSelectContainer : public CCNode {
public:
    static ObjectSelectContainer* create(CCArray* objects);

    void shiftObject(bool forward);
protected:

    bool init(CCArray* objects);

    int m_index;

    Ref<CCArray> m_objects;

    geode::NineSlice* m_objectsBG;
    AdvancedScrollLayer* m_scrollLayer;
    std::unordered_map<Ref<GameObject>, Ref<CCSprite>> m_objectSprites;

    CCSprite* m_selectDot;
};

class HoverObjectNode : public CCNode, public TouchDelegate {
public:
    static HoverObjectNode* create();
    bool hoveringObjects();

protected:

    bool init() override;
    bool mouseEntered(TouchEvent* touch) override;
    void mouseMoved(TouchEvent* touch) override;

    void onEnter() override;
    void onExit() override;

    void onHoverObjects(const CCPoint& pos);

    void showObjectList();
    void removeObjectList();

    void shiftObject(bool forward);

    ObjectSelectContainer* m_activeSelectContainer;
    Ref<CCArray> m_lastObjects;
    CCPoint m_lastPos;
    bool m_active;
};

}

class $module(BetterSelect) {
    void onEditor();
    bool hoveringObjects();

    tinker::ui::HoverObjectNode* m_hover;
};