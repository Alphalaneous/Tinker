#include "ZoomGroundFix.hpp"

void ZGFGJBaseGameLayer::updateCameraBGArt(CCPoint position, float zoom) {
	GJBaseGameLayer::updateCameraBGArt(position, zoom);

	auto editor = LevelEditorLayer::get();
	auto fix = ZoomGroundFix::get();

	if (!fix || !editor || this->m_playbackMode == PlaybackMode::Playing) {
		return;
	} 
	
	auto ground = this->m_groundLayer;
	auto middleground = this->m_middleground;

	float camX = this->m_objectLayer->getPositionX();
	float camAngle = this->m_gameState.m_cameraAngle;

	if (ground && (ground->m_ground1Sprite || ground->m_ground2Sprite)) {

		float originX = camX / ground->getScaleX();
		int tilesToMove = std::ceil(originX / ground->m_textureWidth);

		if (camAngle != 0.0f) {

			int tiles = std::ceil(ground->m_groundWidth / ground->m_textureWidth) + 2;
			int extra = std::ceil(1.0f / zoom);
			int total = tiles + extra;

			ground->loadGroundSprites(total, true);
			if (ground->m_ground2Sprite)
				ground->loadGroundSprites(total, false);

			tilesToMove += extra / 2;
		}

		float finalX = originX - (tilesToMove * ground->m_textureWidth);

		if (ground->m_ground1Sprite) ground->m_ground1Sprite->setPositionX(finalX);
		if (ground->m_ground2Sprite) ground->m_ground2Sprite->setPositionX(finalX);
	}

	if (middleground && (middleground->m_ground1Sprite || middleground->m_ground2Sprite)) {

		float originX = camX / middleground->getScaleX();
		int tilesToMove = std::ceil(originX / middleground->m_textureWidth);

		if (camAngle != 0.0f) {

			int tiles = std::ceil(middleground->m_groundWidth / middleground->m_textureWidth) + 2;
			int extra = std::ceil(1.0f / zoom);
			int total = tiles + extra;

			middleground->loadGroundSprites(total, true);
			if (middleground->m_ground2Sprite)
				middleground->loadGroundSprites(total, false);

			tilesToMove += extra / 2;
		}

		float finalX = originX - (tilesToMove * middleground->m_textureWidth);

		if (middleground->m_ground1Sprite) middleground->m_ground1Sprite->setPositionX(finalX);
		if (middleground->m_ground2Sprite) middleground->m_ground2Sprite->setPositionX(finalX);
	}
}