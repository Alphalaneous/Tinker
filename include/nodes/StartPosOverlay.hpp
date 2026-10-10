#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/ui/Button.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/StartPosObject.hpp>

using namespace geode::prelude;

namespace tinker::ui {

    class StartPosOverlay : public CCNodeRGBA {
    public:
        static StartPosOverlay* create();
        void setStartPos(StartPosObject* startPos);
        void updateOverlay();
        StartPosObject* getStartPos();
        StartPosObject* getLastStartPos();
        void checkDeletedObject(GameObject* object);
        void reset();

    protected:
        bool init() override;
        void updatePos(float dt);

        geode::Button* m_button;
        geode::NineSlice* m_background;
        Ref<StartPosObject> m_startPos;
        Ref<StartPosObject> m_lastStartPos;
    };

}