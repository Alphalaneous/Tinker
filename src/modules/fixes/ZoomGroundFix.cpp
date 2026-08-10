#include "modules/fixes/ZoomGroundFix.hpp"
#include "utils/Constants.hpp"

bool ZoomGroundFix::onToggled(bool state) {
	return true;
}

void ZGFGJBaseGameLayer::updateCameraBGArt(CCPoint position, float zoom) {
	GJBaseGameLayer::updateCameraBGArt(position, zoom);
    using namespace tinker::constants;

	if (m_playbackMode == PlaybackMode::Playing) return;
	
	float camX = m_objectLayer->getPositionX();
	float camAngle = m_gameState.m_cameraAngle;

	auto editorLayer = static_cast<LevelEditorLayer*>(static_cast<GJBaseGameLayer*>(this));

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

	if (m_groundLayer && (m_groundLayer->m_ground1Sprite || m_groundLayer->m_ground2Sprite)) {

		float originX = camX / m_groundLayer->getScaleX();
		int tilesToMove = std::ceil(originX / m_groundLayer->m_textureWidth);

		if (camAngle != 0.f) {

			int tiles = std::ceil(m_groundLayer->m_groundWidth / m_groundLayer->m_textureWidth) + 2;
			int extra = std::ceil(1.0f / zoom);
			int total = tiles + extra;

			m_groundLayer->loadGroundSprites(total, true);
			if (m_groundLayer->m_ground2Sprite) {
				m_groundLayer->loadGroundSprites(total, false);
			}

			tilesToMove += extra / 2;
		}

		float finalX = originX - (tilesToMove * m_groundLayer->m_textureWidth);

		if (m_groundLayer->m_ground1Sprite) m_groundLayer->m_ground1Sprite->setPositionX(finalX);
		if (m_groundLayer->m_ground2Sprite) m_groundLayer->m_ground2Sprite->setPositionX(finalX);

		m_groundLayer->updateGround01Color(color1);
		m_groundLayer->updateGround02Color(color2);
	}

	if (m_middleground && (m_middleground->m_ground1Sprite || m_middleground->m_ground2Sprite)) {

		float originX = camX / m_middleground->getScaleX();
		int tilesToMove = std::ceil(originX / m_middleground->m_textureWidth);

		if (camAngle != 0.f) {

			int tiles = std::ceil(m_middleground->m_groundWidth / m_middleground->m_textureWidth) + 2;
			int extra = std::ceil(1.f / zoom);
			int total = tiles + extra;

			m_middleground->loadGroundSprites(total, true);

			if (m_middleground->m_ground2Sprite) {
				m_middleground->loadGroundSprites(total, false);
			}

			tilesToMove += extra / 2;
		}

		float finalX = originX - (tilesToMove * m_middleground->m_textureWidth);

		if (m_middleground->m_ground1Sprite) m_middleground->m_ground1Sprite->setPositionX(finalX);
		if (m_middleground->m_ground2Sprite) m_middleground->m_ground2Sprite->setPositionX(finalX);

		m_middleground->updateGroundColor(color1DataMG.color, true);
		m_middleground->updateGroundColor(color2DataMG.color, false);

		m_middleground->updateGroundOpacity(color1DataMG.opacity, true);
		m_middleground->updateGroundOpacity(color2DataMG.opacity, false);

		m_middleground->updateMG01Blend(color1DataMG.blending);
		m_middleground->updateMG02Blend(color2DataMG.blending);
	}
}