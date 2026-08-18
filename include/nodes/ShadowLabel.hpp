#pragma once

#include <alphalaneous.alphas-ui-pack/include/nodes/RenderNode.hpp>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class ShadowLabel : public CCNodeRGBA {
public:
    static ShadowLabel* create(ZStringView text, ZStringView font);

    void setText(ZStringView text);
protected:
    bool init(ZStringView text, ZStringView font);

    Ref<CCNode> m_container;
    geode::Label* m_mainLabel;
    geode::Label* m_shadowLabel;
    alpha::ui::RenderNode* m_render;
};

}