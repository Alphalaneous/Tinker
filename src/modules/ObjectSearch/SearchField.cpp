#include "SearchField.hpp"
#include "../../ObjectNames.hpp"
#include <alphalaneous.alphas-ui-pack/include/Utils.hpp>
#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include <Geode/external/fts/fts_fuzzy_match.h>
#include "../../Utils.hpp"
#include "ObjectSearch.hpp"

using namespace tinker::ui;

SearchField::~SearchField() {
    m_searchBG->removeAllChildren();
}

Result<SearchField::ItemInformation> SearchField::infoForID(unsigned int id) {
    auto fields = m_editorUI->m_fields.self();

    auto iter = fields->m_items.find(id);
    if (iter == fields->m_items.end()) return geode::Err("item not found");

    if (iter->second.name.empty()) return geode::Err("item has no name");

    return geode::Ok(iter->second);
}

CCArray* SearchField::generateItemArrayForSearch(const std::string& search) {
    auto fields = m_editorUI->m_fields.self();

    auto arr = CCArray::createWithCapacity(fields->m_orderedItems.size());

    if (search.empty()) {
        for (const auto& item : fields->m_orderedItems) {
            arr->addObject(item->item);
        }
        return arr;
    }

    auto lower = geode::utils::string::toLower(search);

    if (lower.starts_with("id:")) {
        std::vector<std::string> parts = tinker::utils::split(lower, ":");
        if (parts.size() == 2) {
            Result<int> numRes = numFromString<int>(geode::utils::string::trim(parts[1]));
            if (numRes.isOk()) {
                int id = numRes.unwrap();

                auto info = infoForID(id);
                if (info) arr->addObject(info.unwrap().item);
            }
        }
    }
    else if (lower.starts_with("exact:")) {
        std::vector<std::string> parts = tinker::utils::split(lower, ":", 2);
        if (parts.size() == 2) {
            for (auto& [k, v] : ObjectNames::get()->getNames()) {
                std::string lowerV = geode::utils::string::toLower(v);
                if (lowerV == geode::utils::string::trim(parts[1])) {
                    auto info = infoForID(k);
                    if (info) arr->addObject(info.unwrap().item);
                }
            }
        }
    }
    else {
        struct Score {
            unsigned int id;
            std::string name;
            int score;
        };

        std::vector<Score> nameScores;

        for (auto& [k, v] : ObjectNames::get()->getNames()) {
            int score = 0;
            if (!lower.empty() && !fts::fuzzy_match(lower.c_str(), v.c_str(), score)) continue;
            std::string lowerV = geode::utils::string::toLower(v);
            //we still want the results to make some sense
            if (geode::utils::string::contains(lowerV, lower)) { 
                nameScores.push_back({k, v, score});
            }
        }

        if (!nameScores.empty()) {
            std::sort(nameScores.begin(), nameScores.end(), [&](const auto& a, const auto& b) {
                return a.score > b.score;
            });

            for (const auto& score : nameScores) {
                auto info = infoForID(score.id);
                if (info) arr->addObject(info.unwrap().item);
            }
        }
    }
    return arr;
}

