#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tinker::ui {

template<class T>
class ValueToggler : public CCMenuItemSpriteExtra {
public:
    static ValueToggler* create(geode::Function<void(float)> callback, std::vector<T> values, size_t defaultIdx) {
        auto ret = new ValueToggler();
        if (ret->init(std::move(callback), std::move(values), defaultIdx)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
protected:
    bool init(geode::Function<void(float)> callback, std::vector<T> values, size_t defaultIdx) {
        m_callback = std::move(callback);
        m_values = std::move(values);
        m_idx = defaultIdx;

        auto slice = NineSlice::create("button-empty.png"_spr);

        int precision = 0;
        if constexpr (std::is_floating_point_v<T>) {
            slice->setContentSize({35, 20});
            precision = 2;
        }
        else {
            slice->setContentSize({20, 20});
        }

        slice->setScaleMultiplier(0.6f);
        slice->setCascadeOpacityEnabled(true);

        if (!CCMenuItemSpriteExtra::init(slice, nullptr, nullptr, nullptr)) return false;
        setCascadeOpacityEnabled(true);

        m_label = CCLabelBMFont::create(numToString<T>(m_values[m_idx], precision).c_str(), "bigFont.fnt");
        m_label->setPosition(slice->getContentSize() / 2.f);
        m_label->setScale(0.25f);

        slice->addChild(m_label);

        setOpacity(127);
        setEnabled(false);
        if (m_callback) m_callback(m_values[m_idx]);

        return true;
    }

    void updateLabel() {
        int precision = 0;
        if constexpr (std::is_floating_point_v<T>) {
            precision = 2;
        }
        m_label->setString(numToString<T>(m_values[m_idx], precision).c_str());
    }

    void activate() override {
        CCMenuItemSpriteExtra::activate();
        if (CCKeyboardDispatcher::get()->getAltKeyPressed()) {
            m_idx--;
            if (m_idx < 0) {
                m_idx = m_values.size() - 1;
            }
        }
        else {
            m_idx++;
            if (m_idx >= m_values.size()) {
                m_idx = 0;
            }
        }

        updateLabel();
        if (m_callback) m_callback(m_values[m_idx]);
    }

    int m_idx = 0;
    CCLabelBMFont* m_label;
    geode::Function<void(float)> m_callback;
    std::vector<T> m_values;
};

}