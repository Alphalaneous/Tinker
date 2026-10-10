#pragma once

#include <Geode/loader/Loader.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/utils/string.hpp>
#include <Geode/utils/ZStringView.hpp>

namespace tinker::utils::mod {

bool shouldLoadTinker();

bool modWillBeLoaded(geode::ZStringView ID);

template<geode::utils::string::ConstexprString ID>
inline geode::Mod* getMod() {
    static auto mod = geode::Loader::get()->getLoadedMod(ID.data());
    return mod;
}

template<geode::utils::string::ConstexprString ID>
inline geode::Mod* getInstalledMod() {
    static auto mod = geode::Loader::get()->getInstalledMod(ID.data());
    return mod;
}

}