#pragma once

#include "../Module.hpp"
#include <Geode/modify/TextGameObject.hpp>

class $globalModule(FixTextObjectBounds) {
    bool onToggled(bool state) override;
};

class $modify(FTOBTextGameObject, TextGameObject) {
    $registerGlobalHooks(FixTextObjectBounds)

    void fixBounds();

    void customObjectSetup(gd::vector<gd::string>& values, gd::vector<void*>& exists);
    void updateTextObject(gd::string text, bool defaultFont);
};