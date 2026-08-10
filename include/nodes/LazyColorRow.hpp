#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class LazyColorRow : public CCMenu {
public:
    static LazyColorRow* create(geode::Function<void(LazyColorRow* self)> callback, int min, int max);
    bool init(geode::Function<void(LazyColorRow* self)> callback, int min, int max);

    int getMin();
    int getMax();

    void load();
protected:
    geode::Function<void(LazyColorRow* self)> m_callback;
    bool m_loaded;
    int m_min;
    int m_max;
};

}