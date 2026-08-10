#include "utils/DestroyListener.hpp"

DestroyListener::~DestroyListener() {
    if (m_destroyCallback) m_destroyCallback();
}

DestroyListener* DestroyListener::create(geode::Function<void()> destroyCallback) {
    auto ret = new DestroyListener();
    ret->m_destroyCallback = std::move(destroyCallback);
    ret->autorelease();
    return ret;
}