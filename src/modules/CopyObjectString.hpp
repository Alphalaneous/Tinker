#pragma once

#include "Geode/utils/ZStringView.hpp"
#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(CopyObjectString) {
    bool onToggled(bool state) override;

    static bool isObjectString(ZStringView str);
};

class $modify(COSEditorUI, EditorUI) {
    $registerEditorHooks(CopyObjectString)

    void doCopyObjects(bool withColor);
    void doPasteObjects(bool withColor);
};