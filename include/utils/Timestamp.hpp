#pragma once

#include <Geode/utils/ZStringView.hpp>
#include <chrono>
#include <string>

class Timestamp {
public:
    Timestamp(geode::ZStringView id);
    ~Timestamp();
    void snapshot(geode::ZStringView label);
private:
    std::string m_id = "";
    std::chrono::time_point<std::chrono::steady_clock> m_start{};
    std::chrono::time_point<std::chrono::steady_clock> m_last{};
};