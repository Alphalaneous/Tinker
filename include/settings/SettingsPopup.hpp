#pragma once

#include "settings/SettingNode.hpp"
#include "settings/SettingsCache.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class SettingsPopup : public geode::Popup {
public:
    static SettingsPopup* create();
    bool init() override;
    void onClose(CCObject* sender) override;
protected:

    void loadSettingNodes(bool retainPosition = true);
    void loadSettings();
    void applyUncommitted();
    void switchCategory();
    bool hasUncommitted();
    CCMenu* createCategoryButton(ZStringView name, ZStringView id);

    std::string m_searchQuery;
    std::string m_category;
    StringMap<bool> m_categoryCollapsed;
    CCLabelBMFont* m_titleLabel;
    alpha::ui::AdvancedScrollLayer* m_settingScrollLayer;
    alpha::ui::AdvancedScrollLayer* m_categoryScrollLayer;
    std::vector<CCMenuItemSpriteExtra*> m_categoryButtons;
    std::vector<Ref<tinker::settings::SettingNodeBase>> m_settingNodes;
    std::unordered_map<Ref<tinker::settings::SettingNodeBase>, std::shared_ptr<SettingsCache::SettingInfo>> m_settingNodeMap;
};