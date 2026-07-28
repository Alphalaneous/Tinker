#pragma once

#include "alphalaneous.level-storage-api/include/LevelStorageAPI.hpp"
#include "utils/NextFree/NextFreeProvider.hpp"

class ColorSource : public NextFreeSource {
public:
    virtual std::unordered_set<int> getUsedIDs() override {
        std::unordered_set<int> used;
        for (auto obj : m_editorLayer->m_objects->asExt<GameObject>()) {
            if (obj->m_detailColor) {
                used.insert(obj->m_detailColor->m_colorID);
            }
            if (obj->m_baseColor) {
                used.insert(obj->m_baseColor->m_colorID);
            }
        }
        for (auto color : m_editorLayer->m_effectManager->getAllColorActions()->asExt<ColorAction>()) {
            if (color->m_color != ccColor3B{255, 255, 255} || color->m_copyID != 0 || color->m_blending) {
                used.insert(color->m_colorID);
            }
        }
        return used;
    }
    
    virtual int getMin() override {
        return 1;
    }

    virtual int getMax() override {
        return 999;
    }

    virtual int getOffset() override {
        auto saved = alpha::level_storage::getSaveContainer(m_editorLayer, Mod::get());
        auto res = saved.get("next-free-offset/color");
        if (!res) return getMin();

        auto valueRes = res.unwrap().asInt();
        if (!valueRes) return getMin();
        
        return valueRes.unwrap();
    }

    virtual void setOffset(int offset) override {
        if (!m_editorLayer) return;
        alpha::level_storage::setSavedValue(m_editorLayer, "next-free-offset/color", offset);
    }
};