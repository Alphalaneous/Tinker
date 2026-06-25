#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class CCCallbackAction : public CCActionInterval {
public:
    static CCCallbackAction* create(CCFiniteTimeAction* action, geode::Function<void(CCNode*)> cb);

    virtual void startWithTarget(CCNode* target);
    virtual void update(float t);
    virtual void stop();

protected:

    Ref<CCFiniteTimeAction> innerAction = nullptr;
    geode::Function<void(CCNode*)> callback;
};