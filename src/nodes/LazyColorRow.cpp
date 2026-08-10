#include "nodes/LazyColorRow.hpp"

namespace tinker::ui {

LazyColorRow* LazyColorRow::create(geode::Function<void(LazyColorRow* self)> callback, int min, int max) {
    auto ret = new LazyColorRow();
    if (ret->init(std::move(callback), min, max)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool LazyColorRow::init(geode::Function<void(LazyColorRow* self)> callback, int min, int max) {
    if (!CCMenu::init()) return false;
    m_callback = std::move(callback);
    m_min = min;
    m_max = max;

    return true;
}

int LazyColorRow::getMin() {
    return m_min;
}

int LazyColorRow::getMax() {
    return m_max;
}

void LazyColorRow::load() {
    if (m_loaded) return;
    if (m_callback) m_callback(this);
    m_loaded = true;
}

}