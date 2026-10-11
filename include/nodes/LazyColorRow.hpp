#pragma once

#include <Geode/cocos/menu_nodes/CCMenu.h>
#include <Geode/utils/function.hpp>

namespace tinker::ui {

class LazyColorRow : public cocos2d::CCMenu {
public:
    static LazyColorRow* create(geode::Function<void(LazyColorRow* self)> callback, int min, int max);
    bool init(geode::Function<void(LazyColorRow* self)> callback, int min, int max);

    int getMin();
    int getMax();

    void load();
protected:
    geode::Function<void(LazyColorRow* self)> m_callback = nullptr;
    bool m_loaded = false;
    int m_min = 0;
    int m_max = 0;
};

}