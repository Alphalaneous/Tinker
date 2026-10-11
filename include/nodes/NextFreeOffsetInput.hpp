#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/ui/Label.hpp>
#include <Geode/ui/Layout.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <concepts>
#include <Geode/utils/general.hpp>
#include "utils/next-free/NextFreeProvider.hpp"

namespace tinker::ui {

template <class T>
requires std::derived_from<T, NextFreeSource>
class NextFreeOffsetInput : public cocos2d::CCNode {
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
    geode::TextInput* m_input;
    T m_source;

    bool init() {
        if (!cocos2d::CCNode::init()) return false;
    
        m_source = T();
        m_source.setEditor(LevelEditorLayer::get());

        setAnchorPoint({0.5f, 0.5f});
        setContentSize({40.f, 30.f});

        auto label = geode::Label::create("Offset", "goldFont.fnt");
        label->setScale(0.35f);
        addChildAtPosition(label, geode::Anchor::Top, {0.f, -5.f});
        
        m_input = geode::TextInput::create(60.f, "Num");
        m_input->setCommonFilter(geode::CommonFilter::Uint);
        m_input->setScale(0.5f);

        auto offset = m_source.getOffset();

        if (offset > m_source.getMin()) {
            m_input->setString(geode::utils::numToString(offset));
        }
        m_input->setCallback([this](auto str) {
            if (auto value = geode::utils::numFromString<int>(str)) {
                m_source.setOffset(std::clamp(value.unwrap(), m_source.getMin(), m_source.getMax()));
            }
            else {
                m_source.setOffset(m_source.getMin());
            }
        });
        addChildAtPosition(m_input, geode::Anchor::Bottom, {0.f, 10.f});

        return true;
    }
};

}