#include "modules/ImprovedNotifications.hpp"
#include "Geode/loader/Log.hpp"
#include "Geode/utils/cocos.hpp"
#include "nodes/AnimatedNotification.hpp"
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/TextAlertPopup.hpp>

using namespace geode::prelude;

bool ImprovedNotifications::onToggled(bool state) {
    return true;
}

bool INTextAlertPopup::init(std::string text, float delay, float scale, int opacity, std::string font) {
    if (!TextAlertPopup::init(text, delay, scale, opacity, font)) return false;

    runAction(CallFuncExt::create([this, text] {
        if (getParent() != EditorUI::get()) return;
        removeFromParent();
        tinker::ui::AnimatedNotification::create(text)->show();
    }));

    return true;
}