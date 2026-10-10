#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/cocos/label_nodes/CCLabelBMFont.h>
#include <Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h>
#include <Geode/cocos/text_input_node/CCIMEDelegate.h>
#include <Geode/ui/Label.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <alphalaneous.alphas-ui-pack/include/touch/TouchDelegate.hpp>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace geode::prelude;

namespace tinker::ui {

class TextInput : public CCNode, public alpha::dispatcher::TouchDelegate, public CCIMEDelegate {
public:
    static TextInput* create(const CCSize& size, float textScale = 1.f, ZStringView font = "bigFont.fnt", ZStringView placeholder = "");

    void setString(ZStringView str, bool runCallback = false);

    void focus();
    void unfocus();

    void deleteBackward() override;
    void deleteForward() override;

    void textChanged();
    void enterPressed();

    void copy();
    void cut();
    void paste();

    void undo();
    void redo();

    void selectAll();
    void deselectAll();

    void onEnter() override;
    void onExit() override;

    bool attachWithIME() override;
    bool detachWithIME() override;

    bool canAttachWithIME() override;
    bool canDetachWithIME() override;

    void insertText(const char * text, int len, enumKeyCodes keyCode) override;

    bool onKeyboard(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat, double timestamp);

    const char* getContentText() override;

protected:
    void parse();
    void addLabel(std::string_view str);

    void insert(ZStringView text, bool undo);

    bool init(const CCSize& size, float textScale, ZStringView font, ZStringView placeholder);

    bool clickBegan(alpha::dispatcher::TouchEvent* touch) override;
	void clickMoved(alpha::dispatcher::TouchEvent* touch) override;
	void clickEnded(alpha::dispatcher::TouchEvent* touch) override;

    int kerningAmountForFirst(unsigned short first, unsigned short second);

    CCPoint posForIndex(int index);
    int indexForPos(const CCPoint& pos);

    int getLineForIndex(int index);

    float getCommonHeight();

    void updateCursorPos();
    void blinkCursor(float dt);

    void removeSelection();

    int getSelectStart();
    int getSelectEnd();

    void updateSelect();

    void pushUndo();

    float m_textScale;

    bool m_callbackEnabled;

    bool m_focused;
    int m_maxLength;

    bool m_controlPressed;
    bool m_shiftPressed;

    std::string m_allowedChars;

    struct InputState {
        int m_cursorPos;
        std::string m_text;
        bool m_selecting;
        int m_selectStart;
        int m_selectEnd;
    };

    InputState m_inputState;

    std::vector<InputState> m_undoStack;
    std::vector<InputState> m_redoStack;

    struct CharPosRange {
        int line;
        float startX;
        float endX;
    };

    std::unordered_map<int, std::shared_ptr<CharPosRange>> m_characterRanges;
    std::unordered_map<int, int> m_countForLine;

    geode::Label* m_cursorLabel;
    std::vector<geode::Label*> m_labels;
    std::string m_fontFile;
    CCNode* m_labelContainer;
    CCNode* m_selectContainer;

    Ref<CCBMFontConfiguration> m_fontConfig;
};

}