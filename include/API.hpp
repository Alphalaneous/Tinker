#pragma once

#include <Geode/loader/Event.hpp>
#include <Geode/loader/EventExport.hpp>

#define MY_MOD_ID "alphalaneous.tinker"

namespace tinker::api {

    inline bool isLoaded() {
        return geode::Loader::get()->getLoadedMod("alphalaneous.tinker");
    }

    template <typename F>
    void waitForTinker(F&& callback) {
        if (isLoaded()) {
            callback();
            return;
        } 
        auto mod = geode::Loader::get()->getInstalledMod("alphalaneous.tinker");
        if (!mod) return;

        geode::ModStateEvent(geode::ModEventType::Loaded, mod).listen(
            [callback = std::forward<F>(callback)]() {
                callback();
            }
        ).leak();
    }
};