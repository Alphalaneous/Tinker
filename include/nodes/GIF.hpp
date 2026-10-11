#pragma once

#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/utils/ZStringView.hpp>

namespace tinker::ui {

class GIF : public cocos2d::CCSprite {
public:
    static GIF* create(geode::ZStringView gif);
protected:
    bool init(geode::ZStringView gif);
};

}