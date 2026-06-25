#pragma once

#include "utils/Singleton.hpp"
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class ObjectNames : public Singleton<ObjectNames> {
public:
    void checkNames();
    Result<std::string_view> getName(unsigned int id);
    const std::unordered_map<unsigned int, std::string>& getNames();
protected:
    int checkVersion();
    void downloadNames();
    void loadNames(std::string_view names);
    void loadNamesFromFile();

    std::unordered_map<unsigned int, std::string> m_names;
    async::TaskHolder<web::WebResponse> m_listener;
};