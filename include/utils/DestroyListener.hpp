#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DestroyListener : public CCObject {
public:
    virtual ~DestroyListener();

    static DestroyListener* create(geode::Function<void()> destroyCallback);
protected:
    geode::Function<void()> m_destroyCallback;
};