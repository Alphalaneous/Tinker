#pragma once

#include <Geode/ui/Button.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/Geode.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

using namespace geode::prelude;
using namespace alpha::prelude;

class OSEditorUI;

namespace tinker::ui {
    class SearchField : public CCNode, public TouchDelegate, public TextInputDelegate {
    public:
        struct ItemInformation {
            Ref<CreateMenuItem> item;
            std::string name;
            int id;
        };
        
        ~SearchField();

        static SearchField* create(OSEditorUI* editorUI);

        void onEnter() override;
        void onExit() override;

        void textInputShouldOffset(CCTextInputNode* node, float yOffset) override;

        void defocus();
        void focus();

        virtual bool clickBegan(TouchEvent* touch) override;

        CCArray* generateItemArrayForSearch(const std::string& search);
    protected:

        bool init(OSEditorUI* editorUI);

        Result<ItemInformation> infoForID(unsigned int id);

        OSEditorUI* m_editorUI;
        geode::TextInput* m_searchInput;
        Ref<geode::NineSlice> m_searchBG;
        geode::Button* m_clearButton;
    };
}