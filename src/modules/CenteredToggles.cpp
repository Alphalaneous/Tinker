#include "modules/CenteredToggles.hpp"

using namespace geode::prelude;

bool CenteredToggles::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        auto editor = getEditor();
        setOffset(editor->m_swipeBtn, 0.f);
        setOffset(editor->m_freeMoveBtn, 0.f);
        setOffset(editor->m_rotateBtn, 0.f);
        setOffset(editor->m_snapBtn, 0.f);

        auto toolbarCategoriesMenu = editor->getChildByID("toolbar-categories-menu");
        if (toolbarCategoriesMenu) {
            for (auto child : toolbarCategoriesMenu->getChildrenExt()) {
                setOffset(child, 0.f);
            }
        }
    }
    return true;
}

void CenteredToggles::setOffset(CCNode* node, float offset) {
    auto spr = node->getChildByType<ButtonSprite>();
    if (spr) {
        spr->setAnchorPoint({0.5f, 0.5f});
        spr->updateSpriteOffset({0.f, offset});
    }
}

void CenteredToggles::onEditor() {
    auto editor = getEditor();

    setOffset(editor->m_swipeBtn, -1.5f);
    setOffset(editor->m_freeMoveBtn, -1.5f);
    // arrow weight at the top makes it look uncentered so I offset it slightly less
    setOffset(editor->m_rotateBtn, -1.2f);
    setOffset(editor->m_snapBtn, -1.5f);
}