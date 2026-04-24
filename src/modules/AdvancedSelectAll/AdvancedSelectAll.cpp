#include "AdvancedSelectAll.hpp"
#include <algorithm>
#include <typeinfo>

// class : AdvancedSelectAll
void AdvancedSelectAll::onEditor() {
    auto fields = static_cast<ASAEditorUI*>(m_editorUI)->m_fields.self();

    fields->m_selectAllOfObjectButton = m_editorUI->getSpriteButton("select-all-of-object.png"_spr, menu_selector(ASAEditorUI::onSelectAllOfObjectButton), nullptr, 0.9f);
    fields->m_selectAllOfObjectButton->setID("select-all-of-object"_spr);
    m_editorUI->m_editButtonBar->m_buttonArray->addObject(fields->m_selectAllOfObjectButton);

    fields->m_selectAllOfBaseColorButton = m_editorUI->getSpriteButton("select-all-of-base-color.png"_spr, menu_selector(ASAEditorUI::onSelectAllOfBaseColorButton), nullptr, 0.9f);
    fields->m_selectAllOfBaseColorButton->setID("select-all-of-base-color"_spr);
    m_editorUI->m_editButtonBar->m_buttonArray->addObject(fields->m_selectAllOfBaseColorButton);

    fields->m_selectAllOfDetailColorButton = m_editorUI->getSpriteButton("select-all-of-detail-color.png"_spr, menu_selector(ASAEditorUI::onSelectAllOfDetailColorButton), nullptr, 0.9f);
    fields->m_selectAllOfDetailColorButton->setID("select-all-of-detail-color"_spr);
    m_editorUI->m_editButtonBar->m_buttonArray->addObject(fields->m_selectAllOfDetailColorButton);
}

