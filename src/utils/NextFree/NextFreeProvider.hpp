#pragma once

#include "utils/Singleton.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class NextFreeSource {
public:
    void setEditor(LevelEditorLayer* editor) {
        m_editorLayer = editor;
    }

    virtual std::unordered_set<int> getUsedIDs() = 0;
    virtual int getMin() = 0;
    virtual int getMax() = 0;
    virtual int getOffset() = 0;
    virtual void setOffset(int offset) = 0;

protected:
    LevelEditorLayer* m_editorLayer;
};

class NextFreeProvider : public Singleton<NextFreeProvider> {
public:
    void setEditorLayer(LevelEditorLayer* editorLayer);

    template<class T>
    requires std::derived_from<T, NextFreeSource>
    Result<int> nextFree() {
        if (!m_activeEditorLayer) {
            return Err("Not in editor layer");
        }
        auto source = T();
        source.setEditor(m_activeEditorLayer);
        auto used = source.getUsedIDs();

        auto min = std::max(source.getOffset(), source.getMin());

        for (int i = min; i <= source.getMax(); i++) {
            if (!used.contains(i)) {
                return Ok(i);
            }
        }

        return Err("No free IDs");
    }

protected:
    LevelEditorLayer* m_activeEditorLayer;
};