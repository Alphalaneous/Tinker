#pragma once

#include <Geode/cocos/cocoa/CCObject.h>
#include <Geode/cocos/menu_nodes/CCMenu.h>
#include <Geode/ui/Label.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <vector>
#include "ObjectList.hpp"

namespace tinker::ui {

class ObjectPopup : public geode::Popup {
public:
    static ObjectPopup* create(LevelEditorLayer* levelEditorLayer);
protected:
    bool init(LevelEditorLayer* levelEditorLayer);
    void generateList(const SortOptions& sortOptions);
    void onSort(cocos2d::CCObject* sender);
    void onFilter(cocos2d::CCObject* sender);
    void onShowHidden(cocos2d::CCObject* sender);
    void createFilterToggler(const std::string& spr, float scale, int tag, geode::ZStringView id);
    CCMenuItemToggler* createToggler(const std::string& spr, cocos2d::SEL_MenuHandler selector, bool alt, float scale);

    std::vector<geode::Ref<CCMenuItemToggler>> m_togglers{};
    SortOptions m_sortOptions{};
    LevelEditorLayer* m_editorLayer = nullptr;
    cocos2d::CCMenu* m_sortButtons = nullptr;
    geode::Label* m_uniqueLabel = nullptr;
    ObjectList* m_objectList = nullptr;
};

}
