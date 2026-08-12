#include "nodes/SearchField.hpp"
#include <alphalaneous.alphas-ui-pack/include/Utils.hpp>
#include <smjs.object-collab/include/object_collab_optional.hpp>
#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include <Geode/external/fts/fts_fuzzy_match.h>
#include "utils/Utils.hpp"
#include "modules/ObjectSearch.hpp"
#include "modules/ScrollableObjects.hpp"

namespace tinker::ui {

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

    auto lower = geode::utils::string::toLower(search);
    geode::utils::string::trimIP(lower);

    if (lower.empty()) {
        for (const auto& item : fields->m_orderedItems) {
            if (!item->item) continue;
            arr->addObject(item->item);
        }
        return arr;
    }

    if (lower.starts_with("id:")) {
        std::vector<std::string> parts = tinker::utils::split(lower, ":");
        if (parts.size() == 2) {
            Result<int> numRes = numFromString<int>(geode::utils::string::trim(parts[1]));
            if (numRes.isOk()) {
                int id = numRes.unwrap();

                auto info = infoForID(id);
                if (info) arr->addObject(info.unwrap().item);
            }
            else {
                auto registryRes = object_collab::getOptionalRegister();
                if (registryRes) {
                    auto registry = registryRes.unwrap();
                    for (auto& [k, v] : registry) {
                        if (v.id == parts[1]) {
                            auto info = infoForID(k);
                            if (info) arr->addObject(info.unwrap().item);
                        }
                    }
                }
            }
        }
    }
    else if (lower.starts_with("exact:")) {
        auto& items = static_cast<OSEditorUI*>(ObjectSearch::get()->getEditor())->m_fields->m_items;
        std::vector<std::string> parts = tinker::utils::split(lower, ":", 2);
        if (parts.size() == 2) {
            for (auto& [k, v] : items) {
                std::string lowerV = geode::utils::string::toLower(v.name);
                if (lowerV == geode::utils::string::trim(parts[1])) {
                    auto info = infoForID(k);
                    if (info) arr->addObject(info.unwrap().item);
                }
            }
        }
    }
    else {
        struct NameScore {
            unsigned int id;
            std::string name;
            int score;
        };

        std::vector<NameScore> nameScores;

        auto queryWords = geode::utils::string::split(lower, " ");
        auto& items = static_cast<OSEditorUI*>(ObjectSearch::get()->getEditor())->m_fields->m_items;

        for (const auto& [k, v] : items) {
            auto nameWords = geode::utils::string::split(geode::utils::string::toLower(v.name), " ");

            int totalScore = 0;
            bool matches = true;

            for (const auto& queryWord : queryWords) {
                bool found = false;
                int bestScore = INT_MIN;

                for (const auto& nameWord : nameWords) {
                    int score = 0;

                    if (fts::fuzzy_match(queryWord.c_str(), nameWord.c_str(), score)) {
                        found = true;
                        bestScore = std::max(bestScore, score);
                    }
                }

                if (!found) {
                    matches = false;
                    break;
                }

                totalScore += bestScore;
            }

            if (matches) {
                nameScores.push_back({k, v.name, totalScore});
            }
        }

        std::unordered_map<unsigned int, std::vector<ItemInformation>> sections;
        std::vector<std::vector<ItemInformation>*> sectionsVec;

        if (!nameScores.empty()) {
            std::sort(nameScores.begin(), nameScores.end(), [&](const auto& a, const auto& b) {
                return a.score > b.score;
            });

            for (const auto& score : nameScores) {
                auto infoRes = infoForID(score.id);
                if (infoRes) {
                    auto& info = infoRes.unwrap();
                    auto item = info.item;
                    auto& section = sections[info.tabIndex];
                    section.push_back(info);
                }
            }

            for (auto& [k, v] : sections) {
                sectionsVec.push_back(&v);
            }

            std::sort(sectionsVec.begin(), sectionsVec.end(), [&](const auto& a, const auto& b) {
                return a->size() > b->size();
            });

            for (const auto& section : sectionsVec) {
                for (const auto& item : *section) {
                    arr->addObject(item.item);
                }
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
    setContentSize({300.f, 45.f});

    m_searchInput = geode::TextInput::create(getContentWidth() - 55.f, "Search...");
    m_searchInput->setDelegate(this);
    m_searchInput->setCommonFilter(CommonFilter::Any);
    m_searchInput->setPosition({6.f + m_searchInput->getContentWidth() / 2.f, getContentHeight() / 2.f});
    m_searchInput->setID("search-input"_spr);

    m_searchBG = geode::NineSlice::create("GJ_square02.png");
    m_searchBG->setAnchorPoint({0.f, 0.f});
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
        setupTabOffset();
        #endif
    });

    m_clearButton->setPosition({getContentWidth() - 6.f - m_clearButton->getContentWidth() / 2.f, getContentHeight() / 2.f});
    m_searchBG->addChild(m_clearButton);

    m_tabBG = geode::NineSlice::create("square02b_001.png");
    m_tabBG->setOpacity(127);
    m_tabBG->setColor({0, 0, 0});
    m_tabBG->setID("floating-tab-background"_spr);

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
        auto winSize = CCDirector::get()->getWinSize();

        auto tab = m_editorUI->m_fields->m_searchBar;
        tab->setPositionY(winSize.height - tab->getScaledContentHeight() - 10.f);
        #endif
    }));
}

