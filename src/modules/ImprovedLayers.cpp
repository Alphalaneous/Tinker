#include "modules/ImprovedLayers.hpp"
#include "InputsHandler.hpp"
#include "modules/UIScaling.hpp"
#include "nodes/LabelListener.hpp"
#include "utils/next-free/NextFreeProvider.hpp"
#include "utils/next-free/sources/EditorLayerSource.hpp"
#include <alphalaneous.editorsounds/include/API.hpp>

bool ImprovedLayers::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        auto editor = getEditor();

        editor->m_uiItems->removeObject(m_layerInput);
        m_layerInput->removeFromParent();
        m_layerInput = nullptr;

        editor->m_uiItems->removeObject(m_nextFreeBtn);
        m_nextFreeBtn->removeFromParent();
        m_nextFreeBtn = nullptr;

        m_labelListener = nullptr;

        revertLayerMenu();
    }
    return true;
}

void ImprovedLayers::onEditor() {
    auto editor = getEditor();

    // temporary BetterEdit compat
    editor->runAction(CallFuncExt::create([this, editor] {
        cleanupLayerMenu();

        m_layerInput = tinker::ui::LayerInput::create();
        m_layerInput->setZOrder(3);
        m_layerInput->setID("layer-input"_spr);

        editor->m_uiItems->addObject(m_layerInput);

        m_labelListener = tinker::ui::LabelListener::create(editor->m_currentLayerLabel->getString(), "bigFont.fnt", [this, editor] (ZStringView text) {
            m_layerInput->setText(text);
            updateDisplay();
        });

        editor->m_currentLayerLabel = m_labelListener;

        m_nextFreeBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_arrow_02_001.png", 0.54f, [this, editor] (auto sender) {
            auto nextLayer = NextFreeProvider::get()->nextFree<EditorLayerSource>().unwrapOr(-1);
            m_layerInput->setText(numToString(nextLayer).c_str(), true);
            editor->m_goToBaseBtn->setVisible(true);
        });
        auto spr = static_cast<CCSprite*>(m_nextFreeBtn->getNormalImage());
        spr->setFlipX(true);
        spr->setOpacity(175);
        m_nextFreeBtn->setID("next-free-button"_spr);
        m_nextFreeBtn->setZOrder(5);
        m_nextFreeBtn->setScale(0.8f);
        m_nextFreeBtn->m_baseScale = 0.8f;

        alpha::editor_sounds::assignToMenuItem(m_nextFreeBtn, "next-page");

        editor->m_uiItems->addObject(m_nextFreeBtn);

        editor->m_goToBaseBtn->setZOrder(1);
        m_oldScales[editor->m_goToBaseBtn] = editor->m_goToBaseBtn->getScale();
        m_oldPositions[editor->m_goToBaseBtn] = editor->m_goToBaseBtn->getPosition();
        editor->m_goToBaseBtn->setScale(0.8f);
        editor->m_goToBaseBtn->m_baseScale = 0.8f;

        editor->m_layerPrevBtn->setZOrder(2);
        m_oldScales[editor->m_layerPrevBtn] = editor->m_layerPrevBtn->getScale();
        m_oldPositions[editor->m_layerPrevBtn] = editor->m_layerPrevBtn->getPosition();

        editor->m_layerNextBtn->setZOrder(4);
        m_oldScales[editor->m_layerNextBtn] = editor->m_layerNextBtn->getScale();
        m_oldPositions[editor->m_layerNextBtn] = editor->m_layerNextBtn->getPosition();

        auto layerMenu = editor->getChildByID("layer-menu");
        if (layerMenu) {
            auto layout = static_cast<RowLayout*>(layerMenu->getLayout());
            layout->setAutoScale(false);
            layout->setAutoGrowAxis(0.f);
            layout->setGap(4.f);

            layerMenu->addChild(m_layerInput);
            layerMenu->addChild(m_nextFreeBtn);

            layerMenu->updateLayout();

            auto winSize = CCDirector::get()->getWinSize();
            layerMenu->setPositionX(winSize.width - layerMenu->getScaledContentWidth() / 2.f - 6.f * UIScaling::getScale() - UIScaling::getSafeOffset().x);
        }
    }));
}

void ImprovedLayers::updateDisplay() {
    if (!m_layerInput) return;

    m_layerInput->updateDisplay();
}

