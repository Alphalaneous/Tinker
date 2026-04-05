#include "ReferenceImage.hpp"
#include "../Utils.hpp"
#include <Geode/utils/base64.hpp>
#include <Geode/utils/async.hpp>

bool RIEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) return false;
    auto btn = getSpriteButton("image-btn.png"_spr, menu_selector(RIEditorUI::onImport), nullptr, 0.9f);
    btn->setID("reference-import"_spr);
    m_editButtonBar->m_buttonArray->addObject(btn);

    auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
    auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

    m_editButtonBar->reloadItems(cols, rows);
    return true;
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

void RITextGameObject::onImageFail() {
    for (auto child : getChildrenExt()) {
        child->setVisible(false);
    }
    auto node = CCNodeRGBA::create();
    node->setAnchorPoint({0.5f, 0.5f});
    node->ignoreAnchorPointForPosition(false);
    node->setID("error-node"_spr);

    node->setCascadeColorEnabled(true);
    node->setCascadeOpacityEnabled(true);

    auto spr = CCSprite::createWithSpriteFrameName("image-btn.png"_spr);
    node->addChild(spr);

    node->setContentSize(spr->getContentSize());
    node->setPosition(node->getContentSize()/2);
    spr->setPosition(node->getContentSize()/2);

    auto label = CCLabelBMFont::create("Image Not Found", "chatFont.fnt");
    label->setAnchorPoint({0.5f, 0.f});
    label->setPositionX(node->getContentWidth()/2);
    label->setPositionY(node->getContentHeight() + 3);

    node->addChild(label);
    addChild(node);
    
    setContentSize(node->getContentSize());
    m_width = getContentWidth();
    m_height = getContentHeight();
    updateOrientedBox();
}


bool RITextGameObject::isReferenceImage() {
    return m_fields->m_isReferenceImage;
}

void RITextGameObject::setAttributes() {
    auto fields = m_fields.self();
    if (auto node = getChildByID("error-node"_spr)) {
        node->removeFromParent();
    }
    setContentSize(fields->m_spr->getContentSize());
    fields->m_spr->setPosition(getContentSize()/2);
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
    if (!LevelEditorLayer::get()) {
        updateTextObject("[Path Hidden, Delete Object Before Upload!]", false);
        return false;
    }
    else {
        m_addToNodeContainer = true;
        for (auto child : getChildrenExt()) {
            child->setVisible(false);
        }
    }
    
    if (path.empty()) return false;
    return true;
}


void RITextGameObject::setupImage(const std::string& path) {
    auto fields = m_fields.self();
    if (fields->m_spr) fields->m_spr->removeFromParent();

    auto decodedRes = utils::base64::decodeString(path);
    if (!decodedRes) return;

    auto u16Res = utils::string::utf8ToUtf16(decodedRes.unwrap());
    if (!u16Res) return;

    std::filesystem::path decoded = u16Res.unwrap();

    fields->m_isReferenceImage = true;

    if (std::filesystem::exists(decoded) && !std::filesystem::is_directory(decoded)) {
        fields->m_spr = LazySprite::create({60, 60}, true);
        fields->m_spr->setZOrder(1);
        fields->m_spr->setPosition(getContentSize()/2);
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
                onImageFail();
            }
        });

        fields->m_spr->loadFromFile(decoded, LazySprite::Format::kFmtUnKnown, true);
    }
    else {
        onImageFail();
    }
}

void RITextGameObject::customObjectSetup(gd::vector<gd::string>& p0, gd::vector<void*>& p1) {
    TextGameObject::customObjectSetup(p0, p1);
    setupCustomSprite();
}

void RITextGameObject::updateTextObject(gd::string p0, bool p1) {
    TextGameObject::updateTextObject(p0, p1);
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

            std::string objStr = fmt::format("1,914,31,{}", utils::base64::encode("image:" + utils::base64::encode(utils::string::pathToString(path))));
            auto objects = pasteObjects(objStr, true, true);

            CCPoint pos;

            if (obj) {
                pos = obj->getPosition();
            }
            else {
                auto winSize = CCDirector::get()->getWinSize();
                auto localPosAR = m_editorLayer->m_objectLayer->convertToNodeSpaceAR(winSize/2);
                pos = CCPoint{localPosAR.x, localPosAR.y + m_toolbarHeight};
            }

            for (auto obj : objects->asExt<GameObject*>()) {
                obj->setPosition(pos);
            }

            updateButtons();
            updateObjectInfoLabel();
        }
    );
}
