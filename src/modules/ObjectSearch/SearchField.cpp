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
    m_searchInput->setCallback([this] (const std::string& text) {
        m_searchInput->runAction(CallFuncExt::create([this, text] {
            auto arr = generateItemArrayForSearch(text);

            auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
            auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);
            m_editorUI->m_fields->m_searchBar->loadFromItems(arr, cols, rows, false);
        }));
    });
    m_searchInput->setCommonFilter(CommonFilter::Any);
    m_searchInput->setPosition({6 + m_searchInput->getContentWidth() / 2, getContentHeight() / 2});

    m_searchBG = geode::NineSlice::create("GJ_square02.png");
    m_searchBG->setAnchorPoint({0, 0});
    m_searchBG->setContentSize(getContentSize());

    addChild(m_searchBG);

    m_searchBG->addChild(m_searchInput);

    setScale(0.6f);

    m_clearButton = geode::Button::createWithSpriteFrameName("GJ_longBtn07_001.png", [this] (auto sender) {
        m_searchInput->setString("", true);
    });

    m_clearButton->setPosition({getContentWidth() - 6 - m_clearButton->getContentWidth() / 2, getContentHeight() / 2});
    m_searchBG->addChild(m_clearButton);

    return true;
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
    if (!nodeIsVisible(this) || !alpha::utils::isPointInsideNode(this, touch->getLocation())) return false;
    return true;
}
