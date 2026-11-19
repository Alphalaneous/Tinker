#pragma once

#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/cocos/menu_nodes/CCMenu.h>
#include <Geode/ui/Label.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/StringMap.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/utils/cocos.hpp>
#include <alphalaneous.alphas-ui-pack/API.hpp>
#include <memory>
#include <unordered_map>
#include <vector>
#include "settings/SettingNode.hpp"
#include "settings/SettingsCache.hpp"

namespace tinker::ui {

class SettingsPopup : public geode::Popup {
public:
    static SettingsPopup* create(bool useGeodeTheme);
    bool init(bool useGeodeTheme);
    void onClose(CCObject* sender) override;
protected:

    void loadSettingNodes(bool retainPosition = true);
    void loadSettings();
    void applyUncommitted();
    void switchCategory();
    bool hasUncommitted();
    cocos2d::CCMenu* createCategoryButton(geode::ZStringView name, geode::ZStringView id);

    std::string m_searchQuery = "";
    std::string m_category = "";
    bool m_loadingSettings = false;
    geode::utils::StringMap<bool> m_categoryCollapsed{};
    geode::Label* m_titleLabel = nullptr;
    geode::TextInput* m_searchInput = nullptr;
    alpha::ui::AdvancedScrollLayer* m_settingScrollLayer = nullptr;
    alpha::ui::AdvancedScrollLayer* m_categoryScrollLayer = nullptr;
    std::vector<CCMenuItemSpriteExtra*> m_categoryButtons{};
    std::vector<geode::Ref<tinker::settings::SettingNodeBase>> m_settingNodes{};
    std::unordered_map<geode::Ref<tinker::settings::SettingNodeBase>, std::shared_ptr<SettingsCache::SettingInfo>> m_settingNodeMap{};
};

}