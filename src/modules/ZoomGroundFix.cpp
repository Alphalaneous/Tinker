#include "ZoomGroundFix.hpp"
#include "utils/Constants.hpp"
#include "utils/Utils.hpp"

bool ZoomGroundFix::onToggled(bool state) {
    if (state) m_editorUI->schedule(schedule_selector(ZoomGroundFix::fixPosition));
    else m_editorUI->unschedule(schedule_selector(ZoomGroundFix::fixPosition));
    
    return true;
}

void ZoomGroundFix::onEditor() {
    m_editorUI->schedule(schedule_selector(ZoomGroundFix::fixPosition));
}

void ZoomGroundFix::fixPosition(float dt) {
    using namespace tinker::constants;

    auto editorLayer = LevelEditorLayer::get();
    if (editorLayer && editorLayer->m_playbackMode != PlaybackMode::Playing) {
        float zoom = editorLayer->m_objectLayer->getScale();

        ccColor3B color1;
        ccColor3B color2;

        tinker::utils::ColorData color1DataMG;
        tinker::utils::ColorData color2DataMG;

        if (!editorLayer->m_previewMode) {
            color1 = colors::DefaultGround;
            color2 = colors::DefaultGround;

            color1DataMG = {colors::DefaultMiddleground1, false, 255};
            color2DataMG = {colors::DefaultMiddleground2, false, 255};
        }
        else {
            color1 = tinker::utils::getActiveColor(editorLayer, color_channels::Ground1).color;
            color2 = tinker::utils::getActiveColor(editorLayer, color_channels::Ground2).color;

            color1DataMG = tinker::utils::getActiveColor(editorLayer, color_channels::Middleground1);
            color2DataMG = tinker::utils::getActiveColor(editorLayer, color_channels::Middleground2);
        }

        auto winSize = CCDirector::get()->getWinSize();

        auto gl = editorLayer->m_groundLayer;
        if (gl) {
            auto x = editorLayer->m_objectLayer->getPositionX() / zoom;
            float extra = x > 0.f ? -gl->m_textureWidth : 0.f;

            auto mod = std::fmod(x, gl->m_textureWidth) - gl->m_textureWidth * 3.f + extra;

            gl->scaleGround(zoom);
            gl->m_groundWidth = winSize.width / zoom + 10.f;

            int count = std::ceilf(gl->m_groundWidth / gl->m_textureWidth) + 10.f;
            gl->loadGroundSprites(count, true);
            gl->loadGroundSprites(count, false);
            gl->m_lineSprite->setScaleX(gl->m_lineType > 1 ? (((winSize.width + 10.f) / zoom) / gl->m_lineSprite->getTextureRect().size.width) : (1.f / zoom));
            gl->m_lineSprite->setPosition({gl->m_groundWidth * 0.5f - 5.f, gl->m_lineSprite->getPosition().y});
            gl->updateGroundPos({mod, 0.f});
            gl->updateGround01Color(color1);
            gl->updateGround02Color(color2);
        }

        auto gl2 = editorLayer->m_groundLayer2;
        if (gl2) {
            auto x = gl2->getPositionX() / zoom;
            float extra = x > 0.f ? -gl2->m_textureWidth : 0.f;

            auto mod = std::fmod(x, gl2->m_textureWidth) - gl2->m_textureWidth * 3.f + extra;

            gl2->scaleGround(zoom);

            int count = std::ceilf(gl2->m_groundWidth / gl2->m_textureWidth) + 10.f;
            gl2->loadGroundSprites(count, true);
            gl2->loadGroundSprites(count, false);
            gl2->m_lineSprite->setScaleX(gl2->m_lineType > 1 ? (((winSize.width + 10.f) / zoom) / gl2->m_lineSprite->getTextureRect().size.width) : (1.f / zoom));
            gl2->m_lineSprite->setPosition({gl2->m_groundWidth * 0.5f - 5.f, gl2->m_lineSprite->getPosition().y});

            gl2->updateGroundPos({mod, 0});
            gl2->updateGround01Color(color1);
            gl2->updateGround02Color(color2);
        }

        auto mg = editorLayer->m_middleground;
        if (mg) {
            mg->scaleGround(zoom);

            auto x = mg->getPositionX() / zoom;
            float extra = x > 0.f ? -mg->m_textureWidth : 0.f;

            auto mod = std::fmod(x, mg->m_textureWidth) - mg->m_textureWidth * 3.f + extra;

            int count = std::ceilf(mg->m_groundWidth / mg->m_textureWidth) + 10.f;
            mg->loadGroundSprites(count, true);
            mg->loadGroundSprites(count, false);

            mg->updateGroundPos({mod, 0.f});
            mg->updateGroundColor(color1DataMG.color, true);
            mg->updateGroundColor(color2DataMG.color, false);

            mg->updateGroundOpacity(color1DataMG.opacity, true);
            mg->updateGroundOpacity(color2DataMG.opacity, false);

            mg->updateMG01Blend(color1DataMG.blending);
            mg->updateMG02Blend(color2DataMG.blending);
        }
    }
}

void ZGFEditorUI::onPlaytest(cocos2d::CCObject* sender) {
    EditorUI::onPlaytest(sender);
    if (m_editorLayer->m_groundLayer) m_editorLayer->m_groundLayer->scaleGround(m_editorLayer->m_objectLayer->getScale());
    if (m_editorLayer->m_groundLayer2) m_editorLayer->m_groundLayer2->scaleGround(m_editorLayer->m_objectLayer->getScale());
    if (m_editorLayer->m_middleground) m_editorLayer->m_middleground->scaleGround(m_editorLayer->m_objectLayer->getScale());
}

void ZGFEditorUI::updateZoom(float zoom) {
    EditorUI::updateZoom(zoom);
    auto fix = ZoomGroundFix::get();
    if (fix) fix->fixPosition(0.f);
}

void ZGFEditorUI::constrainGameLayerPosition(float x, float y) {
    EditorUI::constrainGameLayerPosition(x, y);
    auto fix = ZoomGroundFix::get();
    if (fix) fix->fixPosition(0.f);
}