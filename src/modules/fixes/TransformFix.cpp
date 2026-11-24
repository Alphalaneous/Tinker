#include "modules/fixes/TransformFix.hpp"
#include "utils/Mod.hpp"
#include <Geode/binding/GameObject.hpp>

using namespace geode::prelude;

bool TransformFix::onToggled(bool state) {
    return true;
}

void TFGJTransformControl::updateButtons(bool transform, bool skew) {
    auto editor = LevelEditorLayer::get();
    auto shaders = editor->m_shaderLayer->m_state.m_usesShaders;
    editor->m_shaderLayer->m_state.m_usesShaders = false;
    editor->updateShaderLayer(0.f);

    GJTransformControl::updateButtons(transform, skew);

    editor->m_shaderLayer->m_state.m_usesShaders = shaders;
}

void TFGJTransformControl::scaleButtons(float scale) {
    GJTransformControl::scaleButtons(scale);
    if (tinker::utils::mod::getMod<"razoom.improved_transform_controls">()) return;
    if (!m_warpLockButton) return;

    auto spr = m_warpLockButton->getChildByIndex(0);
    if (!spr) return;

    m_warpLockButton->setContentSize(spr->getScaledContentSize());
    spr->setPosition(m_warpLockButton->getContentSize()/2);
}