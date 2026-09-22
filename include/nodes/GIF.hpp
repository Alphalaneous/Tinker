#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class GIF : public CCSprite {
public:
    static GIF* create(ZStringView gif);
protected:
    bool init(ZStringView gif);
};

}