#pragma once

#ifndef GEODE_IS_IOS

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(CopyObjectString) {
    bool onToggled(bool state);

    static bool isObjectString(ZStringView str);
};

class $modify(COSEditorUI, EditorUI) {
    $registerHooks(CopyObjectString)

    void doCopyObjects(bool withColor);
    void doPasteObjects(bool withColor);
};

#endif