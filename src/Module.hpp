#pragma once

#include <Geode/Geode.hpp>
#include "ModuleCore.hpp"
#include "ModuleBase.hpp"
#include "Events.hpp"

using namespace geode::prelude;

template <class T, geode::utils::string::ConstexprString Name>
using EditorModule = ModuleCore<T, EditorModuleBase, Name>;

template <class T, geode::utils::string::ConstexprString Name>
using GlobalModule = ModuleCore<T, GlobalModuleBase, Name>;

#define REGISTERMODULEHOOKSDECLARE(name, base, alt) \
static void onModify(auto& self) { \
    for (const auto& [k, v] : self.m_hooks) { \
        ModuleRegistry<base>::get()->m_hooks[name::getName()].push_back(v); \
        v->setAutoEnable(false); \
    } \
    alt; \
}

#define REGISTERMODULEHOOKS1(name, base) REGISTERMODULEHOOKSDECLARE(name, base, do_nothing(false))
#define REGISTERMODULEHOOKS2(name, alt, base) REGISTERMODULEHOOKSDECLARE(name, base, Self::_onModify(self))

#define $registerEditorHooks(...) \
    GEODE_INVOKE(GEODE_CONCAT(REGISTERMODULEHOOKS, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__, EditorModuleBase)

#define $registerGlobalHooks(...) \
    GEODE_INVOKE(GEODE_CONCAT(REGISTERMODULEHOOKS, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__, GlobalModuleBase)

#define $editorModule(name) \
GEODE_CONCAT(GEODE_CONCAT(name, __LINE__), Dummy); \
struct name : public EditorModule<name, geode::utils::string::ConstexprString(#name)>

#define $globalModule(name) \
GEODE_CONCAT(GEODE_CONCAT(name, __LINE__), Dummy); \
struct name : public GlobalModule<name, geode::utils::string::ConstexprString(#name)>