void ImprovedLayers::cleanupLayerMenu() {
    auto editor = getEditor();
    m_oldLabel = editor->m_currentLayerLabel;
    m_oldScales[m_oldLabel] = m_oldLabel->getScale();
    m_oldPositions[m_oldLabel] = m_oldLabel->getPosition();

    m_oldLabel->removeFromParent();
    editor->m_uiItems->removeObject(m_oldLabel);

    for (auto child : editor->getChildrenExt()) {
        auto input = typeinfo_cast<TextInput*>(child);
        if (input && input->getInputNode()->m_caption == "Z") {
            m_oldBEInput = input;
            input->defocus();
            // cuz it still accepts input lol
            input->setPosition({99999, 99999});
            input->removeFromParent();

            editor->m_uiItems->removeObject(input);
        }
    }

    auto layerMenu = editor->getChildByID("layer-menu");
    if (layerMenu) {
        m_oldMenuSize = layerMenu->getContentSize();

        m_oldBELayerLock = layerMenu->getChildByID("hjfod.betteredit/lock-layer");
        m_oldBENextFree = layerMenu->getChildByID("hjfod.betteredit/next-free-layer-button");

        if (m_oldBELayerLock) {
            m_oldScales[m_oldBELayerLock] = m_oldBELayerLock->getScale();
            m_oldPositions[m_oldBELayerLock] = m_oldBELayerLock->getPosition();
            m_oldBELayerLock->removeFromParent();
        }
        if (m_oldBENextFree) {
            m_oldScales[m_oldBENextFree] = m_oldBENextFree->getScale();
            m_oldPositions[m_oldBENextFree] = m_oldBENextFree->getPosition();
            m_oldBENextFree->removeFromParent();
        }
    }

    m_oldLayerLockButton = editor->getChildByIDRecursive("lock-layer-button");
    if (m_oldLayerLockButton) {
        m_layerLockButtonParent = m_oldLayerLockButton->getParent();
        m_oldLayerLockButton->removeFromParent();
    }

    m_oldLockOpacity = editor->m_layerLockSprite->getOpacity();
    editor->m_layerLockSprite->setOpacity(0);
}

