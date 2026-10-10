#pragma once

#include "utils/next-free/NextFreeProvider.hpp"
#include <Geode/binding/GameObject.hpp>
#include <Geode/utils/cocos.hpp>
#include <unordered_set>

class EditorLayerSource : public NextFreeSource {
public:
    virtual std::unordered_set<int> getUsedIDs() override {
        std::unordered_set<int> used;
        for (auto obj : CCArrayExt<GameObject*>(m_editorLayer->m_objects)) {
            used.insert(obj->m_editorLayer);
            used.insert(obj->m_editorLayer2);
        }
        return used;
    }
    
    virtual int getMin() override {
        return 0;
    }

    virtual int getMax() override {
        return 9999;
    }

    virtual int getOffset() override {
        // do nothing
        return -1;
    }

    virtual void setOffset(int offset) override {
        // do nothing
    }
};