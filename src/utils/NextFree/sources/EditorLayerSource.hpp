#pragma once

#include "utils/NextFree/NextFreeProvider.hpp"

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
        return std::numeric_limits<short>().max();
    }

    virtual int getOffset() override {
        // do nothing
        return -1;
    }

    virtual void setOffset(int offset) override {
        // do nothing
    }
};