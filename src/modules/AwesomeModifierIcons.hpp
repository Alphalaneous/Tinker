#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EffectGameObject.hpp>

class $editorModule(AwesomeModifierIcons) {
    void onEditor() override;

    static std::unordered_map<int, std::string> s_textureMap;
};

class $modify(AMIEffectGameObject, EffectGameObject) {
    $registerEditorHooks(AwesomeModifierIcons)

	void customSetup();
    void updateLetters();
};