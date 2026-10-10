#pragma once

#include "utils/Singleton.hpp"
#include <Geode/utils/web.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <unordered_map>

using namespace geode::prelude;

class ObjectNames : public Singleton<ObjectNames> {
public:
    void checkNames(bool ignoreVersion = false);
    Result<ZStringView> getName(unsigned int id);
    std::string deduceFromID(ZStringView id);
    const std::unordered_map<unsigned int, std::string>& getNames();
    void loadNames(ZStringView names);
protected:
    int checkVersion();
    void downloadNames();
    void loadNamesFromFile();

    std::unordered_map<unsigned int, std::string> m_names;
    async::TaskHolder<web::WebResponse> m_listener;
};