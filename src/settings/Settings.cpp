#include <Geode/Geode.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/ui/Button.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include <alphalaneous.alphas-ui-pack/include/nodes/scroll/AdvancedScrollLayer.hpp>
#include <alphalaneous.alphas-ui-pack/include/Utils.hpp>

using namespace geode::prelude;

class TitleSettingNodeV3 : public CCNode {};

class $nodeModify(TinkerModSettingsPopup, ModSettingsPopup) {

    struct Category {
        CCMenuItemToggler* toggler;
        std::string name;
        geode::Button* tabButton;
        geode::NineSlice* buttonBg;
        std::vector<SettingNodeV3*> settings;
        SettingNodeV3* titleSetting;
        bool isGeodeTheme = true;

        void collapse() {
            buttonBg->setColor(isGeodeTheme ? ccColor3B{26, 24, 29} : ccColor3B{54, 31, 16});
            toggler->toggleWithCallback(true);
        }
        void expand(bool setColor = true) {
            if (setColor) {
                buttonBg->setColor(isGeodeTheme ? ccColor3B{168, 147, 185} : ccColor3B{248, 200, 43});
            }
            toggler->toggleWithCallback(false);
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
    };

    struct Fields {
        StringMap<Category> m_categories;
        alpha::ui::AdvancedScrollLayer* m_scrollLayer;
        geode::ScrollLayer* m_mainScrollLayer;
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
        }
        #ifdef GEODE_IS_DESKTOP
        schedule(schedule_selector(TinkerModSettingsPopup::checkMousePos));
        #endif
    }

    void checkMousePos(float dt) {
        auto fields = m_fields.self();
        fields->m_mainScrollLayer->enableScrollWheel(alpha::utils::isPointInsideNode(fields->m_mainScrollLayer, getMousePos()));
    }

    void customSetup() {
        auto alert = reinterpret_cast<FLAlertLayer*>(this);
        auto fields = m_fields.self();

        auto layerColor = alert->m_mainLayer->getChildByType<CCLayerColor>(0);
        if (!layerColor) return;

        auto listBorders = alert->m_mainLayer->getChildByType<geode::ListBorders>(0);
        if (!listBorders) return;

        auto scrollbar = alert->m_mainLayer->getChildByType<geode::Scrollbar>(0);
        if (!scrollbar) return;

        auto scrollLayer = layerColor->getChildByType<ScrollLayer>(0);
        if (!scrollLayer) return;

        fields->m_mainScrollLayer = scrollLayer;

        auto searchMenu = layerColor->getChildByType<CCMenu>(0);
        if (!searchMenu) return;

        layerColor->setContentHeight(layerColor->getContentHeight() / 0.8f);
        searchMenu->setPositionY(layerColor->getContentHeight());

        bool geodeTheme = isGeodeTheme();

        float width = 346.f;
        float height = geodeTheme ? 250.f : 255.f;

        layerColor->setScale(0.75f);
        layerColor->setAnchorPoint({1.f, 0.5f});

        scrollLayer->setContentHeight(layerColor->getContentHeight() - 30);

        layerColor->setPositionX(alert->m_mainLayer->getContentWidth() - 50);
        listBorders->setPositionX(alert->m_mainLayer->getContentWidth() - 50);

        scrollbar->setScale(0.8);
        scrollbar->addOnEnterCallback([scrollbar] () {
            static_cast<CCNode*>(scrollbar)->draw();
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

        auto tabButton = geode::Button::createWithNode(allBtnBg, [geodeTheme, fields, allBtnBg] (auto sender) {
            for (auto& [key, category] : fields->m_categories) {
                category.expand(false);
                category.buttonBg->setColor(geodeTheme ? ccColor3B{26, 24, 29} : ccColor3B{54, 31, 16});
                category.showTitle();
                category.tabButton->setEnabled(true);
            }
            allBtnBg->setColor(geodeTheme ? ccColor3B{168, 147, 185} : ccColor3B{248, 200, 43});
            sender->setEnabled(false);
        });
        tabButton->setScaleMultiplier(1.1f);
        tabButton->setZOrder(-1);
        tabButton->setEnabled(false);
        fields->m_scrollLayer->getContentLayer()->addChild(tabButton);

        Category* currentCategory;

        int idx = 0;

        for (auto child : scrollLayer->m_contentLayer->getChildrenExt()) {
            auto settingNode = reinterpret_cast<SettingNodeV3*>(child);

            if (auto title = typeinfo_cast<TitleSettingNodeV3*>(child)) {
                auto setting = settingNode->getSetting();

                auto toggler = settingNode->getButtonMenu()->getChildByType<CCMenuItemToggler>(0);
                auto category = Category{toggler, setting->getDisplayName()};

                fields->m_categories[setting->getKey()] = category;
                auto ptr = &fields->m_categories[setting->getKey()];

                currentCategory = ptr;

                auto btnBg = geode::NineSlice::create("tab-bg.png"_spr);
                btnBg->setContentSize({90, 20});
                btnBg->setScaleMultiplier(0.5f);
                btnBg->setColor(geodeTheme ? ccColor3B{26, 24, 29} : ccColor3B{54, 31, 16});

                auto label = CCLabelBMFont::create(setting->getDisplayName().c_str(), "bigFont.fnt");
                label->setPosition(allBtnBg->getContentSize() / 2 + CCPoint{0, 0.5f});
                geode::cocos::limitNodeSize(label, btnBg->getContentSize() - CCSize{12, 8}, 1.f, 0.05f);
                btnBg->addChild(label);

                ptr->buttonBg = btnBg;
                ptr->titleSetting = settingNode;
                ptr->isGeodeTheme = geodeTheme;
                ptr->tabButton = geode::Button::createWithNode(btnBg, [geodeTheme, ptr, fields, allBtnBg, tabButton] (auto sender) {
                    for (auto& [key, category] : fields->m_categories) {
                        category.hideTitle();
                        category.collapse();
                        category.tabButton->setEnabled(true);
                    }
                    allBtnBg->setColor(geodeTheme ? ccColor3B{26, 24, 29} : ccColor3B{54, 31, 16});
                    ptr->expand();
                    sender->setEnabled(false);
                    tabButton->setEnabled(true);
                });
                ptr->tabButton->setScaleMultiplier(1.1f);
                ptr->tabButton->setZOrder(idx);

                fields->m_scrollLayer->getContentLayer()->addChild(ptr->tabButton);
                idx++;
                continue;
            }
            if (currentCategory) {
                currentCategory->settings.push_back(settingNode);
            }
        }

        fields->m_scrollLayer->getContentLayer()->updateLayout();

        scrollLayer->m_contentLayer->updateLayout(false);
        scrollLayer->scrollToTop();

        float offset = geodeTheme ? 0 : -5;

        auto tabListBorders = createGeodeListBorders({100 / 0.75f + offset, height + offset});
        tabListBorders->setAnchorPoint({0.f, 0.5f});
        tabListBorders->setPosition(background->getPosition());
        tabListBorders->setZOrder(1);
        tabListBorders->setScale(0.75f);

        alert->m_mainLayer->addChild(tabListBorders);
    }
};