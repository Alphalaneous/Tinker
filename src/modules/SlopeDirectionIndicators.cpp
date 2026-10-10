#include "modules/SlopeDirectionIndicators.hpp"


bool SlopeDirectionIndicators::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        if (m_directionDraw) {
            m_directionDraw->removeFromParent();
            m_directionDraw = nullptr;
        }
    }
    return true;
}

void SlopeDirectionIndicators::onEditor() {
    auto editorLayer = getEditorLayer();

    m_directionDraw = tinker::ui::SlopeDirectionDraw::create();
    m_directionDraw->setID("slope-direction"_spr);
    m_directionDraw->setZOrder(0);
    editorLayer->m_drawGridLayer->addChild(m_directionDraw);
}

namespace tinker::ui {

SlopeDirectionDraw* SlopeDirectionDraw::create() {
    auto ret = new SlopeDirectionDraw();
    ret->autorelease();
    return ret;
}

void SlopeDirectionDraw::drawIndicator(GameObject* obj, const CCRect& visibleRect) {
    if (obj->m_objectType != GameObjectType::Slope) return;
    auto& rect = obj->getObjectRect();

    if (rect.getMaxX() < visibleRect.getMinX() || rect.getMinX() > visibleRect.getMaxX() || rect.getMaxY() < visibleRect.getMinY() || rect.getMinY() > visibleRect.getMaxY()) return;

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

    const CCPoint corners[] = {
        {rect.getMinX(), rect.getMinY()},
        {rect.getMaxX(), rect.getMinY()},
        {rect.getMaxX(), rect.getMaxY()},
        {rect.getMinX(), rect.getMaxY()},
    };

    const auto& dir = lookup[obj->m_slopeDirection];

    drawLine(corners[dir[0]], corners[dir[1]], {255, 50, 50, 50}, 3);
    drawLine(corners[dir[2]], corners[dir[1]], {255, 50, 50, 50}, 3);
}
    
void SlopeDirectionDraw::draw(const CCRect& visibleRect) {
    auto dgl = getDrawGridLayer();

    auto editorUI = dgl->m_editorLayer->m_editorUI;
    if (editorUI->m_selectedObject) {
        drawIndicator(editorUI->m_selectedObject, visibleRect);
    }
    else {
        for (auto obj : editorUI->m_selectedObjects->asExt<GameObject>()) {
            drawIndicator(obj, visibleRect);
        }
    }
}

}