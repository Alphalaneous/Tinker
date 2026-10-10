#pragma once

#include <Geode/utils/string.hpp>

namespace tinker::utils::string {

template <geode::utils::string::ConstexprString A, geode::utils::string::ConstexprString B>
consteval auto concat2() {
    geode::utils::string::ConstexprString<> out{};
    for (size_t i = 0; i < A.size(); ++i) {
        out.push(A.data()[i]);
    }
    for (size_t i = 0; i < B.size(); ++i) {
        out.push(B.data()[i]);
    }
    return out;
}

template <geode::utils::string::ConstexprString First, geode::utils::string::ConstexprString... Rest>
consteval auto concat() {
    if constexpr (sizeof...(Rest) == 0) {
        return First;
    }
    else {
        return concat2<First, concat<Rest...>()>();
    }
}

template <geode::utils::string::ConstexprString a, geode::utils::string::ConstexprString b>
constexpr bool equals() {
    if (a.size() != b.size()) {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (a.data()[i] != b.data()[i]) {
            return false;
        }
    }
    return true;
}

std::pair<std::string, std::string> splitIntoPair(std::string_view str);

std::string capitalize(std::string input);

std::vector<std::string> split(std::string_view str, std::string_view delimiter, int limit = -1);

std::string floatToString(float num, int precision);

}