#pragma once

#include <Geode/ui/Button.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/Geode.hpp>
#include "SettingNodeLoader.hpp"

using namespace geode::prelude;

namespace tinker::settings {

class SettingNodeBase : public CCNode {
public:
    bool init(float width, std::shared_ptr<SettingV3> setting);

    virtual bool settingWasChanged();
protected:

    Ref<geode::NineSlice> m_background;
    Ref<CCLabelBMFont> m_title;
    Ref<geode::Button> m_infoButton;
    Ref<CCNode> m_titleContainer;

    std::shared_ptr<SettingV3> m_setting;
};

template <class T, geode::utils::string::ConstexprString Type>
class SettingNode : public SettingNodeBase {
public:
    static inline SettingNodeLoader<T, Type> $apply;
    static inline auto const $force = &$apply;

    static T* create(float width, std::shared_ptr<SettingV3> setting) {
        auto ret = new T();
        if (ret->init(width, setting)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
protected:

};

}