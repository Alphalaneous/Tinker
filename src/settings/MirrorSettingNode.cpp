#include <Geode/Geode.hpp>
#include "Geode/loader/Log.hpp"
#include "SettingNode.hpp"

using namespace geode::prelude;

class MirrorSettingNode : public tinker::settings::SettingNode<MirrorSettingNode, "mirror"> {
public:

    bool init(float width, std::shared_ptr<SettingV3> setting) {
        if (!SettingNodeBase::init(width, setting)) return false;

        m_titleContainer->removeFromParent();

        m_settingNode = setting->createNode(width);
        setContentSize(m_settingNode->getContentSize());
        m_background->setContentSize(getContentSize());
        m_background->setPosition(getContentSize() / 2.f);

        m_settingNode->setPosition(getContentSize() / 2.f);

        addChild(m_settingNode);


        return true;
    }

    bool settingWasChanged() {
        bool uncommitted = m_settingNode->hasUncommittedChanges();
        log::info("{} | uncommited: {}", m_setting->getKey(), uncommitted);
        return uncommitted;
    }

protected:

    SettingNodeV3* m_settingNode;
};