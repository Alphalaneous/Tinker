#include "utils/next-free/NextFreeProvider.hpp"
#include "utils/DestroyListener.hpp"

void NextFreeProvider::setEditorLayer(LevelEditorLayer* editorLayer) {
    m_activeEditorLayer = editorLayer;

    editorLayer->setUserObject("on-destroy"_spr, DestroyListener::create([this] {
        m_activeEditorLayer = nullptr;
    }));
}