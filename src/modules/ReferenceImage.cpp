#include "modules/ReferenceImage.hpp"
#include <Geode/utils/base64.hpp>
#include <Geode/utils/async.hpp>

void ReferenceImage::onEditor() {
    auto btn = getEditor()->getSpriteButton("image-btn.png"_spr, menu_selector(RIEditorUI::onImport), nullptr, 0.9f);
    btn->setID("reference-import"_spr);
    getEditor()->m_editButtonBar->m_buttonArray->addObject(btn);

    auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
    auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

    getEditor()->m_editButtonBar->reloadItems(cols, rows);
}

bool RICustomizeObjectLayer::init(GameObject* object, CCArray* objectArray) {
    if (!CustomizeObjectLayer::init(object, objectArray)) return false;
    setTextBtn();
    return true;
};

void RICustomizeObjectLayer::setTextBtn() {
    auto fields = m_fields.self();
    auto textObject = typeinfo_cast<TextGameObject*>(m_targetObject);
    if (!textObject) return;

    auto pair = tinker::utils::splitIntoPair(textObject->m_text);
    if (pair.first != "image") return;

    fields->m_isImageObject = true;
    if (!m_textButton) return;
    if (auto spr = m_textButton->getChildByType<ButtonSprite*>(0)) {
        if (spr->m_label) {
            spr->m_label->setString("Image");
        }
    }
}

void RICustomizeObjectLayer::onSelectMode(CCObject* sender) {
    auto fields = m_fields.self();

    if (sender->getTag() == 3 && fields->m_isImageObject) {
        utils::file::FilePickOptions options;
        options.filters.push_back({"Images", {"*.png", "*.apng", "*.jpg", "*.jpeg", "*.jfif", "*.pjpeg", "*.pjp", "*.webp", "*.gif", "*.bmp", "*.jxl"}});

        async::spawn(
            utils::file::pick(file::PickMode::OpenFile, options),
            [this](Result<std::optional<std::filesystem::path>> result) {
                if (!result.isOk()) return;
                auto opt = result.unwrap();
                if (!opt) return;

                auto path = opt.value();
                if (auto textObject = typeinfo_cast<TextGameObject*>(m_targetObject)) {
                    textObject->updateTextObject("image:" + utils::base64::encode(utils::string::pathToString(path)), false);
                }
            }
        );
    } else {
        CustomizeObjectLayer::onSelectMode(sender);
    }
    setTextBtn();
}

void RITextGameObject::onImageFail(ZStringView icon, ZStringView text) {
    for (auto child : getChildrenExt()) {
        child->setVisible(false);
    }
    auto node = CCNodeRGBA::create();
    node->setAnchorPoint({0.5f, 0.5f});
    node->ignoreAnchorPointForPosition(false);
    node->setID("error-node"_spr);

    node->setCascadeColorEnabled(true);
    node->setCascadeOpacityEnabled(true);

    auto spr = CCSprite::createWithSpriteFrameName(icon.c_str());
    node->addChild(spr);

    node->setContentSize(spr->getContentSize());
    node->setPosition(node->getContentSize() / 2.f);
    spr->setPosition(node->getContentSize() / 2.f);

    auto label = geode::Label::create(text.c_str(), "chatFont.fnt");
    label->setAnchorPoint({0.5f, 0.f});
    label->setScale(0.8f);
    label->setAlignment(geode::Label::Alignment::Center);
    label->setPositionX(node->getContentWidth() / 2.f);
    label->setPositionY(node->getContentHeight() + 3.f);

    node->addChild(label);
    addChild(node);
    
    setContentSize(node->getContentSize());
    m_width = getContentWidth();
    m_height = getContentHeight();
    updateOrientedBox();
}

bool RITextGameObject::isReferenceImage() {
    auto pair = tinker::utils::splitIntoPair(m_text);
    return pair.first == "image";
}

void RITextGameObject::setAttributes() {
    auto fields = m_fields.self();
    if (auto node = getChildByID("error-node"_spr)) {
        node->removeFromParent();
    }
    setContentSize(fields->m_spr->getContentSize());
    fields->m_spr->setPosition(getContentSize() / 2.f);
    fields->m_spr->setColor(getColor());
    fields->m_spr->setOpacity(getOpacity());

    m_width = getContentWidth();
    m_height = getContentHeight();
    updateOrientedBox();
}

