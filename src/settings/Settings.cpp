#include <Geode/Geode.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/ui/Button.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include <alphalaneous.alphas-ui-pack/include/nodes/scroll/AdvancedScrollLayer.hpp>
#include <alphalaneous.alphas-ui-pack/include/Utils.hpp>

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include <Geode/external/fts/fts_fuzzy_match.h>

using namespace geode::prelude;


class TitleSettingNodeV3 : public CCNode {};

class $nodeModify(TinkerModSettingsPopup, ModSettingsPopup) {

    struct Category {
        CCMenuItemToggler* toggler;
        std::string name;
        geode::Button* tabButton;
        geode::NineSlice* buttonBg;
        std::vector<Ref<SettingNodeV3>> settings;
        Ref<SettingNodeV3> titleSetting;
        CCNode* content;
        bool isGeodeTheme = true;

        std::set<Ref<SettingNodeV3>> foundSettings;

        void collapse() {
            buttonBg->setColor(isGeodeTheme ? ccColor3B{26, 24, 29} : ccColor3B{54, 31, 16});

            titleSetting->removeFromParentAndCleanup(false);

            for (auto setting : settings) {
                setting->removeFromParentAndCleanup(false);
            }
        }

        void expand(bool setColor = true) {            
            if (setColor) {
                buttonBg->setColor(isGeodeTheme ? ccColor3B{168, 147, 185} : ccColor3B{248, 200, 43});
            }
            if (!titleSetting->getParent()) content->addChild(titleSetting);
            
            for (auto setting : settings) {
                if (!content->getParent()) content->addChild(setting);
            }
        }

        void hideTitle() {
            titleSetting->setScale(0);
            titleSetting->getButtonMenu()->setVisible(false);
            titleSetting->getNameLabel()->setVisible(false);
        }
        void showTitle() {
            titleSetting->setScale(1);
            titleSetting->getButtonMenu()->setVisible(true);
            titleSetting->getNameLabel()->setVisible(true);
        }

        void updateState(bool setColor = true, bool checkToggle = false) {
            collapse();
            if (checkToggle && toggler->isToggled()) {
                if (!titleSetting->getParent()) content->addChild(titleSetting);
                return;
            }
            expand(setColor);
        }

        bool weightedFuzzyMatch(ZStringView str, ZStringView kw, double weight, double& out) {
            int score;
            if (fts::fuzzy_match(kw.c_str(), str.c_str(), score)) {
                out = std::max(out, score * weight);
                return true;
            }
            return false;
        }

        bool matchSearch(SettingNode* node, ZStringView query) {
            if (typeinfo_cast<TitleSettingNodeV3*>(node)) {
                return true;
            }

            bool addToList = false;
            auto setting = node->getSetting();
            double weighted = 0;
            if (auto name = setting->getName()) {
                addToList |= weightedFuzzyMatch(setting->getKey(), query, 0.5, weighted);
                addToList |= weightedFuzzyMatch(*name, query, 1, weighted);
            }
            // If there's no name, give full weight to key
            else {
                addToList |= weightedFuzzyMatch(setting->getKey(), query, 1, weighted);
            }
            if (weighted < 60.0 + 10.0 * query.size()) {
                addToList = false;
            }
            addToList |= weightedFuzzyMatch(titleSetting->getSetting()->getDisplayName(), query, 1, weighted);

            return addToList;
        }

        bool checkSearch(ZStringView search, geode::SettingNodeV3* settingNode, bool full) {
            auto hasSearch = !search.empty();

            if (typeinfo_cast<TitleSettingNodeV3*>(settingNode)) {
                return true;
            }

            foundSettings.erase(settingNode);
            settingNode->removeFromParent();

            if (!hasSearch || matchSearch(settingNode, search)) {
                if (!full || !toggler->isToggled()) {
                    content->addChild(settingNode);
                }
                foundSettings.insert(settingNode);
                return true;
            }
            return false;
        }

        void search(ZStringView search, bool& bg, bool full = true) {
            for (auto setting : settings) {
                if (checkSearch(search, setting, full)) {
                    setting->setDefaultBGColor(ccc4(0, 0, 0, bg ? 60 : 20));
                    bg = !bg;
                }
            }

            if (full && !checkIfNeedsTitle()) {
                bg = !bg;
            }
        }

        bool checkIfNeedsTitle() {
            if (foundSettings.empty()) {
                hideTitle();
                return false;
            }
            showTitle();
            return true;
        }
    };