// class : ASAEditorUI
void ASAEditorUI::updateButtons() {
    EditorUI::updateButtons();
    auto fields = m_fields.self();
    if (!fields->m_selectAllOfObjectButton) return;

    if (m_selectedObject || (m_selectedObjects && m_selectedObjects->count() > 0)) {
        setButtonColor(fields->m_selectAllOfObjectButton, {255, 255, 255});
        setButtonColor(fields->m_selectAllOfBaseColorButton, {255, 255, 255});
        setButtonColor(fields->m_selectAllOfDetailColorButton, {255, 255, 255});

        fields->m_selectAllOfObjectButton->setEnabled(true);
        fields->m_selectAllOfBaseColorButton->setEnabled(true);
        fields->m_selectAllOfDetailColorButton->setEnabled(true);
    }
    else {
        setButtonColor(fields->m_selectAllOfObjectButton, {166, 166, 166});
        setButtonColor(fields->m_selectAllOfBaseColorButton, {166, 166, 166});
        setButtonColor(fields->m_selectAllOfDetailColorButton, {166, 166, 166});

        fields->m_selectAllOfObjectButton->setEnabled(false);
        fields->m_selectAllOfBaseColorButton->setEnabled(false);
        fields->m_selectAllOfDetailColorButton->setEnabled(false);
    }
}
void ASAEditorUI::onSelectAllOfObjectButton(CCObject* sender) {
    // get the currently selected objects first, so we can get a profile of what object IDs should be selected.
    std::vector<int> objIDProfile{};
    if (m_selectedObject != nullptr && m_selectedObjects->count() == 0) {
        objIDProfile.push_back(m_selectedObject->m_objectID);
    }
    else {
        CCArrayExt<GameObject*> selectedObjectsExt = m_selectedObjects;
        for (auto* gObj : selectedObjectsExt) {
            // prevent duplicates
            if (std::find(objIDProfile.begin(), objIDProfile.end(), gObj->m_objectID) != objIDProfile.end()) continue;
            objIDProfile.push_back(gObj->m_objectID);
        }
    }

    // ok now go, select all the objects we profiled
    CCArray* allObjects = m_editorLayer->getAllObjects();
    CCArray* objectsToSelectList = CCArray::create();
    for (int i = 0; i < allObjects->count(); i++) {
        GameObject* gObj = static_cast<GameObject*>(allObjects->objectAtIndex(i));
        if (std::find(objIDProfile.begin(), objIDProfile.end(), gObj->m_objectID) != objIDProfile.end())
            objectsToSelectList->addObject(gObj);
    }
    EditorUI::selectObjects(objectsToSelectList, false);
}
void ASAEditorUI::onSelectAllOfBaseColorButton(CCObject* sender) {
    // get the currently selected objects first, so we can get a profile of what color IDs should be selected.
    std::vector<int> mainColorIDProfile{};
    if (m_selectedObject != nullptr && m_selectedObjects->count() == 0) {
        mainColorIDProfile.push_back(m_selectedObject->m_activeMainColorID);
    }
    else {
        CCArrayExt<GameObject*> selectedObjectsExt = m_selectedObjects;
        for (auto* gObj : selectedObjectsExt) {
            // prevent duplicates
            if (std::find(mainColorIDProfile.begin(), mainColorIDProfile.end(), gObj->m_activeMainColorID) != mainColorIDProfile.end()) continue;
            mainColorIDProfile.push_back(gObj->m_activeMainColorID);
        }
    }

    // ok now go, select all the objects we profiled
    CCArray* allObjects = m_editorLayer->getAllObjects();
    CCArray* objectsToSelectList = CCArray::create();
    for (int i = 0; i < allObjects->count(); i++) {
        GameObject* gObj = static_cast<GameObject*>(allObjects->objectAtIndex(i));
        if (std::find(mainColorIDProfile.begin(), mainColorIDProfile.end(), gObj->m_activeMainColorID) != mainColorIDProfile.end())
            objectsToSelectList->addObject(gObj);
    }
    EditorUI::selectObjects(objectsToSelectList, false);
}
void ASAEditorUI::onSelectAllOfDetailColorButton(CCObject* sender) {
    // get the currently selected objects first, so we can get a profile of what color IDs should be selected.
    std::vector<int> detailColorIDProfile{};
    if (m_selectedObject != nullptr && m_selectedObjects->count() == 0) {
        if (!m_selectedObject->hasSecondaryColor()) return;
        detailColorIDProfile.push_back(m_selectedObject->m_activeDetailColorID);
    }
    else {
        CCArrayExt<GameObject*> selectedObjectsExt = m_selectedObjects;
        for (auto* gObj : selectedObjectsExt) {
            if (!gObj->hasSecondaryColor()) continue;

            // prevent duplicates
            if (std::find(detailColorIDProfile.begin(), detailColorIDProfile.end(), gObj->m_activeDetailColorID) != detailColorIDProfile.end()) continue;
            detailColorIDProfile.push_back(gObj->m_activeDetailColorID);
        }
    }

    // ok now go, select all the objects we profiled
    CCArray* allObjects = m_editorLayer->getAllObjects();
    CCArray* objectsToSelectList = CCArray::create();
    for (int i = 0; i < allObjects->count(); i++) {
        GameObject* gObj = static_cast<GameObject*>(allObjects->objectAtIndex(i));

        if (!gObj->hasSecondaryColor()) continue;

        if (std::find(detailColorIDProfile.begin(), detailColorIDProfile.end(), gObj->m_activeDetailColorID) != detailColorIDProfile.end())
            objectsToSelectList->addObject(gObj);
    }
    EditorUI::selectObjects(objectsToSelectList, false);
}
void ASAEditorUI::setButtonColor(CCMenuItemSpriteExtra* btn, const ccColor3B& color) {
    auto btnSpr = btn->getChildByType<ButtonSprite*>(0);
    for (auto child : btnSpr->getChildrenExt()) {
        if (auto rgba = typeinfo_cast<CCNodeRGBA*>(child)) {
            rgba->setColor(color);
        }
    }
}