#pragma once

#include <Geode/cocos/actions/CCAction.h>
#include <Geode/cocos/actions/CCActionInterval.h>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/function.hpp>

using namespace geode::prelude;

class CCCallbackAction : public CCActionInterval {
public:
    static CCCallbackAction* create(CCFiniteTimeAction* action, geode::Function<void(CCNode*)> cb);

    virtual void startWithTarget(CCNode* target);
    virtual void update(float t);
    virtual void stop();

protected:

    Ref<CCFiniteTimeAction> innerAction = nullptr;
    geode::Function<void(CCNode*)> callback = nullptr;
};