    struct Fields {
        std::vector<Category> m_categories;
        alpha::ui::AdvancedScrollLayer* m_scrollLayer;
        Ref<geode::ScrollLayer> m_mainScrollLayer;
        geode::ScrollLayer* m_replacementScrollLayer;
        Category* m_activeCategory = nullptr;
        geode::TextInput* m_searchInput;
        geode::Scrollbar* m_scrollBar;
        CCMenuItemSpriteExtra* m_searchClearBtn;
    };

    bool isGeodeTheme() {
        auto alert = reinterpret_cast<FLAlertLayer*>(this);

        auto listBorders = alert->m_mainLayer->getChildByType<ListBorders>(0);
        if (!listBorders) return false;

        auto sprite = listBorders->getChildByType<CCSprite>(0);
        if (geode::cocos::isSpriteFrameName(sprite, "geode.loader/geode-list-side.png")) {
            return true;
        }
        return false;
    }

    ListBorders* createGeodeListBorders(CCSize const& size) {
        auto ret = ListBorders::create();
        const bool geodeTheme = isGeodeTheme();
        if (geodeTheme) {
            ret->setSpriteFrames("geode.loader/geode-list-top.png", "geode.loader/geode-list-side.png", 2);
            ret->setContentSize(size);
        } else {
            ret->setContentSize(size + ccp(5, 5));
        }
        return ret;
    }

    void modify() {
        auto alert = reinterpret_cast<FLAlertLayer*>(this);

        auto label = alert->m_mainLayer->getChildByType<CCLabelBMFont>(0);
        if (label && std::string_view(label->getString()) == "Settings for Tinker") {
            customSetup();
            addEventListener(SettingNodeValueChangeEvent(), [this] (std::string_view modID, std::string_view key, SettingNodeV3* node, bool isCommit) {
                updateState();
            });
            #ifdef GEODE_IS_DESKTOP
            schedule(schedule_selector(TinkerModSettingsPopup::checkMousePos));
            #endif
        }
    }

    void checkMousePos(float dt) {
        auto fields = m_fields.self();
        fields->m_replacementScrollLayer->enableScrollWheel(alpha::utils::isPointInsideNode(fields->m_replacementScrollLayer, getMousePos()));
    }

    void updateState() {
        auto alert = reinterpret_cast<FLAlertLayer*>(this);
        auto fields = m_fields.self();

        auto listPosBefore = fields->m_replacementScrollLayer->m_contentLayer->getPositionY();
        auto listHeightBefore = fields->m_replacementScrollLayer->m_contentLayer->getContentHeight();

        auto search = fields->m_searchInput->getString();
        auto hasSearch = !search.empty();

        auto clearSpr = static_cast<CCSprite*>(fields->m_searchClearBtn->getNormalImage());
        fields->m_searchClearBtn->setEnabled(hasSearch);
        clearSpr->setColor(hasSearch ? ccWHITE : ccGRAY);
        clearSpr->setOpacity(hasSearch ? 255 : 90);

        auto childSpr = clearSpr->getChildByType<CCSprite>(0);

        if (childSpr) {
            childSpr->setColor(hasSearch ? ccWHITE : ccGRAY);
            childSpr->setOpacity(hasSearch ? 255 : 90);
        }

        bool bg = false;

        if (!fields->m_activeCategory) {
            for (auto& category : fields->m_categories) {
                category.updateState(false, true);
                category.titleSetting->setDefaultBGColor(ccc4(0, 0, 0, bg ? 60 : 20));
                bg = !bg;

                category.search(search, bg);
            }
        }
        else {
            for (auto& category : fields->m_categories) {
                category.collapse();
            }
            fields->m_activeCategory->updateState();
            fields->m_activeCategory->titleSetting->setDefaultBGColor(ccc4(0, 0, 0, bg ? 60 : 20));
            bg = !bg;

            fields->m_activeCategory->search(search, bg, false);
        }

        fields->m_replacementScrollLayer->m_contentLayer->updateLayout();        
        fields->m_replacementScrollLayer->m_contentLayer->setPositionY(
            listPosBefore +
                (listHeightBefore - fields->m_replacementScrollLayer->m_contentLayer->getContentHeight())
        );

        bool usesScroll = fields->m_replacementScrollLayer->m_contentLayer->getContentHeight() > fields->m_replacementScrollLayer->getContentHeight();
        fields->m_scrollBar->getThumb()->setVisible(usesScroll);
        fields->m_scrollBar->getTrack()->setOpacity(usesScroll ? 150 : 50);
    }

    void onClearSearch(CCObject* sender) {
        auto fields = m_fields.self();
        
        fields->m_searchInput->setString("");
        updateState();
        fields->m_replacementScrollLayer->moveToTop();
    }

