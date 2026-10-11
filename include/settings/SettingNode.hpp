#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/ui/Button.hpp>
#include <Geode/ui/Label.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/utils/string.hpp>
#include <memory>
#include "SettingNodeLoader.hpp"

namespace tinker::settings {

class SettingNodeBase : public cocos2d::CCLayerColor {
public:
    bool init(float width, std::shared_ptr<geode::SettingV3> setting);

    virtual bool settingWasChanged();
    virtual void updateState();
    virtual void commit();
    virtual void resetToDefault();
    virtual geode::SettingNodeV3* getNode();

    std::shared_ptr<geode::SettingV3> getSetting();
protected:

    geode::Ref<geode::Label> m_title = nullptr;
    geode::Ref<geode::Button> m_infoButton = nullptr;
    geode::Ref<CCNode> m_titleContainer = nullptr;

    std::shared_ptr<geode::SettingV3> m_setting = nullptr;
};

template <class T, geode::utils::string::ConstexprString Type>
class SettingNode : public SettingNodeBase {
public:
    static inline SettingNodeLoader<T, Type> $apply{};
    static inline auto const $force = &$apply;

    static T* create(float width, std::shared_ptr<geode::SettingV3> setting) {
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