#include "MultiTextEdit.hpp"

bool MTECustomizeObjectLayer::init(GameObject* object, CCArray* objects) {
    if (object) return CustomizeObjectLayer::init(object, objects);

    for (auto obj : CCArrayExt<GameObject*>(objects)) {
        if (obj->m_objectID != 914) {
            return CustomizeObjectLayer::init(object, objects);
        }
    }

    auto fields = m_fields.self();

    fields->m_textObjects = true;
    auto ret = CustomizeObjectLayer::init(static_cast<GameObject*>(objects->objectAtIndex(0)), CCArray::create());
    m_targetObjects = objects;
    m_targetObject = nullptr;

    auto str = objects->asExt<TextGameObject>()[0]->m_text;
    bool sharedString = true;
    for (auto obj : m_targetObjects->asExt<TextGameObject>()) {
        if (obj->m_text != str) {
            sharedString = false;
            break;
        }
    }

    if (!sharedString) {
        m_textInput->setString("");
    }

    fields->m_allowChange = true;

    return ret;
};

void MTECustomizeObjectLayer::sliderChanged(CCObject* sender) {
    auto fields = m_fields.self();
    if (!fields->m_textObjects) return CustomizeObjectLayer::sliderChanged(sender);

    auto thumb = static_cast<SliderThumb*>(sender);
    m_kerningAmount = thumb->getValue() * 30.0f - 10.0f;

    for (auto object : CCArrayExt<TextGameObject*>(m_targetObjects)) {
        object->updateTextKerning(m_kerningAmount);
        object->updateTextObject(object->m_text, false);
    }

    m_kerningLabel->setString(fmt::format("Kerning: {}", m_kerningAmount).c_str());
    m_kerningLabel->setExtraKerning(m_kerningAmount);
    m_textInput->m_kerningAmount = m_kerningAmount;
    m_textInput->updateLabel(fields->m_text);
}

void MTECustomizeObjectLayer::onClear(CCObject* sender) {
    auto fields = m_fields.self();
    if (!fields->m_textObjects) return CustomizeObjectLayer::onClear(sender);
    fields->m_text = "";

    for (auto object : CCArrayExt<TextGameObject*>(m_targetObjects)) {
        object->updateTextObject(fields->m_text, false);
    }
    m_textInput->setString(fields->m_text);
    
    CustomizeObjectLayer::onClear(sender);
}

void MTECustomizeObjectLayer::textChanged(CCTextInputNode* node) {
    auto fields = m_fields.self();
    if (!fields->m_textObjects) return CustomizeObjectLayer::textChanged(node);

    if (!fields->m_allowChange) return;

    fields->m_text = node->getString();
    
    for (auto object : CCArrayExt<TextGameObject*>(m_targetObjects)) {
        object->updateTextObject(fields->m_text, false);
    }
}

void MTECustomizeObjectLayer::onClose(CCObject* sender) {
    auto fields = m_fields.self();
    if (fields->m_textObjects) {
        m_targetObject = static_cast<GameObject*>(m_targetObjects->objectAtIndex(0));
        m_targetObjects = nullptr;

        m_textInput->setString(fields->m_text);
    }
    CustomizeObjectLayer::onClose(sender);
}

void MTECustomizeObjectLayer::onSelectMode(cocos2d::CCObject* sender) {
    CustomizeObjectLayer::onSelectMode(sender);
    auto fields = m_fields.self();
    if (fields->m_textObjects) {
        auto textActionsMenu = m_mainLayer->getChildByID("text-actions-menu");
        if (textActionsMenu) {
            textActionsMenu->setVisible(false);
        }
    }
}