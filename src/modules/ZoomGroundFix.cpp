#include "ZoomGroundFix.hpp"

bool ZoomGroundFix::onToggled(bool state) {
    if (state) {
        m_editorUI->schedule(schedule_selector(ZoomGroundFix::fixPosition));
    }
    else {
        m_editorUI->unschedule(schedule_selector(ZoomGroundFix::fixPosition));
    }
    return true;
}

void ZoomGroundFix::onEditor() {
    m_editorUI->schedule(schedule_selector(ZoomGroundFix::fixPosition));
}

void ZoomGroundFix::fixPosition(float dt) {
    auto editorLayer = LevelEditorLayer::get();
    if (editorLayer->m_playbackMode == PlaybackMode::Not) {
        float zoom = editorLayer->m_objectLayer->getScale();
        auto x = editorLayer->m_objectLayer->getPositionX() / zoom;
        float extra = 0;
        if (x > 0) {
            extra = -editorLayer->m_groundLayer->m_textureWidth;
        }

        auto color1 = tinker::utils::getActiveColor(editorLayer, 1001).color;
        auto color2 = tinker::utils::getActiveColor(editorLayer, 1009).color;

        auto color1DataMG = tinker::utils::getActiveColor(editorLayer, 1013);
        auto color2DataMG = tinker::utils::getActiveColor(editorLayer, 1014);

        auto winSize = CCDirector::get()->getWinSize();

        auto mod = std::fmod(x, editorLayer->m_groundLayer->m_textureWidth) - editorLayer->m_groundLayer->m_textureWidth * 3 + extra;

        if (editorLayer->m_groundLayer) {
            editorLayer->m_groundLayer->scaleGround(zoom);
            editorLayer->m_groundLayer->m_groundWidth = winSize.width / zoom + 10.f;

            int count = std::ceilf(editorLayer->m_groundLayer->m_groundWidth / editorLayer->m_groundLayer->m_textureWidth) + 10.f;
            editorLayer->m_groundLayer->loadGroundSprites(count, true);
            editorLayer->m_groundLayer->loadGroundSprites(count, false);
            editorLayer->m_groundLayer->m_lineSprite->setScaleX(editorLayer->m_groundLayer->m_lineType > 1 ? (((winSize.width + 10.f) / zoom) / editorLayer->m_groundLayer->m_lineSprite->getTextureRect().size.width) : (1.f / zoom));
            editorLayer->m_groundLayer->m_lineSprite->setPosition({ editorLayer->m_groundLayer->m_groundWidth * .5f - 5.f, editorLayer->m_groundLayer->m_lineSprite->getPosition().y } );
            editorLayer->m_groundLayer->updateGroundPos({mod, 0});
            editorLayer->m_groundLayer->updateGround01Color(color1);
            editorLayer->m_groundLayer->updateGround02Color(color2);
        }
        if (editorLayer->m_groundLayer2) {
            editorLayer->m_groundLayer2->scaleGround(zoom);

            int count = std::ceilf(editorLayer->m_groundLayer2->m_groundWidth / editorLayer->m_groundLayer2->m_textureWidth) + 10.f;
            editorLayer->m_groundLayer2->loadGroundSprites(count, true);
            editorLayer->m_groundLayer2->loadGroundSprites(count, false);
            editorLayer->m_groundLayer2->m_lineSprite->setScaleX(editorLayer->m_groundLayer2->m_lineType > 1 ? (((winSize.width + 10.f) / zoom) / editorLayer->m_groundLayer2->m_lineSprite->getTextureRect().size.width) : (1.f / zoom));
            editorLayer->m_groundLayer2->m_lineSprite->setPosition({ editorLayer->m_groundLayer2->m_groundWidth * .5f - 5.f, editorLayer->m_groundLayer2->m_lineSprite->getPosition().y } );

            editorLayer->m_groundLayer2->updateGroundPos({mod, 0});
            editorLayer->m_groundLayer2->updateGround01Color(color1);
            editorLayer->m_groundLayer2->updateGround02Color(color2);
        }
        if (editorLayer->m_middleground) {
            editorLayer->m_middleground->scaleGround(zoom);

            float extra = 0;
            if (x > 0) {
                extra = -editorLayer->m_middleground->m_textureWidth;
            }

            auto mod = std::fmod(x, editorLayer->m_middleground->m_textureWidth) - editorLayer->m_middleground->m_textureWidth * 3 + extra;

            int count = std::ceilf(editorLayer->m_middleground->m_groundWidth / editorLayer->m_middleground->m_textureWidth) + 10.f;
            editorLayer->m_middleground->loadGroundSprites(count, true);
            editorLayer->m_middleground->loadGroundSprites(count, false);

            editorLayer->m_middleground->updateGroundPos({mod, 0});
            editorLayer->m_middleground->updateGroundColor(color1DataMG.color, true);
            editorLayer->m_middleground->updateGroundColor(color2DataMG.color, false);

            editorLayer->m_middleground->updateGroundOpacity(color1DataMG.opacity, true);
            editorLayer->m_middleground->updateGroundOpacity(color2DataMG.opacity, false);

            editorLayer->m_middleground->updateMG01Blend(color1DataMG.blending);
            editorLayer->m_middleground->updateMG02Blend(color2DataMG.blending);
        }
    }
}