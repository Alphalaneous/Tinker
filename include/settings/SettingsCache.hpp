#pragma once

#include <Geode/Geode.hpp>
#include "utils/Singleton.hpp"

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include <Geode/external/fts/fts_fuzzy_match.h>

using namespace geode::prelude;

class SettingsCache : public Singleton<SettingsCache> {
public:
    struct SettingCategory;

    struct SettingInfo {
        std::string id;
        std::string name;
        std::string type;
        SettingCategory* category;
        bool hasEnableIf = false;
    };

    struct SettingCategory {
        std::string name;
        std::string id;
        std::set<std::shared_ptr<SettingInfo>> settings;

        bool weightedFuzzyMatch(ZStringView str, ZStringView kw, double weight, double& out) {
            int score;
            if (fts::fuzzy_match(kw.c_str(), str.c_str(), score)) {
                out = std::max(out, score * weight);
                return true;
            }
            return false;
        }

        bool matchSearch(std::shared_ptr<SettingInfo> info, ZStringView query) {
            if (info->type == "title") {
                return true;
            }

            bool addToList = false;
            double weighted = 0;
            addToList |= weightedFuzzyMatch(info->id, query, 0.5, weighted);
            addToList |= weightedFuzzyMatch(info->name, query, 1, weighted);
            
            if (weighted < 60.0 + 10.0 * query.size()) {
                addToList = false;
            }
            addToList |= weightedFuzzyMatch(name, query, 1, weighted);

            return addToList;
        }

        std::set<std::shared_ptr<SettingInfo>> settingsForSearch(ZStringView query) {
            if (query.empty()) {
                return settings;
            }
            std::set<std::shared_ptr<SettingInfo>> set;

            std::shared_ptr<SettingInfo> title;

            for (auto& setting : settings) {
                if (matchSearch(setting, query)) {
                    set.insert(setting);

                    if (setting->type == "title") {
                        title = setting;
                    }
                }
            }

            if (set.size() == 1 && set.contains(title)) {
                set.clear();
            }

            return set;
        }
    };

    const StringMap<SettingCategory>& getSettings();
    const StringMap<std::shared_ptr<SettingInfo>>& getSettingsMap();
    const std::vector<std::shared_ptr<SettingInfo>>& getSettingsList();
    const std::vector<SettingCategory*>& getCategoryList();

    void loadSettings();

protected:
    StringMap<SettingCategory> m_settings;
    StringMap<std::shared_ptr<SettingInfo>> m_settingsMap;
    std::vector<std::shared_ptr<SettingInfo>> m_settingsVec;
    std::vector<SettingCategory*> m_categoryVec;
};