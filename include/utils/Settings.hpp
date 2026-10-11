#pragma once

#include <Geode/loader/Mod.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/utils/string.hpp>

namespace tinker::utils::settings {

template<class S, geode::utils::string::ConstexprString key>
const S& get() {
    static auto setting = geode::Mod::get()->getSettingValue<S>(key.data());
    static auto listener = geode::listenForSettingChanges<S>(key.data(), [] (S value) {
        setting = std::move(value);
    });
    return setting;
}

template<class S, geode::utils::string::ConstexprString key, auto Callback = nullptr>
void onChange() {
    static auto listener = geode::listenForSettingChanges<S>(key.data(), [] (S value) {
        if constexpr (Callback) {
            Callback(value);
        }
    });
    if constexpr (Callback) {
        Callback(getSetting<S, key>());
    }
}

}