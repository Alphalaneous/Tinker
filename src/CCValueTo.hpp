#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

template<typename T>
class CCValueTo : public CCActionInterval {
public:
    using UpdateFn = geode::Function<void(float, T start, T end, T&)>;

    static CCValueTo* create(float duration, T start, T end, UpdateFn update) {
        CCValueTo<T>* ret = new CCValueTo<T>();
        if (ret->initWithDuration(duration, start, end, std::move(update))) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool initWithDuration(float duration, T start, T end, UpdateFn update) {
        if (!CCActionInterval::initWithDuration(duration)) return false;
        m_start = start;
        m_end = end;
        m_update = std::move(update);
        return true;
    }

    virtual void update(float t) override {
        if (m_update) m_update(t, m_start, m_end, m_value);
    }

protected:
    T m_start = {};
    T m_end = {};
    T m_value = {};
    UpdateFn m_update;
};