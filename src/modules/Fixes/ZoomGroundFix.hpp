#pragma once

#include "module/Module.hpp"
#include <Geode/modify/GJBaseGameLayer.hpp>

class $editorModule(ZoomGroundFix) {
	bool onToggled(bool state);
};

class $modify(ZGFGJBaseGameLayer, GJBaseGameLayer) {
	$registerEditorHooks(ZoomGroundFix)

	void updateCameraBGArt(CCPoint position, float zoom);
};