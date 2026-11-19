#pragma once

#include <Geode/utils/ZStringView.hpp>
#include <alphalaneous.alphas-ui-pack/nodes/RenderNode.hpp>

namespace tinker::ui {

class AnimatedNotification : public cocos2d::CCNodeRGBA {
public:
    static AnimatedNotification* create(geode::ZStringView text);

    void show();
protected:
    bool init(geode::ZStringView text);

    geode::NineSlice* m_container = nullptr;
    geode::Label* m_label = nullptr;
    alpha::ui::RenderNode* m_render = nullptr;
    std::string m_text = "";
};

}