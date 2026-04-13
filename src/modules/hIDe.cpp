#include "hIDe.hpp"

bool hIDe::onToggled(bool state) {
    return true;
}

bool hIDe::onSettingChanged(std::string_view key, const matjson::Value& value) {
    return true;
}

bool HDEditLevelLayer::init(GJGameLevel* level) {
    if (!EditLevelLayer::init(level)) return false;
    
    auto fields = m_fields.self();

    fields->m_label = typeinfo_cast<CCLabelBMFont*>(getChildByID("level-id-label"));
    fields->m_replacementLabel = CCLabelBMFont::create("ID: Hidden (shift)", "goldFont.fnt");

    if (std::string_view(fields->m_label->getString()) == "ID: na") {
        fields->m_shouldSetVisibility = false;
        return true;
    }
    else {
        if (auto menu = typeinfo_cast<CCMenu*>(getChildByID("cvolton.betterinfo/level-id-menu"))) {
            CCMenuItemSpriteExtra* sprite = typeinfo_cast<CCMenuItemSpriteExtra*>(menu->getChildrenExt()[0]);

            fields->m_label = typeinfo_cast<CCLabelBMFont*>(sprite->getChildrenExt()[0]);

            fields->m_replacementLabel->setPosition(fields->m_label->getPosition());
            fields->m_replacementLabel->setScale(fields->m_label->getScale());
            fields->m_replacementLabel->setAnchorPoint(fields->m_label->getAnchorPoint());

            sprite->addChild(fields->m_replacementLabel);
        }
        else {
            fields->m_replacementLabel->setPosition(fields->m_label->getPosition());
            fields->m_replacementLabel->setScale(fields->m_label->getScale());
            fields->m_replacementLabel->setAnchorPoint(fields->m_label->getAnchorPoint());

            addChild(fields->m_replacementLabel);
        }

        fields->m_label->setVisible(false);
    }

    schedule(schedule_selector(HDEditLevelLayer::checkShift));

    return true;
}

void HDEditLevelLayer::setIDVisible(bool visible) {
    auto fields = m_fields.self();
    fields->m_label->setVisible(visible);
    fields->m_replacementLabel->setVisible(!visible);
}

void HDEditLevelLayer::checkShift(float dt) {
    setIDVisible(CCKeyboardDispatcher::get()->getShiftKeyPressed());
}