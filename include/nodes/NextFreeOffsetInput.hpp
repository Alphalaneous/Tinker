#pragma once

#include <Geode/Geode.hpp>
#include "utils/next-free/NextFreeProvider.hpp"

using namespace geode::prelude;

namespace tinker::ui {

template <class T>
requires std::derived_from<T, NextFreeSource>
class NextFreeOffsetInput : public CCNode {
public:
    static NextFreeOffsetInput* create() {
        auto ret = new NextFreeOffsetInput();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
    
protected:
    TextInput* m_input;
    T m_source;

    bool init() {
        if (!CCNode::init()) return false;
    
        m_source = T();
        m_source.setEditor(LevelEditorLayer::get());

        setAnchorPoint({0.5f, 0.5f});
        setContentSize({40.f, 30.f});

        auto label = CCLabelBMFont::create("Offset", "goldFont.fnt");
        label->setScale(0.35f);
        addChildAtPosition(label, Anchor::Top, {0.f, -5.f});
        
        m_input = TextInput::create(60.f, "Num");
        m_input->setCommonFilter(CommonFilter::Uint);
        m_input->setScale(0.5f);

        auto offset = m_source.getOffset();

        if (offset > m_source.getMin()) {
            m_input->setString(numToString(offset));
        }
        m_input->setCallback([this](auto str) {
            if (auto value = numFromString<int>(str)) {
                m_source.setOffset(std::clamp(value.unwrap(), m_source.getMin(), m_source.getMax()));
            }
            else {
                m_source.setOffset(m_source.getMin());
            }
        });
        addChildAtPosition(m_input, Anchor::Bottom, {0.f, 10.f});

        return true;
    }
};

}