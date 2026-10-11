#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/ui/Label.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/utils/cocos.hpp>
#include <alphalaneous.alphas-ui-pack/include/nodes/RenderNode.hpp>

namespace tinker::ui {

class ShadowLabel : public cocos2d::CCNodeRGBA {
public:
    static ShadowLabel* create(geode::ZStringView text, geode::ZStringView font);

    void setText(geode::ZStringView text);
protected:
    bool init(geode::ZStringView text, geode::ZStringView font);

    geode::Ref<cocos2d::CCNode> m_container = nullptr;
    geode::Label* m_mainLabel = nullptr;
    geode::Label* m_shadowLabel = nullptr;
    alpha::ui::RenderNode* m_render = nullptr;
};

}