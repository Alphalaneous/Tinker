#include "utils/String.hpp"
#include <Geode/utils/general.hpp>
#include <Geode/utils/string.hpp>
#include <string_view>
#include <utility>
#include <vector>

using namespace geode::prelude;

namespace tinker::utils::string {

std::pair<std::string, std::string> splitIntoPair(std::string_view str) {
    auto split = geode::utils::string::split(str, ":");
    auto& key = split[0];
    if (split.size() < 2) {
        return {key, ""};
    }
    auto value = str.substr(key.size() + 1);

    return std::pair<std::string, std::string>{key, value};
}

std::string capitalize(std::string input) {
    bool newWord = true;

    for (char& c : input) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            newWord = true;
            continue;
        } 
        if (newWord) {
            c = std::toupper(static_cast<unsigned char>(c));
            newWord = false;
        }
    }

    return input;
}

std::vector<std::string> split(std::string_view str, std::string_view delimiter, int limit) {
    std::vector<std::string> result{};
    size_t start = 0;
    size_t end = str.find(delimiter);
    int count = 0;

    while (end != std::string::npos && (count < limit - 1 || limit == -1)) {
        result.emplace_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
        count++;
    }

    result.emplace_back(str.substr(start));
    return result;
}

std::string floatToString(float num, int precision) {
    double scale = std::pow(10.0, precision);
    auto rounded = std::round(num * scale) / scale;

    return geode::utils::numToString(rounded);
}

}