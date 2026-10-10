#pragma once

#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/utils/ZStringView.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class GIF : public CCSprite {
public:
    static GIF* create(ZStringView gif);
protected:
    bool init(ZStringView gif);
};

}