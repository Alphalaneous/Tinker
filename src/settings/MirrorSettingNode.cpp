#include <Geode/Geode.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include "settings/SettingNode.hpp"

using namespace geode::prelude;

class MirrorSettingNode : public tinker::settings::SettingNode<MirrorSettingNode, "mirror"> {
public:

    bool init(float width, std::shared_ptr<SettingV3> setting) {
        if (!SettingNodeBase::init(width, setting)) return false;
        m_width = width;
        m_titleContainer->removeFromParent();

        setContentSize({width, 30.f});

        return true;
    }

    bool settingWasChanged() override {
        if (!m_settingNode) return false;
        return m_settingNode->hasUncommittedChanges();
    }

    void updateState() override {
        if (!m_settingNode) return;
        m_settingNode->updateState2(nullptr);
    }

    void commit() override {
        if (!m_settingNode) return;
        m_settingNode->commit();
    }

    void resetToDefault() override {
        if (!m_settingNode) return;
        m_settingNode->resetToDefault();
    }

    void setVisible(bool visible) override {
        CCLayerColor::setVisible(visible);
        if (!m_loaded && visible) {


            float scaleOffset = 0.8f;
            float offset = 2.f;

            m_settingNode = m_setting->createNode((m_width / scaleOffset) - offset);
            m_settingNode->setScale(scaleOffset);
            m_settingNode->setAnchorPoint({0.f, 0.f});
            setContentSize({m_width, m_settingNode->getScaledContentHeight()});

            m_settingNode->setPosition({0.f, 0.f});

            addChild(m_settingNode);
            updateState();

            auto parent = static_cast<alpha::ui::ScrollContent*>(getParent());
            if (parent) {
                auto scrollLayer = parent->getScrollLayer();
                float y = scrollLayer->getScrollPoint().y;

                auto heightOld = parent->getContentHeight();
                parent->updateLayout();
                auto heightDelta = heightOld - parent->getContentHeight();

                if ((parent->getContentHeight() - getPositionY()) < y) {
                    m_loaded = true;
                    scrollLayer->offsetScrollY(-heightDelta);
                }
            }
            m_loaded = true;
        }
    };

protected:
    float m_width;
    bool m_loaded;
    SettingNodeV3* m_settingNode;
};