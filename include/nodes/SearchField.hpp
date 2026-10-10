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

using namespace geode::prelude;
using namespace alpha::prelude;

class OSEditorUI;

namespace tinker::ui {
    class SearchField : public CCNode, public TouchDelegate, public TextInputDelegate {
    public:
        struct ItemInformation {
            Ref<CreateMenuItem> item;
            int id;
            int tabIndex;
            std::string name;
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

        ZStringView nameForInfo(ItemInformation& item);
        CCArray* generateItemArrayForSearch(const std::string& search);
    protected:

        bool init(OSEditorUI* editorUI);

        Result<ItemInformation> infoForID(unsigned int id);

        OSEditorUI* m_editorUI;
        geode::TextInput* m_searchInput;
        Ref<geode::NineSlice> m_searchBG;
        geode::Button* m_clearButton;
        Ref<geode::NineSlice> m_tabBG;

        float m_yOffset;
        bool m_lockClose;
        bool m_inputFocused;
    };
}