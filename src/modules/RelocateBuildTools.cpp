#include "RelocateBuildTools.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

StringMap<std::string> RelocateBuildTools::s_labelToIcon = {
    {"Reset\nScroll", "ResetScroll"},
    {"Create\nLoop", "CreateLoop"},
    {"Re-\nGroup", "Regroup"},
    {"AlignX", "AlignX"},
    {"AlignY", "AlignY"},
    {"Select\nAll", "SelectAll"},
    {"Select\nAll\nLeft", "SelectAllLeft"},
    {"Select\nAll\nRight", "SelectAllRight"},
    {"New\nGroupX", "CreateGroupX"},
    {"New\nGroupY", "CreateGroupY"},
    {"Build\nHelper", "BuildHelper"},
    {"Copy+\nColor", "CopyColor"},
    {"Paste+\nColor", "PasteColor"},
    {"Unlock\nLayers", "UnlockLayers"},
    {"Reset\nUnused", "ResetUnused"},
    {"Uncheck\nPortals", "UncheckPortals"},
};

void RelocateBuildTools::onEditor() {
    m_pauseLayer = EditorPauseLayer::create(m_editorLayer);
    m_pauseLayer->setTouchEnabled(false);
    m_pauseLayer->setKeyboardEnabled(false);
    m_pauseLayer->setKeypadEnabled(false);
    m_pauseLayer->setUserFlag("ignore"_spr);
    static_cast<RBTEditorPauseLayer*>(m_pauseLayer.data())->m_fields->m_noResume = true;

    // lol
    CCTouchDispatcher::get()->unregisterForcePrio(m_pauseLayer);
    CCTouchDispatcher::get()->removeDelegate(m_pauseLayer);
    CCKeyboardDispatcher::get()->removeDelegate(m_pauseLayer);
    CCScheduler::get()->unscheduleAllForTarget(m_pauseLayer);
    CCDirector::get()->m_pKeypadDispatcher->removeDelegate(m_pauseLayer);
    CCDirector::get()->m_pMouseDispatcher->removeDelegate(m_pauseLayer);
    CCDirector::get()->m_pActionManager->removeAllActionsFromTarget(m_pauseLayer);

    alpha::editor_tabs::addTab("build-tools"_spr, alpha::editor_tabs::EDIT, 
    [this] {
        return createBuildTools();
    },
    [] {
        return CCLabelBMFont::create("B", "bigFont.fnt");
    });
}

CCSprite* RelocateBuildTools::addIcon(CCNode* node, CCLabelBMFont* label) {
    std::string iconTexture;

    bool iconsOnly = false;

    if (getSetting<std::string, "icons">() == "Icons Only") {
        iconTexture = fmt::format("o_{}.png"_spr, s_labelToIcon[label->getString()]);
        iconsOnly = true;
    }
    else if (getSetting<std::string, "icons">() == "Icons & Text") {
        iconTexture = fmt::format("u_{}.png"_spr, s_labelToIcon[label->getString()]);
    }

    if (auto spr = CCSprite::create(iconTexture.c_str())) {
        spr->setZOrder(1);
        spr->setID("tool-sprite"_spr);
        if (!iconsOnly) {
            spr->setColor({0, 0, 0});
            spr->setOpacity(96);
        }
        spr->setScale(0.7f);
        spr->setPosition({node->getContentSize().width/2, node->getContentSize().height/2});
        if (spr->getUserObject("geode.texture-loader/fallback")) return nullptr;
        node->addChild(spr);
        return spr;
    }
    return nullptr;
}

void RelocateBuildTools::rebuildButtons(std::vector<Ref<CCNode>> nodes) {
    for (auto child : nodes) {
        child->setContentSize({40, 40});
        child->setVisible(true);
        auto childSize = child->getContentSize();
        
        if (auto buttonSprite = child->getChildByType<ButtonSprite>(0)) {
            buttonSprite->setContentSize({40, 40});
            buttonSprite->setScale(1);
            buttonSprite->setPosition({childSize.width/2, childSize.height/2});

            if (auto bg = buttonSprite->getChildByType<CCScale9Sprite>(0)) {
                bg->removeFromParent();
            }
            if (auto label = buttonSprite->getChildByType<CCLabelBMFont>(0)) {
                label->setScale(0.25f);
                label->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
                label->setPosition({childSize.width/2, childSize.height/2});
                auto labelText = std::string(label->getString());

                utils::string::replaceIP(labelText, " ", "\n");

                label->setString(labelText.c_str());
                label->setZOrder(2);

                if (getSetting<std::string, "icons">() != "None") {
                    if (addIcon(buttonSprite, label) && getSetting<std::string, "icons">() == "Icons Only") {
                        label->setVisible(false);
                    }
                }
            }
            auto buttonBG = CCSprite::create("GJ_button_04.png");
            buttonBG->setPosition({childSize.width/2, childSize.height/2});

            buttonSprite->addChild(buttonBG);
        }
    }
}

CCNode* RelocateBuildTools::createBuildTools() {
    
    std::vector<Ref<CCNode>> nodes;

    if (auto smallActionsMenu = m_pauseLayer->getChildByID("small-actions-menu")) {
        for (auto child : smallActionsMenu->getChildrenExt()) {
            nodes.push_back(child);
        }
        smallActionsMenu->removeAllChildrenWithCleanup(false);
    }

    if (auto actionsMenu = m_pauseLayer->getChildByID("actions-menu")) {
        actionsMenu->removeChildByID("keys-button");

        for (auto child : actionsMenu->getChildrenExt()) {
            nodes.push_back(child);
        }
        actionsMenu->removeAllChildrenWithCleanup(false);
    }

    if (nodes.size() > 0) {
        rebuildButtons(nodes);
    }

    return alpha::editor_tabs::createEditButtonBar(nodes);
}

void RelocateBuildTools::onEditorPauseLayer(EditorPauseLayer* editorPauseLayer) {
    if (auto smallActionsMenu = editorPauseLayer->getChildByID("small-actions-menu")) {
        static_cast<AxisLayout*>(smallActionsMenu->getLayout())->ignoreInvisibleChildren(true);
        for (auto child : smallActionsMenu->getChildrenExt()) {
            child->setVisible(false);
        }
        smallActionsMenu->updateLayout();
    }

    if (auto actionsMenu = editorPauseLayer->getChildByID("actions-menu")) {
        static_cast<AxisLayout*>(actionsMenu->getLayout())->ignoreInvisibleChildren(true);

        for (auto child : actionsMenu->getChildrenExt()) {
            child->setVisible(false);
        }
        if (auto keys = actionsMenu->getChildByID("keys-button")) {
            keys->setVisible(true);
        }
        actionsMenu->updateLayout();
    }
}

void RBTEditorPauseLayer::onResume(CCObject* sender) {
    if (m_fields->m_noResume) return;
    EditorPauseLayer::onResume(sender);
}