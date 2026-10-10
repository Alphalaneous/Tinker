#include "nodes/TextInput.hpp"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"
#include "Geode/loader/Log.hpp"
#include <algorithm>
#include <alphalaneous.alphas-ui-pack/include/Utils.hpp>

namespace tinker::ui {

TextInput* TextInput::create(const CCSize& size, float textScale, ZStringView font, ZStringView placeholder) {
    auto ret = new TextInput();
    if (ret->init(size, textScale, font, placeholder)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool TextInput::init(const CCSize& size, float textScale, ZStringView font, ZStringView placeholder) {
    m_fontFile = font;
    m_textScale = textScale;

    m_fontConfig = FNTConfigLoadFile(font.c_str());
    if (!m_fontConfig) return false;
    
    setContentSize(size);
    setAnchorPoint({0.5f, 0.5f});

    m_labelContainer = CCNode::create();
    m_labelContainer->setContentSize(size);
    m_labelContainer->setAnchorPoint({0.f, 0.f});
    m_labelContainer->setZOrder(1);

    m_labelContainer->setLayout(SimpleColumnLayout::create()
        ->setMainAxisScaling(AxisScaling::None)
        ->setMainAxisAlignment(MainAxisAlignment::Start)
        ->setCrossAxisAlignment(CrossAxisAlignment::Start)
    );

    addChild(m_labelContainer);

    m_allowedChars = geode::getCommonFilterAllowedChars(CommonFilter::Any);
    m_allowedChars += "\n";

    auto n = geode::TextInput::create(0, "");
    n->setCommonFilter(CommonFilter::Any);

    m_selectContainer = CCNode::create();
    m_selectContainer->setContentSize(size);
    m_selectContainer->setAnchorPoint({0.f, 0.f});

    addChild(m_selectContainer);

    m_cursorLabel = geode::Label::create("|", "chatFont.fnt");
    m_cursorLabel->setZOrder(2);
    m_cursorLabel->setScale(m_textScale);

    addChild(m_cursorLabel);

    schedule(schedule_selector(TextInput::blinkCursor), 0.5f);

    textChanged();
    m_cursorLabel->setVisible(false);

    pushUndo();

    return true;
}

void TextInput::setString(ZStringView str, bool runCallback) {
    
}

CCPoint TextInput::posForIndex(int index) {
    bool isLast = index == m_inputState.m_text.size();
    if (isLast) {
        index--;
    }

    auto it = m_characterRanges.find(index);
    if (it == m_characterRanges.end()) {
        auto world = m_labelContainer->convertToWorldSpace({0.f, m_labelContainer->getContentHeight() - (getCommonHeight() * m_textScale) / 2.f});
        return convertToNodeSpace(world);
    }

    auto range = it->second;

    float startX = range->startX;
    if (isLast) {
        startX = range->endX;
    }

    auto label = m_labels[range->line];

    auto pos = CCPoint{startX, getCommonHeight() / 2.f};

    if (isLast && m_inputState.m_text[index] == '\n') {
        pos = CCPoint{0.f, pos.y - getCommonHeight()};
    }

    auto world = label->convertToWorldSpace(pos);
    return convertToNodeSpace(world);
}

int TextInput::indexForPos(const CCPoint& pos) {
    int start = 0;
    int end = 0;

    auto world = convertToWorldSpace(pos);
    auto containerSpace = m_labelContainer->convertToNodeSpace(world);

    geode::Label* label;

    for (int i = 0; i < m_countForLine.size(); i++) {
        label = m_labels[i];

        if (containerSpace.y >= label->boundingBox().getMinY() && containerSpace.y < label->boundingBox().getMaxY()) {
            end = start + m_countForLine[i];
            if (i != m_countForLine.size() - 1) {
                end--;
            }
            break;
        }
        start += m_countForLine[i];
    }

    if (end == 0) {
        if (containerSpace.y > m_labelContainer->getContentHeight()) {
            return 0;
        }
        return m_inputState.m_text.size();
    }

    for (int i = start; i <= end; i++) {
        if (i == end - 1 && m_inputState.m_text[end - 1] == '\n') {
            return end - 1;
        }
        if (i == start && m_inputState.m_text[start] == '\n') {
            return start;
        }

        const auto& range = m_characterRanges[i];

        auto nodeSpace = label->convertToNodeSpace(world);

        float middle = (range->startX + range->endX) / 2.f;

        if ((nodeSpace.x >= range->startX || i == start) && nodeSpace.x < middle) {
            return i;
        }

        if (nodeSpace.x > middle && (nodeSpace.x <= range->endX || i == end - 1)) {
            return std::min(i + 1, end);
        }
    }

    log::info("here");
    return m_inputState.m_text.size();
}

int TextInput::getLineForIndex(int index) {
    if (m_characterRanges.empty()) {
        return 0;
    }

    index = std::clamp(index, 0, static_cast<int>(m_inputState.m_text.size() - 1));
    auto range = m_characterRanges[index];

    return range->line;
}

float TextInput::getCommonHeight() {
    return m_fontConfig->m_nCommonHeight / CCDirector::get()->getContentScaleFactor();
}

void TextInput::updateCursorPos() {
    unschedule(schedule_selector(TextInput::blinkCursor));
    schedule(schedule_selector(TextInput::blinkCursor), 0.5f);

    m_inputState.m_cursorPos = std::clamp(m_inputState.m_cursorPos, 0, static_cast<int>(m_inputState.m_text.size()));

    m_cursorLabel->setVisible(m_focused);
    m_cursorLabel->setPosition(posForIndex(m_inputState.m_cursorPos));
}

int TextInput::getSelectStart() {
    if (m_inputState.m_selectStart > m_inputState.m_selectEnd) {
        return m_inputState.m_selectEnd;
    }
    return m_inputState.m_selectStart;
}

int TextInput::getSelectEnd() {
    if (m_inputState.m_selectEnd < m_inputState.m_selectStart) {
        return m_inputState.m_selectStart;
    }
    return m_inputState.m_selectEnd;
}

void TextInput::updateSelect() {
    int start = getSelectStart();
    int end = getSelectEnd();

    m_selectContainer->removeAllChildren();

    if (!m_inputState.m_selecting) return;

    m_cursorLabel->setVisible(false);

    int lastTotal = 0;
    int total = 0;

    for (int i = 0; i < m_countForLine.size(); i++) {
        lastTotal = total;
        total += m_countForLine[i];

        if ((start >= total && end >= total) || (start <= lastTotal && end <= lastTotal)) {
            continue;
        }

        auto label = m_labels[i];

        auto selectNode = CCLayerColor::create({100, 100, 255, 100});
        selectNode->setContentHeight(getCommonHeight() * m_textScale);
        selectNode->setAnchorPoint({0.f, 0.f});
        selectNode->ignoreAnchorPointForPosition(false);

        const auto& rangeStart = m_characterRanges[std::max(start, lastTotal)];
        const auto& rangeEnd = m_characterRanges[std::min(end - 1, total - 1)];
        
        float width = rangeEnd->endX - rangeStart->startX;

        selectNode->setContentWidth(width * m_textScale);
        selectNode->setPositionY(label->boundingBox().getMinY());
        
        auto world = label->convertToWorldSpace({rangeStart->startX, 0.f});
        auto nodeSpace = m_labelContainer->convertToNodeSpace(world);

        selectNode->setPositionX(nodeSpace.x);

        m_selectContainer->addChild(selectNode);
    }
}

void TextInput::blinkCursor(float dt) {
    if (!m_focused) return;
    m_cursorLabel->setVisible(!m_cursorLabel->isVisible() && !m_inputState.m_selecting);
}

int TextInput::kerningAmountForFirst(unsigned short first, unsigned short second) {
    int ret = 0;
    unsigned int key = (first << 16) | (second & 0xffff);

    if (m_fontConfig->m_pKerningDictionary) {
        tCCKerningHashElement* element = nullptr;
        HASH_FIND_INT(m_fontConfig->m_pKerningDictionary, &key, element);        
        if (element) {
            ret = element->amount;
        }
    }
    return ret;
}

void TextInput::parse() {
    m_labelContainer->removeAllChildren();
    m_labels.clear();

    m_characterRanges.clear();
    m_characterRanges.reserve(m_inputState.m_text.size());
    m_countForLine.clear();
    
    auto text = std::string_view(m_inputState.m_text);
    int start = 0;
    int lastSpace = -1;

    float nextX = 0.f;
    char prev = -1;

    int lineIdx = 0;

    bool lastNewline;

    for (int i = 0; i < text.size(); i++) {
        auto c = text[i];

        if (c == ' ') {
            lastSpace = i;
        }
        if (c == '\n') {
            auto count = i - start + 1;
            m_countForLine[lineIdx] = count;

            addLabel(text.substr(start, count - 1));

            auto range = m_characterRanges[i] = std::make_shared<CharPosRange>(lineIdx, nextX, nextX + m_textScale);

            start = i + 1;
            lastSpace = -1;
            nextX = 0.f;
            lineIdx++;
            lastNewline = true;
            continue;
        }
        lastNewline = false;

        auto subStr = text.substr(start, i - start + 1);

        float kerning = kerningAmountForFirst(prev, c);
        float advance = 0.f;

        tCCFontDefHashElement* element = NULL;

        unsigned int key = c;
        HASH_FIND_INT(m_fontConfig->m_pFontDefDictionary, &key, element);
        if (element) {
            const auto& fontDef = element->fontDef;
            advance = fontDef.xAdvance + kerning;
        }
        else {
            advance = kerning;
        }

        auto prevX = nextX;

        nextX += (advance / CCDirector::get()->getContentScaleFactor());

        m_characterRanges[i] = std::make_shared<CharPosRange>(lineIdx, prevX, nextX);

        if (nextX > (getContentWidth() / m_textScale)) {
            if (lastSpace >= start) {
                auto count = lastSpace - start + 1;
                m_countForLine[lineIdx] = count;

                addLabel(text.substr(start, count));

                start = lastSpace + 1;
                i = start - 1;
            }
            else {
                auto count = i - start;
                m_countForLine[lineIdx] = count;

                addLabel(text.substr(start, count));

                start = i;
                i = start - 1;
            }
            lineIdx++;
            nextX = 0.f;
            lastSpace = -1;
        }
    }

    if (start < text.size()) {
        auto count = text.size() - start;
        m_countForLine[lineIdx] = count;
        addLabel(text.substr(start));
    }

    if (lastNewline) {
        m_countForLine[lineIdx] = 1;
        m_characterRanges[text.size()] = std::make_shared<CharPosRange>(lineIdx, 0.f, 0.f);
        addLabel("");
    }

    m_labelContainer->updateLayout();
}

void TextInput::onEnter() {
    CCNode::onEnter();
    addEventListener("keyboard-input"_spr, KeyboardInputEvent(), [this] (enumKeyCodes key, KeyboardInputData& data) {
        if (!m_focused) return false;

        bool down = data.action == KeyboardInputData::Action::Press || data.action == KeyboardInputData::Action::Repeat;
        onKeyboard(key, down, data.action == KeyboardInputData::Action::Repeat, data.timestamp);
        return true;
    });
    CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
}

void TextInput::onExit() {
    CCNode::onExit();
    removeEventListener("keyboard-input"_spr);
    CCTouchDispatcher::get()->removeDelegate(this);
}

void TextInput::focus() {
    attachWithIME();
}

void TextInput::unfocus() {
    detachWithIME();
}

bool TextInput::attachWithIME() {
    auto ret = CCIMEDelegate::attachWithIME();
    if (ret) {
        CCEGLView::get()->setIMEKeyboardState(true);
    }
    return ret;
}

bool TextInput::detachWithIME() {
    auto ret = CCIMEDelegate::detachWithIME();
    if (ret) {
        CCEGLView::get()->setIMEKeyboardState(false);
    }
    return ret;
}

bool TextInput::canAttachWithIME() {
    m_focused = true;

    return true;
}

bool TextInput::canDetachWithIME() {
    m_focused = false;
    m_cursorLabel->setVisible(false);

    return true;
}

bool TextInput::clickBegan(alpha::dispatcher::TouchEvent* touch) {
    if (!nodeIsVisible(this) || !alpha::utils::isPointInsideNode(this, touch->getLocation())) {
        unfocus();
        return false;
    }

    m_inputState.m_selecting = false;
    m_inputState.m_selectEnd = 0;

    focus();
    m_inputState.m_cursorPos = indexForPos(convertToNodeSpace(touch->getLocation()));
    m_inputState.m_selectStart = m_inputState.m_cursorPos;

    updateSelect();

    updateCursorPos();

    return true;
}

void TextInput::clickMoved(alpha::dispatcher::TouchEvent* touch) {
    m_inputState.m_selectEnd = indexForPos(convertToNodeSpace(touch->getLocation()));

    m_inputState.m_selecting = m_inputState.m_selectEnd != m_inputState.m_selectStart;
    updateSelect();
}

void TextInput::clickEnded(alpha::dispatcher::TouchEvent* touch) {

}

void TextInput::addLabel(std::string_view str) {

    geode::Label* label;
    if (str.empty()) {
        label = geode::Label::create(" ", m_fontFile.c_str());
    }
    else {
        label = geode::Label::create(std::string(str), m_fontFile.c_str());
    }

    label->setAnchorPoint({0.f, 1.f});
    label->setPosition({0.f, getContentHeight()});
    label->setScale(m_textScale);

    m_labels.push_back(label);
    m_labelContainer->addChild(label);
}

void TextInput::textChanged() {
    parse();
}

const char* TextInput::getContentText() {
    return m_inputState.m_text.c_str();
}

void TextInput::enterPressed() {
    log::info("enter pressed");
    insertText("\n", 1, cocos2d::KEY_Unknown);
}

void TextInput::pushUndo() {
    m_redoStack.clear();
    m_undoStack.insert(m_undoStack.begin(), m_inputState);

    if (m_undoStack.size() == 100) {
        m_undoStack.erase(m_undoStack.end() - 1);
    }
}

void TextInput::insertText(const char* text, int len, cocos2d::enumKeyCodes keyCode) {
    if (!text) return;

    insert(text, true);
}

void TextInput::insert(ZStringView text, bool undo) {
    if (m_maxLength > 0 && m_inputState.m_text.size() >= m_maxLength) {
        return;
    }

    std::string filtered;

    for (char c : text) {
        if (m_allowedChars.find(c) != std::string::npos) {
            filtered += c;
        }
    }

    if (!filtered.empty()) {
        if (m_maxLength > 0 && filtered.size() >= static_cast<size_t>(m_maxLength)) {
            filtered.resize(m_maxLength - 1);
        }

        bool wasSelecting = m_inputState.m_selecting;
        if (wasSelecting) {
            pushUndo();
            removeSelection();
        }

        m_inputState.m_text.insert(m_inputState.m_cursorPos, filtered);

        textChanged();

        m_inputState.m_cursorPos += filtered.size();
        updateCursorPos();

        if (!wasSelecting && undo && (text == " " || text == "\n")) {
            pushUndo();
        }
    }
}

void TextInput::removeSelection() {
    m_inputState.m_cursorPos = getSelectStart();
    m_inputState.m_text.erase(m_inputState.m_cursorPos, getSelectEnd() - getSelectStart()); 
    m_inputState.m_selecting = false;

    updateSelect();
}

bool TextInput::onKeyboard(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat, double timestamp) {
    if (isKeyDown) {
        if (key == enumKeyCodes::KEY_Enter || key == cocos2d::KEY_NumEnter) {
            enterPressed();
            return true;
        }
        if (key == enumKeyCodes::KEY_Backspace) {
            pushUndo();

            if (m_inputState.m_selecting) {
                removeSelection();
                textChanged();
                updateCursorPos();
            }
            else {
                deleteBackward();
            }
            return true;
        }
        if (key == enumKeyCodes::KEY_Delete) {
            pushUndo();

            if (m_inputState.m_selecting) {
                removeSelection();
                textChanged();
                updateCursorPos();
            }
            else {
                deleteForward();
            }
            return true;
        }
        if (key == KEY_ArrowLeft || key == KEY_Left) {
            int newPos = m_inputState.m_cursorPos - 1;
            if (m_inputState.m_selecting) {
                newPos = getSelectStart();
            }
            m_inputState.m_cursorPos = std::max(newPos, 0);

            deselectAll();
            updateCursorPos();
            return true;
        }
        if (key == KEY_ArrowRight || key == KEY_Right) {
            int newPos = m_inputState.m_cursorPos + 1;
            if (m_inputState.m_selecting) {
                newPos = getSelectEnd();
            }
            m_inputState.m_cursorPos = std::min(newPos, static_cast<int>(m_inputState.m_text.size()));

            deselectAll();
            updateCursorPos();
            return true;
        }
        if (key == KEY_ArrowUp || key == KEY_Up) {
            if (getLineForIndex(m_inputState.m_cursorPos) == 0) return true;

            auto pos = posForIndex(m_inputState.m_cursorPos);
            pos.y += getCommonHeight() * m_textScale;

            deselectAll();
            m_inputState.m_cursorPos = indexForPos(pos);

            updateCursorPos();
            return true;
        }
        if (key == KEY_ArrowDown || key == KEY_Down) {
            if (getLineForIndex(m_inputState.m_cursorPos) == m_countForLine.size() - 1) return true;
            
            auto pos = posForIndex(m_inputState.m_cursorPos);
            pos.y -= getCommonHeight() * m_textScale;

            deselectAll();
            m_inputState.m_cursorPos = indexForPos(pos);

            updateCursorPos();
            return true;
        }
        if (m_controlPressed) {
            if (key == KEY_Z) {
                if (m_shiftPressed) {
                    redo();
                }
                else {
                    undo();
                }
            }
            if (key == KEY_Y) {
                redo();
            }
            if (key == KEY_X) {
                cut();
            }
            if (key == KEY_C) {
                copy();
            }
            if (key == KEY_V) {
                paste();
            }
            if (key == KEY_A) {
                selectAll();
            }
        }
    }

    if (key == KEY_Control || key == KEY_LeftControl || key == KEY_RightControl) {
        m_controlPressed = isKeyDown;
        return true;
    }
    if (key == cocos2d::KEY_Shift || key == KEY_LeftShift || key == KEY_RightShift) {
        m_shiftPressed = isKeyDown;
        return true;
    }

    return false;
}

void TextInput::copy() {
    if (!m_inputState.m_selecting) return;

    auto subStr = m_inputState.m_text.substr(m_inputState.m_cursorPos, getSelectEnd() - getSelectStart());
    geode::utils::clipboard::write(subStr);
}

void TextInput::cut() {
    if (!m_inputState.m_selecting) return;

    pushUndo();

    m_inputState.m_cursorPos = getSelectStart();
    auto subStr = m_inputState.m_text.substr(m_inputState.m_cursorPos, getSelectEnd() - getSelectStart());
    geode::utils::clipboard::write(subStr);

    removeSelection();
    textChanged();
    updateCursorPos();
}

void TextInput::paste() {
    auto clipboardText = geode::utils::clipboard::read();
    if (clipboardText.empty()) return;
    
    if (!m_inputState.m_selecting) {
        pushUndo();
    }
    insert(clipboardText, false);
}

void TextInput::undo() {
    if (m_undoStack.empty()) {
        return;
    }

    m_redoStack.insert(m_redoStack.begin(), m_inputState);

    m_inputState = m_undoStack[0];

    textChanged();
    updateCursorPos();
    updateSelect();

    m_undoStack.erase(m_undoStack.begin());
}

void TextInput::redo() {
    if (m_redoStack.empty()) return;

    m_undoStack.insert(m_undoStack.begin(), m_inputState);

    m_inputState = m_redoStack[0];

    textChanged();
    updateCursorPos();
    updateSelect();

    m_redoStack.erase(m_redoStack.begin());
}

void TextInput::selectAll() {
    m_inputState.m_selecting = true;
    m_inputState.m_selectStart = 0;
    m_inputState.m_selectEnd = m_inputState.m_text.size();

    updateSelect();
}

void TextInput::deselectAll() {
    m_inputState.m_selecting = false;
    m_inputState.m_selectStart = 0;
    m_inputState.m_selectEnd = 0;

    updateSelect();
}

void TextInput::deleteBackward() {
    if (m_inputState.m_text.empty()) return;

    m_inputState.m_cursorPos = std::max(m_inputState.m_cursorPos - 1, 0);

    m_inputState.m_text.erase(m_inputState.m_cursorPos, 1); 

    textChanged();
    updateCursorPos();
}

void TextInput::deleteForward() {
    if (m_inputState.m_text.empty()) return;

    m_inputState.m_text.erase(m_inputState.m_cursorPos, 1); 

    textChanged();
    updateCursorPos();
}

}

/*
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/MenuGameLayer.hpp>

class $modify(MyMenuLayer, MenuLayer) {

    bool init() {
        if (!MenuLayer::init()) return false;

        auto input = tinker::ui::TextInput::create({350, 150}, 0.4f, "chatFont.fnt");
        input->setAnchorPoint({0.5f, 1.f});
        input->setPosition({getContentWidth() / 2.f, getContentHeight() / 2.f + 80.f});

        addChild(input);

        getChildByID("main-menu")->setVisible(false);

        return true;
    }

};


class $modify(MyMenuGameLayer, MenuGameLayer) {

    void updateSprite(CCSprite* spr) {
        if (!spr) return;
        spr->setOpacity(127);
    }

    void updateSpriteColors() {
        updateSprite(m_backgroundSprite);
        updateSprite(m_groundLayer->m_ground1Sprite);
        updateSprite(m_groundLayer->m_ground2Sprite);
    }

    void update(float dt) {
        MenuGameLayer::update(dt);
        updateSpriteColors();
    }
};*/