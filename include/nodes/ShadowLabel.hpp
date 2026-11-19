#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/ui/Label.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/utils/cocos.hpp>
#include <alphalaneous.alphas-ui-pack/nodes/RenderNode.hpp>

namespace tinker::ui {

class ShadowLabel : public cocos2d::CCNodeRGBA {
public:
    static ShadowLabel* create(geode::ZStringView text, geode::ZStringView font, float shadowDistance = 1.f);

    void setText(geode::ZStringView text);
    void validate();
    void setLimitLabelWidth(float width, float maxScale = 1.f, float minScale = 0.f);
    float getLabelScale();

protected:
    bool init(geode::ZStringView text, geode::ZStringView font, float shadowDistance);

    geode::Ref<cocos2d::CCNode> m_container = nullptr;
    geode::Label* m_mainLabel = nullptr;
    geode::Label* m_shadowLabel = nullptr;
    alpha::ui::RenderNode* m_render = nullptr;
    float m_shadowDistance = 1.f;
};

}