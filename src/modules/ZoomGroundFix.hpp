#pragma once

#include "../Module.hpp"

class $editorModule(ZoomGroundFix) {
    bool onToggled(bool state) override;

    void onEditor() override;

    void fixPosition(float dt);
};