void SearchField::textInputOpened(CCTextInputNode* node) {
    #ifdef GEODE_IS_MOBILE
    m_inputFocused = true;

    runAction(CallFuncExt::create([this] {
        setupTabOffset();
    }));

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
            onClosed();
        }
        m_lockClose = false;
    }));
    #endif
}

void SearchField::onClosed() {
    m_inputFocused = false;

    float buildTabHeight = 0.f;
    float scale = 1.f;
    if (auto node = m_editorUI->getChildByID("build-tabs-menu")) {
        buildTabHeight = node->getScaledContentHeight();
        scale = node->getScale();
    }

    setPosition({m_editorUI->getContentWidth() / 2, tinker::utils::getToolbarHeight() + 5.f * scale + buildTabHeight});

    auto tab = m_editorUI->m_fields->m_searchBar;

    tab->setPositionY(0.f);
    m_tabBG->removeFromParent();

    tab->setZOrder(10);
}

void SearchField::textInputShouldOffset(CCTextInputNode* node, float yOffset) {
    #ifdef GEODE_IS_MOBILE
    m_yOffset = std::max(yOffset, tinker::utils::getToolbarHeight() + 15.f);
   
    runAction(CallFuncExt::create([this] {
        setupTabOffset();
    }));
    #endif
}

void SearchField::setupTabOffset() {
    auto winSize = CCDirector::get()->getWinSize();
    auto tab = m_editorUI->m_fields->m_searchBar;

    #ifdef GEODE_IS_ANDROID
        m_yOffset = winSize.height - tab->getScaledContentHeight() - 20.f - getScaledContentHeight();
    #endif

    setPositionY(m_yOffset);

    float heightOffset = 2.f;
    if (ScrollableObjects::isEnabled()) {
        heightOffset = ScrollableObjects::getSetting<float, "y-offset">();
    }

    tab->setPositionY(winSize.height - tab->getScaledContentHeight() - 10.f);
    
    m_tabBG->setContentSize(tab->getScaledContentSize() + CCSize{0.f, 10.f - heightOffset * tab->getScale() + 2.f});
    m_tabBG->setPosition(tab->getPosition() + CCPoint{0.f, tab->getScaledContentHeight() / 2.f});
    tab->setZOrder(20);
    m_tabBG->setZOrder(19);

    m_tabBG->removeFromParent();

    m_editorUI->addChild(m_tabBG);
}

}