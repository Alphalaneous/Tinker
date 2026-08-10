#pragma once

#include <Geode/Geode.hpp>
#include "module/ModuleCore.hpp"
#include "module/ModuleBase.hpp"

using namespace geode::prelude;

#define REGISTERMODULEHOOKSDECLARE(name, alt) \
static void onModify(auto& self) { \
    for (const auto& [k, v] : self.m_hooks) { \
        ModuleRegistry::get()->getData(name::getName()).getHooks().push_back(v); \
        v->setAutoEnable(false); \
    } \
    alt; \
}

#define REGISTERMODULEHOOKS1(name) REGISTERMODULEHOOKSDECLARE(name, do_nothing(false))
#define REGISTERMODULEHOOKS2(name, alt) REGISTERMODULEHOOKSDECLARE(name, Self::_onModify(self))

#define $registerHooks(...) \
    GEODE_INVOKE(GEODE_CONCAT(REGISTERMODULEHOOKS, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)

#define REGISTERMODULEDECLARE(name, global) \
    GEODE_CONCAT(GEODE_CONCAT(name, __LINE__), Dummy); \
    struct name : public ModuleCore<name, geode::utils::string::ConstexprString(#name), global>

#define REGISTERMODULE1(name) REGISTERMODULEDECLARE(name, false)
#define REGISTERMODULE2(name, global) REGISTERMODULEDECLARE(name, global)

#define $module(...) \
    GEODE_INVOKE(GEODE_CONCAT(REGISTERMODULE, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)