void ImprovedLayers::revertLayerMenu() {
    auto editor = getEditor();
    auto layerMenu = editor->getChildByID("layer-menu");

    auto isBase = editor->m_editorLayer->m_currentLayer == -1;
    auto isLocked = !isBase && editor->m_editorLayer->isLayerLocked(editor->m_editorLayer->m_currentLayer);

    if (m_oldLayerLockButton && m_layerLockButtonParent) {
        m_layerLockButtonParent->addChild(m_oldLayerLockButton);
    }

    if (editor->m_layerLockSprite) {
        editor->m_layerLockSprite->setOpacity(m_oldLockOpacity);
    }

    if (m_oldBEInput) {
        editor->m_uiItems->addObject(m_oldBEInput);
        editor->addChild(m_oldBEInput);
    }

    if (layerMenu) {
        layerMenu->setContentSize(m_oldMenuSize);

        auto layout = static_cast<RowLayout*>(layerMenu->getLayout());
        layout->setAutoScale(true);
        layout->setAutoGrowAxis(std::nullopt);
        layout->setGap(5.f);

        if (m_oldBELayerLock) {
            m_oldBELayerLock->setZOrder(0);
            m_oldBELayerLock->setScale(m_oldScales[m_oldBELayerLock]);
            m_oldBELayerLock->setPosition(m_oldPositions[m_oldBELayerLock]);
            static_cast<CCMenuItemSpriteExtra*>(m_oldBELayerLock.data())->m_baseScale = m_oldScales[m_oldBELayerLock];
            layerMenu->addChild(m_oldBELayerLock);
        }
        if (m_oldBENextFree) {
            m_oldBENextFree->setZOrder(0);
            m_oldBENextFree->setScale(m_oldScales[m_oldBENextFree]);
            m_oldBENextFree->setPosition(m_oldPositions[m_oldBENextFree]);
            static_cast<CCMenuItemSpriteExtra*>(m_oldBENextFree.data())->m_baseScale = m_oldScales[m_oldBENextFree];
            layerMenu->addChild(m_oldBENextFree);
        }

        editor->m_goToBaseBtn->setZOrder(0);
        editor->m_goToBaseBtn->setScale(m_oldScales[editor->m_goToBaseBtn]);
        editor->m_goToBaseBtn->m_baseScale = m_oldScales[editor->m_goToBaseBtn];
        editor->m_goToBaseBtn->setPosition(m_oldPositions[editor->m_goToBaseBtn]);

        editor->m_layerPrevBtn->setZOrder(0);
        editor->m_layerPrevBtn->setScale(m_oldScales[editor->m_layerPrevBtn]);
        editor->m_layerPrevBtn->m_baseScale = m_oldScales[editor->m_layerPrevBtn];
        editor->m_layerPrevBtn->setPosition(m_oldPositions[editor->m_layerPrevBtn]);

        editor->m_layerNextBtn->setZOrder(0);
        editor->m_layerNextBtn->setScale(m_oldScales[editor->m_layerNextBtn]);
        editor->m_layerNextBtn->m_baseScale = m_oldScales[editor->m_layerNextBtn];
        editor->m_layerNextBtn->setPosition(m_oldPositions[editor->m_layerNextBtn]);

        if (m_oldLabel) {
            m_oldLabel->setZOrder(0);
            m_oldLabel->setScale(m_oldScales[m_oldLabel]);

            layerMenu->addChild(m_oldLabel);
            editor->m_uiItems->addObject(m_oldLabel);
            editor->m_currentLayerLabel = m_oldLabel;

            // betteredit will crash if called before addChild, and will refuse the new position and scale if not forced
            editor->m_currentLayerLabel->runAction(CallFuncExt::create([this, editor] {
                editor->m_currentLayerLabel->setPosition(m_oldPositions[editor->m_currentLayerLabel]);
                editor->m_currentLayerLabel->m_bPositionDirty = true;
                editor->m_currentLayerLabel->m_bTransformDirty = true;
                editor->m_currentLayerLabel->m_bInverseDirty = true;
                editor->m_currentLayerLabel->nodeToParentTransform();
            }));
        }

        auto winSize = CCDirector::get()->getWinSize();
        layerMenu->setPositionX(winSize.width - layerMenu->getScaledContentWidth() / 2.f - 6.f * UIScaling::getScale() - UIScaling::getSafeOffset().x);
    }

    editor->m_layerPrevBtn->setEnabled(true);
    editor->m_layerPrevBtn->setOpacity(255);
    editor->m_layerPrevBtn->setColor({255, 255, 255});

    editor->m_layerNextBtn->setEnabled(true);
    editor->m_layerNextBtn->setOpacity(255);
    editor->m_layerNextBtn->setColor({255, 255, 255});

    editor->m_goToBaseBtn->setVisible(!isBase);

    m_oldLabel = nullptr;
    m_oldBEInput = nullptr;
    m_oldBELayerLock = nullptr;
    m_oldBENextFree = nullptr;
    m_oldLayerLockButton = nullptr;
    m_layerLockButtonParent = nullptr;
}

void ILEditorUI::onLockLayer(CCObject* sender) {
    EditorUI::onLockLayer(sender);
    ImprovedLayers::get()->updateDisplay();
}

