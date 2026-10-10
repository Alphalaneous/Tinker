#pragma once

#include <Geode/binding/EditorPauseLayer.hpp>

namespace tinker::utils {

// yoinked straight from from BetterEdit
struct FakeEditorPauseLayer final {
    char m_alloc[sizeof(EditorPauseLayer)];
    EditorPauseLayer* operator->() {
        return reinterpret_cast<EditorPauseLayer*>(&m_alloc);
    }
};

static FakeEditorPauseLayer fakeEditorPauseLayer() {
    auto epl = FakeEditorPauseLayer();
    epl->m_editorLayer = LevelEditorLayer::get();
    return epl;
}

}