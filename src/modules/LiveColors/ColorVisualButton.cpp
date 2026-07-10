#include "ColorVisualButton.hpp"
#include "utils/Constants.hpp"

using namespace tinker::ui;

ColorVisualButton* ColorVisualButton::create(EditorUI* editorUI) {
    auto ret = new ColorVisualButton();
    if (ret->init(editorUI)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
};

bool ColorVisualButton::init(EditorUI* editorUI) {
    m_colorChannelSprite = ColorChannelSprite::create();
    CCMenuItemSpriteExtra::init(m_colorChannelSprite, nullptr, this, menu_selector(ColorVisualButton::openColorPicker));
    m_editorUI = editorUI;

    m_IDLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_IDLabel->setScale(0.4f);
    m_IDLabel->setPositionX(getContentSize().width / 2.f);
    m_IDLabel->setPositionY(getContentSize().height / 2.f + 1.f);

    addChild(m_IDLabel);

    setVisible(false);
    return true;
}

void ColorVisualButton::setColorData(int id, const ccColor3B& color, bool blending, float opacity, ColorAction* action) {
    m_currentColorID = id;
    m_IDLabel->setString(idToString(id).c_str());
    m_IDLabel->setScale(0.4f);
    m_action = action;
    m_colorChannelSprite->setColor(color);
    m_colorChannelSprite->updateBlending(blending);
    m_colorChannelSprite->updateOpacity(opacity);
    if (action) {
        if (action->m_copyID > 0) {
            m_IDLabel->setString(fmt::format("{}>{}", idToString(id), idToString(action->m_copyID)).c_str());
            m_IDLabel->setScale(0.3f);
        }
    }

    setVisible(id != -1);
}

void ColorVisualButton::openColorPicker(CCObject* obj) {
    if (m_colorSetupLayer) {
        m_colorSetupLayer->m_colorSprites->removeAllObjects();
        m_colorSetupLayer->m_colorLabels->removeAllObjects();
    }
    m_colorSetupLayer = GJColorSetupLayer::create(m_editorUI->m_editorLayer->m_levelSettings);
    CCTouchDispatcher::get()->removeDelegate(m_colorSetupLayer);
    CCTouchDispatcher::get()->unregisterForcePrio(m_colorSetupLayer);

    m_colorSetupLayer->m_page = (m_currentColorID - 1) / m_colorSetupLayer->m_colorsPerPage;
    int idx = ((m_currentColorID - 1) % m_colorSetupLayer->m_colorsPerPage) + 1;
    obj->setTag(idx);
    m_colorSetupLayer->onColor(obj);
}

std::string ColorVisualButton::idToString(int ID) {
    using namespace tinker::constants::color_channels;

    switch (ID) {
        case Background: return "BG";
        case Ground1: return "G1";
        case Line: return "L";
        case Line3D: return "3DL";
        case Obj: return "Obj";
        case PlayerColor1: return "P1";
        case PlayerColor2: return "P2";
        case LightBackground: return "LBG";
        case Ground2: return "G2";
        case Black: return "B";
        case White: return "W";
        case Lighter: return "LTR";
        case Middleground1: return "MG";
        case Middleground2: return "MG2";
        default: return utils::numToString(ID);
    }
}

ColorVisualButton::~ColorVisualButton() {
    if (m_colorSetupLayer) {
        m_colorSetupLayer->m_colorSprites->removeAllObjects();
    }
}