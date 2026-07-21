#include "SlopeDirectionIndicators.hpp"

bool SlopeDirectionIndicators::onToggled(bool state) {
    return true;
}

void SlopeDirectionDraw::drawIndicator(GameObject* obj, float minX, float maxX, float minY, float maxY) {
    if (obj->m_objectType != GameObjectType::Slope) return;
    auto& rect = obj->getObjectRect();
    if (rect.getMaxX() < minX || rect.getMinX() > maxX || rect.getMaxY() < minY || rect.getMinY() > maxY) return;

    obj->determineSlopeDirection();

    enum Corner : unsigned char {
        BL, BR, TR, TL
    };

    static constexpr Corner lookup[][3] = {
        {BL, BR, TR},
        {TL, TR, BR},
        {BR, BL, TL},
        {TR, TL, BL},
        {TL, BL, BR},
        {BR, TR, TL},
        {BL, TL, TR},
        {TR, BR, BL},
    };

    const ccVertex2F corners[] = {
        {rect.getMinX(), rect.getMinY()},
        {rect.getMaxX(), rect.getMinY()},
        {rect.getMaxX(), rect.getMaxY()},
        {rect.getMinX(), rect.getMaxY()},
    };

    const auto& dir = lookup[obj->m_slopeDirection];

    DrawGridAPI::get().drawLine(corners[dir[0]], corners[dir[1]], {255, 50, 50, 50}, 3);
    DrawGridAPI::get().drawLine(corners[dir[2]], corners[dir[1]], {255, 50, 50, 50}, 3);
}
    
void SlopeDirectionDraw::draw(DrawGridLayer* dgl, float minX, float maxX, float minY, float maxY) {
    auto editorUI = dgl->m_editorLayer->m_editorUI;
    if (editorUI->m_selectedObject) {
        drawIndicator(editorUI->m_selectedObject, minX, maxX, minY, maxY);
    }
    else {
        for (auto obj : editorUI->m_selectedObjects->asExt<GameObject>()) {
            drawIndicator(obj, minX, maxX, minY, maxY);
        }
    }
}

$on_mod(Loaded) {
	auto& api = DrawGridAPI::get();
    
	auto& node = api.addDraw<SlopeDirectionDraw>("slope-direction-indicators");

    listenForSettingChanges<bool>("SlopeDirectionIndicators-enabled", [&node] (bool val) {
        node.setEnabled(val);
    });

    node.setEnabled(SlopeDirectionIndicators::isEnabled());
}