SearchField* SearchField::create(OSEditorUI* editorUI) {
    auto ret = new SearchField();
    if (ret->init(editorUI)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SearchField::init(OSEditorUI* editorUI) {
    if (!CCNode::init()) return false;
    m_editorUI = editorUI;

    setAnchorPoint({0.5f, 0.f});
    setContentSize({300, 45});

    m_searchInput = geode::TextInput::create(getContentWidth() - 55, "Search...");
    m_searchInput->setDelegate(this);
    m_searchInput->setCommonFilter(CommonFilter::Any);
    m_searchInput->setPosition({6 + m_searchInput->getContentWidth() / 2, getContentHeight() / 2});
    m_searchInput->setID("search-input"_spr);

    m_searchBG = geode::NineSlice::create("GJ_square02.png");
    m_searchBG->setAnchorPoint({0, 0});
    m_searchBG->setContentSize(getContentSize());
    m_searchBG->setID("background"_spr);

    addChild(m_searchBG);

    m_searchBG->addChild(m_searchInput);

    setScale(0.6f);

    m_clearButton = geode::Button::createWithSpriteFrameName("GJ_longBtn07_001.png", [this] (auto sender) {
        m_lockClose = true;
        m_searchInput->setString("", true);
        m_searchInput->focus();
    });
    m_clearButton->setID("clear-button"_spr);

    m_clearButton->setSelectCallback([this] (auto sender) {
        m_lockClose = true;
        m_searchInput->focus();
        #ifdef GEODE_IS_MOBILE
        textInputShouldOffset(m_searchInput->getInputNode(), m_yOffset);
        #endif
    });

    m_clearButton->setPosition({getContentWidth() - 6 - m_clearButton->getContentWidth() / 2, getContentHeight() / 2});
    m_searchBG->addChild(m_clearButton);

    m_tabBG = geode::NineSlice::create("square02b_001.png");
    m_tabBG->setOpacity(127);
    m_tabBG->setColor({0, 0, 0});
    m_tabBG->setID("floating-tab-background"_spr);

    return true;
}

void SearchField::setOrigY() {
    m_origY = getPositionY();
}

void SearchField::defocus() {
    m_searchInput->defocus();
}

void SearchField::focus() {
    m_searchInput->focus();
}

void SearchField::onEnter() {
    CCNode::onEnter();
    CCTouchDispatcher::get()->addTargetedDelegate(this, -120, true);
}

void SearchField::onExit() {
    CCNode::onExit();
    CCTouchDispatcher::get()->removeDelegate(this);
}

bool SearchField::clickBegan(TouchEvent* touch) {
    if (m_inputFocused) return true;
    if (!nodeIsVisible(this) || !alpha::utils::isPointInsideNode(this, touch->getLocation())) return false;
    return true;
}

void SearchField::textChanged(CCTextInputNode* node) {
    m_searchInput->runAction(CallFuncExt::create([this, node] {
        auto arr = generateItemArrayForSearch(node->getString());

        auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
        auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

        auto fields = m_editorUI->m_fields.self();

        fields->m_searchBar->loadFromItems(arr, cols, rows, false);

        #ifdef GEODE_IS_MOBILE
        fields->m_searchBar->setPositionY(m_yOffset + getScaledContentHeight() + 10);
        #endif
    }));
}

void SearchField::textInputOpened(CCTextInputNode* node) {
    #ifdef GEODE_IS_MOBILE
    m_inputFocused = true;

    setupTabOffset();

    m_lockClose = true;
    runAction(CallFuncExt::create([this] {
        m_lockClose = false;
    }));
    #endif
}

void SearchField::textInputClosed(CCTextInputNode* node) {
    #ifdef GEODE_IS_MOBILE
    runAction(CallFuncExt::create([this] {
        if (!m_lockClose) {
            m_inputFocused = false;

            setPositionY(m_origY);

            auto tab = m_editorUI->m_fields->m_searchBar;

            tab->setPositionY(0);
            m_tabBG->removeFromParent();

            tab->setZOrder(10);
        }
        m_lockClose = false;
    }));
    #endif
}

void SearchField::textInputShouldOffset(CCTextInputNode* node, float yOffset) {
    #ifdef GEODE_IS_MOBILE
    m_yOffset = yOffset;
    runAction(CallFuncExt::create([this] {
        setupTabOffset();
    }));
    #endif
}

void SearchField::setupTabOffset() {
    setPositionY(m_yOffset);

    auto tab = m_editorUI->m_fields->m_searchBar;
    tab->setPositionY(m_yOffset + getScaledContentHeight() + 10);

    m_tabBG->setContentSize(tab->getScaledContentSize() + CCSize{0, 10});
    m_tabBG->setPosition(tab->getPosition() + CCPoint{0, tab->getScaledContentHeight() / 2});
    tab->setZOrder(20);
    m_tabBG->setZOrder(19);

    m_tabBG->removeFromParent();

    m_editorUI->addChild(m_tabBG);
}