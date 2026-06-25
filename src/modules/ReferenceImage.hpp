#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/TextGameObject.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>

class $editorModule(ReferenceImage) {
	void onEditor() override;
};

class $modify(RICustomizeObjectLayer, CustomizeObjectLayer) {
	$registerEditorHooks(ReferenceImage);

	struct Fields {
		bool m_isImageObject;
	};

	bool init(GameObject* object, CCArray* objectArray);
	void setTextBtn();
	void onSelectMode(CCObject* sender);
};

class $modify(RITextGameObject, TextGameObject) {
	$registerEditorHooks(ReferenceImage);

	struct Fields {
		Ref<LazySprite> m_spr;
		bool m_isReferenceImage;
	};

	void onImageFail();
	void setAttributes();
	void setupCustomSprite();
	bool setupInitial(const std::string& path);
	void setupImage(const std::string& path);
	void setupImageBackwardsCompat(const std::string& path);
    void customObjectSetup(gd::vector<gd::string>& values, gd::vector<void*>& exists);
	void updateTextObject(gd::string text, bool defaultFont);

	bool isReferenceImage();
};

class $modify(RIEditorUI, EditorUI) {
	$registerEditorHooks(ReferenceImage)

	void onImport(CCObject* sender);
};