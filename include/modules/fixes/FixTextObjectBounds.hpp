#pragma once

#include "module/Module.hpp"
#include <Geode/modify/TextGameObject.hpp>

class $module(FixTextObjectBounds) {
    bool onToggled(bool state);
};

class $modify(FTOBTextGameObject, TextGameObject) {
    $registerHooks(FixTextObjectBounds)

    void fixBounds();

    void customObjectSetup(gd::vector<gd::string>& values, gd::vector<void*>& exists);
    void updateTextObject(gd::string text, bool defaultFont);
};