    void customSetup() {
        auto alert = reinterpret_cast<FLAlertLayer*>(this);
        auto fields = m_fields.self();

        auto layerColor = alert->m_mainLayer->getChildByType<CCLayerColor>(0);
        if (!layerColor) return;

        auto listBorders = alert->m_mainLayer->getChildByType<geode::ListBorders>(0);
        if (!listBorders) return;

        fields->m_scrollBar = alert->m_mainLayer->getChildByType<geode::Scrollbar>(0);
        if (!fields->m_scrollBar) return;

        auto scrollLayer = layerColor->getChildByType<ScrollLayer>(0);
        if (!scrollLayer) return;

        fields->m_mainScrollLayer = scrollLayer;

        auto searchMenu = layerColor->getChildByType<CCMenu>(0);
        if (!searchMenu) return;

        layerColor->setContentHeight(layerColor->getContentHeight() / 0.8f);
        searchMenu->setPositionY(layerColor->getContentHeight());

        fields->m_searchInput = typeinfo_cast<geode::TextInput*>(searchMenu->getChildByID("search-input"));
        if (!fields->m_searchInput) return;

        fields->m_searchClearBtn = searchMenu->getChildByType<CCMenuItemSpriteExtra>(0);
        if (!fields->m_searchClearBtn) return;

        fields->m_searchClearBtn->setTarget(this, menu_selector(TinkerModSettingsPopup::onClearSearch));

        fields->m_searchInput->setCallback([this, fields] (auto const&) {
            updateState();
            fields->m_replacementScrollLayer->scrollToTop();
        });

        bool geodeTheme = isGeodeTheme();

        float width = 346.f;
        float height = geodeTheme ? 250.f : 255.f;

        layerColor->setScale(0.75f);
        layerColor->setAnchorPoint({1.f, 0.5f});

        scrollLayer->setContentHeight(layerColor->getContentHeight() - 30);

        layerColor->setPositionX(alert->m_mainLayer->getContentWidth() - 50);
        listBorders->setPositionX(alert->m_mainLayer->getContentWidth() - 50);

        fields->m_scrollBar->setScale(0.8);
        fields->m_scrollBar->addOnEnterCallback([fields] () {
            static_cast<CCNode*>(fields->m_scrollBar)->draw();
        });
        listBorders->setScale(0.75f);
        listBorders->setContentSize({width, height});
        listBorders->setAnchorPoint({1.f, 0.5f});

        auto background = CCLayerColor::create({0, 0, 0, 75});
        background->setContentSize({100, layerColor->getScaledContentHeight()});
        background->setPosition({25, alert->m_mainLayer->getContentHeight() / 2});
        background->setAnchorPoint({0.f, 0.5f});
        background->ignoreAnchorPointForPosition(false);

        fields->m_scrollLayer = alpha::ui::AdvancedScrollLayer::create({100, layerColor->getScaledContentHeight()});
        fields->m_scrollLayer->setPosition(background->getContentSize() / 2);
        fields->m_scrollLayer->setContentSize({100, layerColor->getScaledContentHeight()});
        fields->m_scrollLayer->ignoreAnchorPointForPosition(false);

        auto layout = static_cast<SimpleAxisLayout*>(ScrollLayer::createDefaultListLayout());
        layout->setPadding({4, 4, 4, 4});
        fields->m_scrollLayer->getContentLayer()->setLayout(layout);

        background->addChild(fields->m_scrollLayer);

        alert->m_mainLayer->addChild(background);

        auto allBtnBg = geode::NineSlice::create("tab-bg.png"_spr);
        allBtnBg->setContentSize({90, 20});
        allBtnBg->setScaleMultiplier(0.5f);
        allBtnBg->setColor(geodeTheme ? ccColor3B{168, 147, 185} : ccColor3B{248, 200, 43});

        auto label = CCLabelBMFont::create("All Settings", "bigFont.fnt");
        label->setPosition(allBtnBg->getContentSize() / 2 + CCPoint{0, 0.5f});
        geode::cocos::limitNodeSize(label, allBtnBg->getContentSize() - CCSize{12, 8}, 1.f, 0.05f);
        allBtnBg->addChild(label);

        auto tabButton = geode::Button::createWithNode(allBtnBg, [this, scrollLayer, geodeTheme, fields, allBtnBg] (auto sender) {
            
            for (auto& category : fields->m_categories) {
                category.collapse();
            }
            for (auto& category : fields->m_categories) {
                category.buttonBg->setColor(geodeTheme ? ccColor3B{26, 24, 29} : ccColor3B{54, 31, 16});
                category.showTitle();
                category.expand(false);
                category.tabButton->setEnabled(true);
            }
            allBtnBg->setColor(geodeTheme ? ccColor3B{168, 147, 185} : ccColor3B{248, 200, 43});
            sender->setEnabled(false);

            fields->m_activeCategory = nullptr;
            updateState();
            fields->m_replacementScrollLayer->scrollToTop();
        });
        tabButton->setScaleMultiplier(1.1f);
        tabButton->setZOrder(-1);
        tabButton->setEnabled(false);
        fields->m_scrollLayer->getContentLayer()->addChild(tabButton);
        
        fields->m_replacementScrollLayer = geode::ScrollLayer::create({0, 0, scrollLayer->getContentWidth(), scrollLayer->getContentHeight()});
        fields->m_replacementScrollLayer->setPosition(scrollLayer->getPosition());
        fields->m_replacementScrollLayer->setAnchorPoint(scrollLayer->getAnchorPoint());
        fields->m_replacementScrollLayer->setContentSize(scrollLayer->getContentSize());
        fields->m_replacementScrollLayer->setZOrder(scrollLayer->getZOrder());
        fields->m_replacementScrollLayer->setScale(scrollLayer->getScale());
        fields->m_replacementScrollLayer->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout(0));
        fields->m_scrollBar->setTarget(fields->m_replacementScrollLayer);

