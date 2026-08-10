#include "modules/fixes/AddToSectionCrashFix.hpp"

bool AddToSectionCrashFix::onToggled(bool state) {
    return true;
}

void ATSCFGJBaseGameLayer::addToSection(GameObject* object) {
    double x = object->m_positionX;
    double y = object->m_positionY;

    auto sanitize = [](double& value) {
        if (std::isnan(value)) {
            value = 0;
        }
        else if (std::isinf(value)) {
            value = value > 0 ? std::numeric_limits<double>::max() : std::numeric_limits<double>::lowest();
        }
    };

    sanitize(object->m_positionX);
    sanitize(object->m_positionY);

    GJBaseGameLayer::addToSection(object);

    object->m_positionX = x;
    object->m_positionY = y;
}