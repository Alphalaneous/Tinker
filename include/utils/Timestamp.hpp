#pragma once

#include <Geode/utils/ZStringView.hpp>
#include <chrono>
#include <string>

using namespace geode::prelude;

class Timestamp {
public:
    Timestamp(ZStringView id);
    ~Timestamp();
    void snapshot(ZStringView label);
private:
    std::string m_id;
    std::chrono::time_point<std::chrono::steady_clock> m_start;
    std::chrono::time_point<std::chrono::steady_clock> m_last;
};