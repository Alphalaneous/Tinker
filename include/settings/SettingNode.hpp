#pragma once

#include <Geode/Geode.hpp>
#include "SettingNodeLoader.hpp"

using namespace geode::prelude;

namespace tinker::settings {

class SettingNodeBase : public CCLayerColor {
public:
    bool init(float width, std::shared_ptr<SettingV3> setting);

    virtual bool settingWasChanged();
    virtual void updateState();
    virtual void commit();
    virtual void resetToDefault();
    virtual SettingNodeV3* getNode();

    std::shared_ptr<SettingV3> getSetting();
protected:

    Ref<geode::Label> m_title;
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