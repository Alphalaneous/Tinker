#include "utils/Mod.hpp"
#include <Geode/loader/Loader.hpp>

using namespace geode::prelude;

namespace tinker::utils::mod {

bool shouldLoadTinker() {
    return true;
}

bool modWillBeLoaded(geode::ZStringView ID) {
    auto mod = geode::Loader::get()->getInstalledMod(ID);
    if (mod && mod->isOrWillBeEnabled()) return true;
    return false;
}

}