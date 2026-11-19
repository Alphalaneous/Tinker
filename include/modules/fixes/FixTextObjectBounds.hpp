#pragma once

#include "module/Module.hpp"
#include <Geode/modify/TextGameObject.hpp>

class $module(FixTextObjectBounds) {
    bool onToggled(bool state);
};

class $modify(FTOBTextGameObject, TextGameObject) {
    $registerHooks(FixTextObjectBounds)

    void fixBounds();

    void customObjectSetup(std::vector<std::string>& values, std::vector<void*>& exists);
    void updateTextObject(std::string text, bool defaultFont);
};