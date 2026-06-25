#include "CCCallbackAction.hpp"

CCCallbackAction* CCCallbackAction::create(CCFiniteTimeAction* action, geode::Function<void(CCNode*)> cb) {
    auto p = new CCCallbackAction();
    if (p->initWithDuration(action->getDuration())) {
        p->innerAction = action;
        p->callback = std::move(cb);
        p->autorelease();
        return p;
    }

    delete p;
    return nullptr;
}

void CCCallbackAction::startWithTarget(CCNode* target) {
    CCActionInterval::startWithTarget(target);
    innerAction->startWithTarget(target);
}

void CCCallbackAction::update(float t) {
    if (innerAction) {
        innerAction->update(t);
    }

    if (callback && m_pTarget) {
        callback(m_pTarget);
    }
}

void CCCallbackAction::stop() {
    if (innerAction) {
        innerAction->stop();
    }

    CCActionInterval::stop();
}