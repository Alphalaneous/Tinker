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

namespace tinker::ui {

class TextInput : public cocos2d::CCNode, public alpha::dispatcher::TouchDelegate, public cocos2d::CCIMEDelegate {
public:
    static TextInput* create(const cocos2d::CCSize& size, float textScale = 1.f, geode::ZStringView font = "bigFont.fnt", geode::ZStringView placeholder = "");

    void setString(geode::ZStringView str, bool runCallback = false);

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

    void insertText(const char * text, int len, cocos2d::enumKeyCodes keyCode) override;

    bool onKeyboard(cocos2d::enumKeyCodes key, bool isKeyDown, bool isKeyRepeat, double timestamp);

    const char* getContentText() override;

protected:
    void parse();
    void addLabel(std::string_view str);

    void insert(geode::ZStringView text, bool undo);

    bool init(const cocos2d::CCSize& size, float textScale, geode::ZStringView font, geode::ZStringView placeholder);

    bool clickBegan(alpha::dispatcher::TouchEvent* touch) override;
	void clickMoved(alpha::dispatcher::TouchEvent* touch) override;
	void clickEnded(alpha::dispatcher::TouchEvent* touch) override;

    int kerningAmountForFirst(unsigned short first, unsigned short second);

    cocos2d::CCPoint posForIndex(int index);
    int indexForPos(const cocos2d::CCPoint& pos);

    int getLineForIndex(int index);

    float getCommonHeight();

    void updateCursorPos();
    void blinkCursor(float dt);

    void removeSelection();

    int getSelectStart();
    int getSelectEnd();

    void updateSelect();

    void pushUndo();

    float m_textScale = 0.f;

    bool m_callbackEnabled = false;

    bool m_focused = false;
    int m_maxLength = 0;

    bool m_controlPressed = false;
    bool m_shiftPressed = false;

    std::string m_allowedChars = "";

    struct InputState {
        int m_cursorPos = 0;
        std::string m_text = "";
        bool m_selecting = false;
        int m_selectStart = 0;
        int m_selectEnd = 0;
    };

    InputState m_inputState{};

    std::vector<InputState> m_undoStack{};
    std::vector<InputState> m_redoStack{};

    struct CharPosRange {
        int line = 0;
        float startX = 0.f;
        float endX = 0.f;
    };

    std::unordered_map<int, std::shared_ptr<CharPosRange>> m_characterRanges{};
    std::unordered_map<int, int> m_countForLine{};

    geode::Label* m_cursorLabel = nullptr;
    std::vector<geode::Label*> m_labels{};
    std::string m_fontFile = "";
    cocos2d::CCNode* m_labelContainer = nullptr;
    cocos2d::CCNode* m_selectContainer = nullptr;

    geode::Ref<cocos2d::CCBMFontConfiguration> m_fontConfig = nullptr;
};

}