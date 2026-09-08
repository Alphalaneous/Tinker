#pragma once

#include "modules/Gizmos/Gizmos.hpp"

class TestGizmo : public GizmoInterface {
    CCNode* setup() override;
    void onEditObject() override;
    void loadData(std::optional<const matjson::Value> data) override;
    matjson::Value saveData() override;
};