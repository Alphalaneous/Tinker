#include "nodes/ColorChannelSprite.hpp"
#include "utils/Constants.hpp"
#include "utils/Utils.hpp"
#include "modules/ImprovedColorPicker.hpp"

namespace tinker::ui {

ColorChannelSprite* ColorChannelSprite::create(int colorID, bool showLabel) {
    auto ret = new ColorChannelSprite();
    if (ret->init(colorID, showLabel)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ColorChannelSprite::init(int colorID, bool showLabel) {
    using namespace tinker::constants::color_channels;

    if (!CCSprite::initWithSpriteFrameName("GJ_colorBtn_001.png")) return false;

    m_colorID = colorID;

    setScale(0.8f);
    setID("channel-sprite"_spr);

    m_selectSpr = CCSprite::createWithSpriteFrameName("GJ_select_001.png");
    m_selectSpr->setScale(1.1f);
    m_selectSpr->setID("selected-indicator"_spr);
    m_selectSpr->setPosition(getContentSize() / 2.f);
    m_selectSpr->setVisible(false);
    addChild(m_selectSpr);

    if (showLabel) {
        m_idLabel = CCLabelBMFont::create("", "bigFont.fnt");
        m_idLabel->limitLabelWidth(25.f, 0.375f, 0.2f);
        m_idLabel->setPosition(getContentSize() / 2.f + CCPoint{0.f, 0.5f});
        m_idLabel->setID("id-label"_spr);

        addChild(m_idLabel);
    }

    m_copyLabel = CCLabelBMFont::create("", "chatFont.fnt");
    m_copyLabel->setScale(0.6f);
    m_copyLabel->setAnchorPoint({1.f, 1.f});
    m_copyLabel->setPosition(getContentSize() - CCPoint{4.5f, 3.5f});
    m_copyLabel->setID("copy-label"_spr);

    addChild(m_copyLabel);
    
    m_opacityLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_opacityLabel->setScale(0.25f);
    m_opacityLabel->setAnchorPoint({0.5f, 0.f});
    m_opacityLabel->setPosition({getContentSize().width / 2.f, 3.5f});
    m_opacityLabel->setID("opacity-label"_spr);

    addChild(m_opacityLabel);

    m_blendingLabel = CCLabelBMFont::create("•", "bigFont.fnt");
    m_blendingLabel->setScale(0.5f);
    m_blendingLabel->setAnchorPoint({0.f, 1.f});
    m_blendingLabel->setPosition({3.5f, getContentHeight() + 2.5f});
    m_blendingLabel->setVisible(false);
    m_blendingLabel->setID("blending-label"_spr);

    addChild(m_blendingLabel);

    updateSprite();
    
    return true;
}

std::string ColorChannelSprite::textForID(int colorID, bool any) {
    using namespace tinker::constants::color_channels;

    std::string text;

    if ((colorID != Black && colorID != White && colorID != -2) || any) {
        auto iter = ColorNamesShort.find(colorID);
        if (iter == ColorNamesShort.end()) {
            text = geode::utils::numToString(colorID);
        }
        else {
            text = iter->second;
        }
    }

    return text;
}

void ColorChannelSprite::setColorID(int colorID) {
    m_colorID = colorID;
    updateSprite();
}

void ColorChannelSprite::setLive(bool live) {
    m_live = live;
}

bool ColorChannelSprite::isLive() {
    return m_live;
}

void ColorChannelSprite::updateSprite() {
    using namespace tinker::constants::color_channels;

    auto color = ccColor3B{255, 255, 255};
    m_realOpacity = 255;

    m_copyLabel->setString("");
    m_opacityLabel->setString("");
    m_blendingLabel->setVisible(false);

    switch (m_colorID) {
        case -2: {
            m_realOpacity = 105;
            color = {20, 20, 20};
            break;
        }
        case Black: {
            color = {0, 0, 0};
            break;
        }
        case LightBackground: {
            m_realOpacity = 120;
            break;
        }
        case Default: {
            m_realOpacity = 50;
            break;
        }
        default: {
            ColorAction* action = nullptr;

            auto editor = LevelEditorLayer::get();

            if (m_live) {
                auto colorData = utils::getActiveColor(editor, m_colorID);

                color = colorData.color;
                action = colorData.action;

                if (colorData.opacity < 255) {
                    m_opacityLabel->setString(numToString(colorData.opacity / 255.f, 2).c_str());
                }
            }
            else {
                auto allActions = editor->m_levelSettings->m_effectManager->getAllColorActions();

                for (auto colorAction : allActions->asExt<ColorAction>()) {
                    if (colorAction->m_colorID == m_colorID) {
                        action = colorAction;
                        break;
                    }
                }

                if (action) {
                    color = action->m_fromColor;

                    if (ImprovedColorPicker::getSetting<bool, "show-copy-color">()) {
                        if (action->m_copyID != 0) {
                            color = tinker::utils::getRealizedColor(action->m_colorID);
                        }
                    }

                    if (action->m_copyID == 0) {
                        if (action->m_playerColor == 1) {
                            color = tinker::utils::getRealizedColor(PlayerColor1);
                        }
                        else if (action->m_playerColor == 2) {
                            color = tinker::utils::getRealizedColor(PlayerColor2);
                        }
                    }

                    if (action->m_fromOpacity < 1) {
                        m_opacityLabel->setString(numToString(action->m_fromOpacity, 2).c_str());
                    }
                }
            }

            if (action) {
                if (action->m_copyID != 0) {
                    m_copyLabel->setString(textForID(action->m_copyID, true).c_str());
                }
                else {
                    if (action->m_playerColor == 1) {
                        m_copyLabel->setString("P1");
                    }
                    else if (action->m_playerColor == 2) {
                        m_copyLabel->setString("P2");
                    }
                }

                m_blendingLabel->setVisible(action->m_blending);
            }
        }
    }

    if (m_idLabel) {
        m_idLabel->setString(textForID(m_colorID, false).c_str());
    }

    m_copyLabel->setColor(tinker::utils::color::getContrastingColor(color));
    setColor(color);
    setOpacity(m_realOpacity / (m_enabled ? 1.f : 5.f));
}

void ColorChannelSprite::setSelected(bool selected) {
    m_selectSpr->setVisible(selected);
}

void ColorChannelSprite::setEnabled(bool enabled) {
    m_enabled = enabled;
    setOpacity(m_realOpacity / (m_enabled ? 1.f : 5.f));
    if (m_idLabel) {
        m_idLabel->setOpacity(enabled ? 255 : 50);
    }
}

int ColorChannelSprite::getColorID() {
    return m_colorID;
}

}