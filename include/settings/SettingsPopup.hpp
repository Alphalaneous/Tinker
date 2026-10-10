#pragma once

#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/cocos/menu_nodes/CCMenu.h>
#include <Geode/ui/Label.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/StringMap.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/utils/cocos.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <memory>
#include <unordered_map>
#include <vector>
#include "settings/SettingNode.hpp"
#include "settings/SettingsCache.hpp"

using namespace geode::prelude;

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
    CCMenu* createCategoryButton(ZStringView name, ZStringView id);

    std::string m_searchQuery;
    std::string m_category;
    bool m_loadingSettings;
    StringMap<bool> m_categoryCollapsed;
    geode::Label* m_titleLabel;
    geode::TextInput* m_searchInput;
    alpha::ui::AdvancedScrollLayer* m_settingScrollLayer;
    alpha::ui::AdvancedScrollLayer* m_categoryScrollLayer;
    std::vector<CCMenuItemSpriteExtra*> m_categoryButtons;
    std::vector<Ref<tinker::settings::SettingNodeBase>> m_settingNodes;
    std::unordered_map<Ref<tinker::settings::SettingNodeBase>, std::shared_ptr<SettingsCache::SettingInfo>> m_settingNodeMap;
};

}