        layerColor->addChild(fields->m_replacementScrollLayer);
        scrollLayer->removeFromParentAndCleanup(false);

        int idx = 0;

        for (auto child : scrollLayer->m_contentLayer->getChildrenExt()) {
            auto settingNode = reinterpret_cast<SettingNodeV3*>(child);

            if (auto title = typeinfo_cast<TitleSettingNodeV3*>(child)) {
                auto setting = settingNode->getSetting();

                auto toggler = settingNode->getButtonMenu()->getChildByType<CCMenuItemToggler>(0);
                auto category = Category{toggler, setting->getDisplayName()};

                auto btnBg = geode::NineSlice::create("tab-bg.png"_spr);
                btnBg->setContentSize({90, 20});
                btnBg->setScaleMultiplier(0.5f);
                btnBg->setColor(geodeTheme ? ccColor3B{26, 24, 29} : ccColor3B{54, 31, 16});

                auto label = CCLabelBMFont::create(setting->getDisplayName().c_str(), "bigFont.fnt");
                label->setPosition(allBtnBg->getContentSize() / 2 + CCPoint{0, 0.5f});
                geode::cocos::limitNodeSize(label, btnBg->getContentSize() - CCSize{12, 8}, 1.f, 0.05f);
                btnBg->addChild(label);

                category.buttonBg = btnBg;
                category.titleSetting = settingNode;
                category.isGeodeTheme = geodeTheme;
                category.tabButton = geode::Button::createWithNode(btnBg, [this, idx, scrollLayer, geodeTheme, fields, allBtnBg, tabButton] (auto sender) mutable {
                    for (auto& category : fields->m_categories) {
                        category.hideTitle();
                        category.collapse();
                        category.tabButton->setEnabled(true);
                    }
                    allBtnBg->setColor(geodeTheme ? ccColor3B{26, 24, 29} : ccColor3B{54, 31, 16});
                    fields->m_categories[idx].expand();
                    sender->setEnabled(false);
                    tabButton->setEnabled(true);
                    fields->m_activeCategory = &fields->m_categories[idx];
                    updateState();
                    fields->m_replacementScrollLayer->scrollToTop();
                });
                category.tabButton->setScaleMultiplier(1.1f);
                category.tabButton->setZOrder(idx);
                category.content = fields->m_replacementScrollLayer->m_contentLayer;

                fields->m_scrollLayer->getContentLayer()->addChild(category.tabButton);
                fields->m_categories.push_back(std::move(category));

                idx++;
                continue;
            }
            if (idx > 0) {
                fields->m_categories[idx-1].settings.push_back(settingNode);
            }
        }

        fields->m_scrollLayer->getContentLayer()->updateLayout();

        scrollLayer->m_contentLayer->updateLayout();
        scrollLayer->scrollToTop();

        updateState();
        fields->m_replacementScrollLayer->scrollToTop();

        float offset = geodeTheme ? 0 : -5;

        auto tabListBorders = createGeodeListBorders({100 / 0.75f + offset, height + offset});
        tabListBorders->setAnchorPoint({0.f, 0.5f});
        tabListBorders->setPosition(background->getPosition());
        tabListBorders->setZOrder(1);
        tabListBorders->setScale(0.75f);

        alert->m_mainLayer->addChild(tabListBorders);
    }
};