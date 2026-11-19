#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/ui/Button.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/StartPosObject.hpp>
#include <Geode/ui/Toggler.hpp>

namespace tinker::ui {

class StartPosOverlay : public cocos2d::CCNodeRGBA {
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

    geode::Toggler* m_activeStartPosToggle = nullptr;
    geode::NineSlice* m_background = nullptr;
    geode::Ref<StartPosObject> m_startPos = nullptr;
    geode::Ref<StartPosObject> m_lastStartPos = nullptr;
};

}