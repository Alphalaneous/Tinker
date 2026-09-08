#include "modules/fixes/SelectionFix.hpp"
#include "utils/Utils.hpp"

bool SelectionFix::onToggled(bool state) {
    return true;
}

GameObject* SFLevelEditorLayer::objectAtPosition(cocos2d::CCPoint position) {
    auto pointRect = CCRect{position.x, position.y, 1.f, 1.f};
    auto center = pointRect.origin + pointRect.size / 2.f;

    m_obb2->calculateWithCenter(center, pointRect.size.width, pointRect.size.height, 0.f);

    GameObject* object = nullptr;

    tinker::utils::forEachObject(this, [this, position, &object] (GameObject* obj) mutable {
        if (object) return;

        if (validGroup(obj, true)) {
            auto rect = getObjectRect(obj, false, false);

            if (!m_nonSquareRotation) {
                if (rect.containsPoint(position)) {
                    object = obj;
                    return;
                }
            }
            else if (m_currentOBB2D->overlaps1Way(m_obb2) && m_obb2->overlaps1Way(m_currentOBB2D)) {
                object = obj;
                return;
            }
        }
    });

    return object;
}

cocos2d::CCArray* SFLevelEditorLayer::objectsAtPosition(cocos2d::CCPoint position) {
    auto pointRect = CCRect{position.x, position.y, 1.f, 1.f};
    auto center = pointRect.origin + pointRect.size / 2.f;

    m_obb2->calculateWithCenter(center, pointRect.size.width, pointRect.size.height, 0.f);

    auto objects = CCArray::create();

    tinker::utils::forEachObject(this, [this, position, objects] (GameObject* obj) mutable {
        if (validGroup(obj, true)) {
            auto rect = getObjectRect(obj, false, false);

            if (!m_nonSquareRotation) {
                if (!rect.containsPoint(position)) {
                    return;
                }
            }
            else if (!m_currentOBB2D->overlaps1Way(m_obb2) || !m_obb2->overlaps1Way(m_currentOBB2D)) {
                return;
            }
            
            objects->addObject(obj);
        }
    });

    return objects;
}