namespace tinker::ui {

LayerInput* LayerInput::create() {
    auto ret = new LayerInput();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void LayerInput::setText(ZStringView text, bool runCallback) {
    m_input->setString(text, runCallback);
}

void LayerInput::updateDisplay() {
    auto editor = EditorUI::get();

    auto isBase = editor->m_editorLayer->m_currentLayer == -1;
    auto isMax = editor->m_editorLayer->m_currentLayer == EditorLayerSource().getMax();

    auto layerLocking = GameManager::get()->getGameVariable(GameVar::LayerLocking);

    m_lockBtn->setVisible(layerLocking);

    auto isLocked = !isBase && editor->m_editorLayer->isLayerLocked(editor->m_editorLayer->m_currentLayer);

    CCSprite* spr;
    CCPoint offset;

    if (isLocked) {
        spr = CCSprite::createWithSpriteFrameName("GJ_lock_001.png");
        spr->setScale(0.38f);
    }
    else {
        spr = CCSpriteGrayscale::createWithSpriteFrameName("GJ_lock_open_001.png");
        spr->setScale(0.4f);
        if (isBase) {
            spr->setOpacity(127);
        }
        offset = CCPoint{0.f, 1.f};
    }

    m_lockBtn->setEnabled(!isBase);

    auto oldSpr = m_lockSprContainer->getChildByType<CCSprite>();
    if (oldSpr) {
        oldSpr->removeFromParent();
    }
    m_lockSprContainer->addChild(spr);
    m_lockBtn->setContentSize({17.f, 18.5f});

    float yOffset = 0.65f;

    m_lockSprContainer->setPosition(m_lockBtn->getContentSize() / 2.f - CCPoint{0.f, yOffset});
    m_lockSprContainer->setContentSize(m_lockBtn->getContentSize());

    spr->setAnchorPoint({0.5f, 0.5f});
    spr->setPosition(m_lockSprContainer->getContentSize() / 2.f + offset);

    m_input->getInputNode()->m_textColor = isLocked ? ccColor3B{255, 200, 90} : ccColor3B{255, 255, 255};
    m_input->getInputNode()->refreshLabel();

    float xOffset = 0.f;

    if (layerLocking) {
        xOffset = 1.5f;
        setContentSize(m_input->getScaledContentSize() + CCPoint{m_lockBtn->getContentWidth() + xOffset * 2.f, 0.f});
    }
    else {
        setContentSize(m_input->getScaledContentSize());
    }

    m_background->setContentSize(getContentSize());
    m_background->setPosition(getContentSize() / 2.f - CCPoint{0.f, yOffset});

    m_lockBorder->setContentSize(getContentSize() + CCSize{8.5f, 8.f});
    m_lockBorder->setPosition(getContentSize() / 2.f - CCPoint{0.f, yOffset});
    m_lockBorder->setVisible(layerLocking && isLocked);

    m_input->setPosition({xOffset, getContentHeight() / 2.f});
    m_lockBtn->setPosition({getContentWidth() - m_lockBtn->getContentWidth() / 2.f - xOffset, getContentHeight() / 2.f});

    if (m_lastLayerLockState != layerLocking) {
        auto parent = getParent();
        if (parent) {
            parent->updateLayout();
        }
    }

    editor->m_layerPrevBtn->setEnabled(!isBase);
    editor->m_layerPrevBtn->setOpacity(isBase ? 175 : 255);
    editor->m_layerPrevBtn->setColor(isBase ? ccColor3B{166, 166, 166} : ccColor3B{255, 255, 255});

    editor->m_layerNextBtn->setEnabled(!isMax);
    editor->m_layerNextBtn->setOpacity(isMax ? 175 : 255);
    editor->m_layerNextBtn->setColor(isMax ? ccColor3B{166, 166, 166} : ccColor3B{255, 255, 255});

    editor->m_goToBaseBtn->setVisible(!isBase);

    m_lastLayerLockState = layerLocking;
}

bool LayerInput::init() {
    setAnchorPoint({0.5f, 0.5f});
    ignoreAnchorPointForPosition(false);
    auto editor = EditorUI::get();

    m_input = geode::TextInput::create(30.f, "L");
    m_input->setCallback([this, editor] (const std::string& text) {
        editor->m_editorLayer->m_currentLayer = std::min(numFromString<int>(text).unwrapOr(-1), EditorLayerSource().getMax());
        ImprovedLayers::get()->m_labelListener->setString(text.c_str());
    });
    m_input->setZOrder(1);
    m_input->setScale(0.74f);
    m_input->getBGSprite()->setVisible(false);
    m_input->setID("layer-text-input"_spr);
    m_input->getInputNode()->setMaxLabelWidth(26.f);
    m_input->setAnchorPoint({0.f, 0.5f});

    addChild(m_input);

    InputEditorUI::addTextInput(m_input);

    m_lockSprContainer = geode::NineSlice::create("square02_001.png");
    m_lockSprContainer->setScaleMultiplier(0.5f);
    m_lockSprContainer->setAnchorPoint({0.5f, 0.5f});
    m_lockSprContainer->setOpacity(40);

    m_lockBtn = geode::Button::createWithNode(m_lockSprContainer, [this, editor] (auto sender) {
        editor->onLockLayer(sender);
    });
    m_lockBtn->setID("layer-lock-button"_spr);
    m_lockBtn->setZOrder(1);

    addChild(m_lockBtn);

    m_background = geode::NineSlice::create("square02_001.png");
    m_background->setOpacity(90);
    m_background->setScaleMultiplier(0.75f);
    m_background->setID("layer-background"_spr);

    addChild(m_background);

    m_lockBorder = geode::NineSlice::create("button-empty.png"_spr);
    m_lockBorder->setColor({255, 200, 90});
    m_lockBorder->setOpacity(45);
    m_lockBorder->setScaleMultiplier(1.17f);
    m_lockBorder->setID("layer-lock-border"_spr);

    addChild(m_lockBorder);

    updateDisplay();

    return true;
}

}