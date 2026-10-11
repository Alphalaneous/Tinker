#pragma once

#include <Geode/cocos/cocoa/CCArray.h>
#include <Geode/ui/Button.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/Result.hpp>
#include <Geode/binding/CCTextInputNode.hpp>
#include <Geode/binding/CreateMenuItem.hpp>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/binding/TextInputDelegate.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <string>

using namespace alpha::prelude;

class OSEditorUI;

namespace tinker::ui {

class SearchField : public cocos2d::CCNode, public TouchDelegate, public TextInputDelegate {
public:
    struct ItemInformation {
        geode::Ref<CreateMenuItem> item = nullptr;
        int id = 0;
        int tabIndex = 0;
        std::string name = "";
    };
    
    ~SearchField();

    static SearchField* create(OSEditorUI* editorUI);

    void onEnter() override;
    void onExit() override;

    void textInputShouldOffset(CCTextInputNode* node, float yOffset) override;
    void textInputOpened(CCTextInputNode* node) override;
    void textInputClosed(CCTextInputNode* node) override;
    void textChanged(CCTextInputNode* node) override;

    void onClosed();

    void defocus();
    void focus();

    void setupTabOffset();

    virtual bool clickBegan(TouchEvent* touch) override;

    geode::ZStringView nameForInfo(ItemInformation& item);
    cocos2d::CCArray* generateItemArrayForSearch(const std::string& search);
protected:

    bool init(OSEditorUI* editorUI);

    geode::Result<ItemInformation> infoForID(unsigned int id);

    OSEditorUI* m_editorUI = nullptr;
    geode::TextInput* m_searchInput = nullptr;
    geode::Ref<geode::NineSlice> m_searchBG = nullptr;
    geode::Button* m_clearButton = nullptr;
    geode::Ref<geode::NineSlice> m_tabBG = nullptr;

    float m_yOffset = 0.f;
    bool m_lockClose = false;
    bool m_inputFocused = false;
};

}