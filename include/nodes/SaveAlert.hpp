#pragma once

#include <Geode/ui/Label.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <alphalaneous.alphas-ui-pack/nodes/RenderNode.hpp>

namespace tinker::ui {

class SaveAlert : public cocos2d::CCNodeRGBA {
public:
    static SaveAlert* create();

    void setText(geode::ZStringView text);
    void setAlertColor(const cocos2d::ccColor3B& color);
    void show();
    void hide();
protected:
    bool init() override;

    cocos2d::CCSprite* m_saveIcon = nullptr;
    geode::NineSlice* m_container = nullptr;
    geode::Label* m_label = nullptr;
    alpha::ui::RenderNode* m_render = nullptr;
    std::string m_text = "";
};

}