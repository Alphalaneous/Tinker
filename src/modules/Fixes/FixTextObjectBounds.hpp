#pragma once

#include "module/Module.hpp"
#include <Geode/modify/TextGameObject.hpp>

class $editorModule(FixTextObjectBounds) {
    bool onToggled(bool state) override;
};

class $modify(FTOBTextGameObject, TextGameObject) {
    $registerEditorHooks(FixTextObjectBounds)

    void fixBounds();

    void customObjectSetup(gd::vector<gd::string>& values, gd::vector<void*>& exists);
    void updateTextObject(gd::string text, bool defaultFont);
};