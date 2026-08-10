#pragma once

#include <Geode/Geode.hpp>
#include "ObjectList.hpp"

using namespace geode::prelude;

namespace tinker::ui {
    class ObjectPopup : public geode::Popup {
    public:
        static ObjectPopup* create(LevelEditorLayer* levelEditorLayer);
    protected:
        bool init(LevelEditorLayer* levelEditorLayer);
        void generateList(const SortOptions& sortOptions);
        void onSort(CCObject* sender);
        void onFilter(CCObject* sender);
        void onShowHidden(CCObject* sender);
        void createFilterToggler(const std::string& spr, float scale, int tag, ZStringView id);
        CCMenuItemToggler* createToggler(const std::string& spr, cocos2d::SEL_MenuHandler selector, bool alt, float scale);

        std::vector<Ref<CCMenuItemToggler>> m_togglers;
        SortOptions m_sortOptions;
        LevelEditorLayer* m_editorLayer;
        CCMenu* m_sortButtons;
        CCLabelBMFont* m_uniqueLabel;
        ObjectList* m_objectList;
    };
}