void RITextGameObject::setupCustomSprite() {
    auto pair = tinker::utils::splitIntoPair(m_text);

    if (pair.first == "image") {
        if (!setupInitial(pair.second)) return;
        setupImage(pair.second);
    }
}

bool RITextGameObject::setupInitial(const std::string& path) {
    m_addToNodeContainer = true;
    for (auto child : getChildrenExt()) {
        child->setVisible(false);
    }
    
    if (path.empty()) return false;
    return true;
}


void RITextGameObject::setupImage(const std::string& path) {
    auto fields = m_fields.self();
    if (fields->m_spr) fields->m_spr->removeFromParent();

    if (LevelEditorLayer::get()) {
        auto decodedRes = utils::base64::decodeString(path);
        if (!decodedRes) return;

        auto u16Res = utils::string::utf8ToUtf16(decodedRes.unwrap());
        if (!u16Res) return;

        std::filesystem::path decoded = u16Res.unwrap();

        if (std::filesystem::exists(decoded) && !std::filesystem::is_directory(decoded)) {
            fields->m_spr = LazySprite::create({60.f, 60.f}, true);
            fields->m_spr->setZOrder(1);
            fields->m_spr->setPosition(getContentSize() / 2.f);
            fields->m_spr->setID("image-reference"_spr);
            addChild(fields->m_spr);

            fields->m_spr->setLoadCallback([this, fields](Result<> res) {
                if (res) setAttributes();
                else {
                    for (auto child : getChildrenExt()) {
                        child->setVisible(true);
                    }
                    fields->m_spr->removeFromParent();
                    fields->m_spr = nullptr;
                    onImageFail("image-btn.png"_spr, "Image Not Found");
                }
            });

            fields->m_spr->loadFromFile(decoded, LazySprite::Format::kFmtUnKnown, true);
        }
        else {
            onImageFail("image-btn.png"_spr, "Image Not Found");
        }
    }
    else {
        onImageFail("image-btn.png"_spr, "Reference Hidden\nDelete Before Upload");
    }
}

void RITextGameObject::customObjectSetup(gd::vector<gd::string>& values, gd::vector<void*>& exists) {
    TextGameObject::customObjectSetup(values, exists);
    setupCustomSprite();
}

void RITextGameObject::updateTextObject(gd::string text, bool defaultFont) {
    TextGameObject::updateTextObject(text, defaultFont);
    if (!LevelEditorLayer::get()) return;
    setupCustomSprite();
}

void RIEditorUI::onImport(CCObject* sender) {
    utils::file::FilePickOptions options;
    options.filters.push_back({"Images", {"*.png", "*.apng", "*.jpg", "*.jpeg", "*.jfif", "*.pjpeg", "*.pjp", "*.webp", "*.gif", "*.bmp", "*.jxl"}});

    async::spawn(
        utils::file::pick(file::PickMode::OpenFile, options),
        [this](Result<std::optional<std::filesystem::path>> result) {
            if (!result.isOk()) return;
            auto opt = result.unwrap();
            if (!opt) return;

            auto path = opt.value();

            auto obj = m_selectedObject;

            CCPoint pos;

            if (obj) {
                pos = obj->getPosition();
            }
            else {
                auto winSize = CCDirector::get()->getWinSize();
                auto localPosAR = m_editorLayer->m_objectLayer->convertToNodeSpaceAR(winSize / 2.f);
                pos = CCPoint{localPosAR.x, localPosAR.y + tinker::utils::getToolbarHeight()};
            }

            std::string objStr = fmt::format("1,914,2,{},3,{},31,{}", pos.x, pos.y, utils::base64::encode("image:" + utils::base64::encode(utils::string::pathToString(path))));
            auto objects = pasteObjects(objStr, true, true);
            
            for (auto obj : objects->asExt<GameObject*>()) {
                obj->m_positionX = pos.x;
                obj->m_positionY = pos.y;

                obj->setPosition(pos);
            }

            updateButtons();
            updateObjectInfoLabel();
        }
    );
}
