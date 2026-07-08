#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/SliderNode.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class ScaleSlider : public SliderNode {
public:
    using ScaleSliderCallback = geode::Function<void(ScaleSlider* sender, float value)>;

    static ScaleSlider* create(ScaleSliderCallback callback, GJScaleControl* control);

    void updateSnap(float snap);
    void setValue(float value, bool skipCallback = false);
    void setPercent(float value, bool skipCallback = false);

protected:
    bool init(ScaleSliderCallback callback, GJScaleControl* control);
    void updateExtendedGroove();

    GJScaleControl* m_scaleControl;
    
    float m_snap = 4.f;
    bool m_skipCallback;
    CCNode* m_snapPointContainer;
    NineSlice* m_extendedGroove;
};

}