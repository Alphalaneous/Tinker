#include "modules/Gizmos/TestGizmo.hpp"
#include "Geode/ui/NineSlice.hpp"

CCNode* TestGizmo::setup() {

    auto nineSlice = geode::NineSlice::create("simple-popup-square.png"_spr);
    nineSlice->setOpacity(160);
    nineSlice->setContentSize({50.f, 50.f});

    m_object->setCanEditObject(true);

    return nineSlice;
}

void TestGizmo::onEditObject() {
    log::info("editing test");
}

void TestGizmo::loadData(std::optional<const matjson::Value> data) {
    if (data) {
        log::info("data: {}", data.value().dump());
    }
    else {
        log::info("no data!");
    }
}

matjson::Value TestGizmo::saveData() {
    return "test";
}