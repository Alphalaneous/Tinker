#pragma once

#include "module/Module.hpp"
#include <Geode/modify/TextGameObject.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>

class $module(ReferenceImage, true) {
	void onEditor();
};

class $modify(RICustomizeObjectLayer, CustomizeObjectLayer) {
	$registerHooks(ReferenceImage);

	struct Fields {
		bool m_isImageObject = false;
	};

	bool init(GameObject* object, cocos2d::CCArray* objectArray);
	void setTextBtn();
	void onSelectMode(cocos2d::CCObject* sender);
};

class $modify(RITextGameObject, TextGameObject) {
	$registerHooks(ReferenceImage);

	struct Fields {
		geode::Ref<geode::LazySprite> m_spr = nullptr;
	};

	void onImageFail(geode::ZStringView icon, geode::ZStringView text);
	void setAttributes();
	void setupCustomSprite();
	bool setupInitial(const std::string& path);
	void setupImage(const std::string& path);
	void setupImageBackwardsCompat(const std::string& path);
    void customObjectSetup(std::vector<std::string>& values, std::vector<void*>& exists);
	void updateTextObject(std::string text, bool defaultFont);

	bool isReferenceImage();
};