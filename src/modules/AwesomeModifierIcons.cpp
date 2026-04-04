#include "AwesomeModifierIcons.hpp"
#include <ranges>

std::unordered_map<int, std::string> AwesomeModifierIcons::s_textureMap = {
    {1755, "d_block.png"_spr},
    {1813, "j_block.png"_spr},
    {1829, "s_block.png"_spr},
    {1859, "h_block.png"_spr},
    {2866, "f_block.png"_spr}
};

void AMIEffectGameObject::customSetup() {
    EffectGameObject::customSetup();
    auto module = AwesomeModifierIcons::get();
    if (!AwesomeModifierIcons::s_textureMap.contains(m_objectID)) return;

    if (!module->getSetting<bool, "solid-border">()) {
        if (auto newSpr = CCSprite::createWithSpriteFrameName("edit_eCollisionBlock01_001.png")) {
            setTexture(newSpr->getTexture());
            setTextureRect(newSpr->getTextureRect());
        }
    }
    
    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    auto spr = CCSprite::create(AwesomeModifierIcons::s_textureMap[m_objectID].c_str());
    if (!spr) return;

    spr->setScale(0.9f);
    addChildAtPosition(spr, Anchor::Center);

    queueInMainThread([module, self = Ref(this)] {
        auto label = self->getChildByType<CCLabelBMFont>(0);
        if (!label) return;
        if (module->getSetting<bool, "show-letter">()) {
            label->setPosition({2, self->getContentHeight()});
            label->setScale(0.3f);
            label->setAnchorPoint({0, 1});
        }
        else {
            label->setVisible(false);
        }
    });
}

bool AMIEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) return false;

    if (!AwesomeModifierIcons::getSetting<bool, "move-f-block">()) return true;

    auto buttonBars = m_createButtonBars->asExt<EditButtonBar>();

    auto portalTabIt = std::find_if(buttonBars.begin(), buttonBars.end(),
        [](auto bar) { 
            return bar && bar->m_tabIndex == 5; 
        }
    );

    if (portalTabIt == buttonBars.end()) return true;
    auto portalTab = *portalTabIt;

    auto buttons = portalTab->m_buttonArray;
    auto buttonsExt = buttons->asExt<CreateMenuItem>();

    Ref<CreateMenuItem> fItem = nullptr;
    int hIndex = -1;
    int fIndex = -1;

    for (const auto& [index, item] : asp::iter::from(buttonsExt).enumerate()) {
        if (!item) continue;

        if (item->m_objectID == 2866) {
            fIndex = index;
            fItem = item;
        }
        else if (item->m_objectID == 1859) {
            hIndex = index;
        }
        if (fItem && hIndex != -1) {
            break;
        }
    }

    if (fItem && hIndex != -1) {
        buttons->removeObject(fItem, false);
        if (fIndex < hIndex) hIndex--;
        buttons->insertObject(fItem, hIndex + 1);
    }

    auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
    auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

    portalTab->reloadItems(cols, rows);

    return true;
}