#pragma once

#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/loader/Event.hpp"
#include "Geode/ui/Button.hpp"
#include "Geode/ui/Label.hpp"
#include "Geode/ui/Layout.hpp"
#include "Geode/ui/NineSlice.hpp"
#include "Geode/utils/ZStringView.hpp"
#include "Geode/utils/function.hpp"
#include "alphalaneous.alphas-ui-pack/nodes/scroll/AdvancedScrollBar.hpp"
#include "alphalaneous.alphas-ui-pack/nodes/scroll/AdvancedScrollLayer.hpp"
#include "alphalaneous.alphas-ui-pack/touch/TouchDelegate.hpp"
#include "module/Module.hpp"
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/Dropdown.hpp>
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/SetupSpawnPopup.hpp>
#include <Geode/modify/SetupRandAdvTriggerPopup.hpp>
#include <Geode/modify/SetupSequenceTriggerPopup.hpp>
#include <Geode/modify/IDNameBrowserPopup.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <algorithm>
#include <alphalaneous.alphas-ui-pack/API.hpp>
#include <initializer_list>
#include <string>
#include <utility>
#include "nodes/Dropdown.hpp"

using namespace alpha::prelude;

namespace tinker::ui {

class GroupLayout : public geode::Layout {
public:
    static GroupLayout* create();

    void apply(cocos2d::CCNode* on) override;
    cocos2d::CCSize getSizeHint(cocos2d::CCNode* on) const override;

    GroupLayout* setHorizontalGap(float gap);
    GroupLayout* setVerticalGap(float gap);
    GroupLayout* setPadding(const geode::Padding& padding);
    GroupLayout* setCenterAligned(bool centerAligned);
    GroupLayout* setSize(const cocos2d::CCSize& size);
    GroupLayout* setMaxWidth(float width);
    GroupLayout* setMinHeight(float height);

    float getHorizontalGap() const;
    float getVerticalGap() const;
    cocos2d::CCSize getSize() const;

    geode::Padding getPadding() const;
    bool isCenterAligned() const;

protected:

    float m_horizontalGap = 0.f;
    float m_verticalGap = 0.f;
    cocos2d::CCSize m_size;
    geode::Padding m_padding{};
    bool m_centerAligned = false;
};

template<class Module, geode::utils::string::ConstexprString SaveKey, class T>
class CycleButton : public geode::Button {
public:
    static CycleButton<Module, SaveKey, T>* create(float width, std::string type, std::vector<std::pair<T, std::string>> values, geode::Function<void(T value)> callback) {
        auto ret = new CycleButton<Module, SaveKey, T>();
        if (ret->init(width, std::move(type), std::move(values), std::move(callback))) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    T getValue() {
        return m_value;
    }

    void setValue(T value) {
        m_value = value;
        int idx = 0;
        for (auto& pair : m_values) {
            if (pair.first == value) {
                m_index = idx;

                m_label->setText(fmt::format("{}: {}", m_type, pair.second));
                m_label->setPosition(getContentSize() / 2.f + cocos2d::CCPoint{0.f, 2.f * m_label->getScale()});
                break;
            }
            idx++;
        }
    }

protected:
    bool init(float width, std::string type, std::vector<std::pair<T, std::string>> values, geode::Function<void(T value)> callback) {
        m_values = std::move(values);
        m_callback = std::move(callback);
        m_type = std::move(type);

        setContentSize({width, 25.f});

        auto bg = geode::NineSlice::create("GJ_button_04.png");
        bg->setContentSize(getContentSize());
        bg->setPosition(getContentSize() / 2.f);

        auto& pair = m_values[0];

        m_label = geode::Label::create(fmt::format("{}: {}", m_type, pair.second), "goldFont.fnt");
        m_label->setLimitLabelWidth(width - 12.f, 0.4f, 0.01f);
        m_label->setPosition(bg->getContentSize() / 2.f + cocos2d::CCPoint{0.f, 2.f * m_label->getScale()});

        bg->addChild(m_label);

        setValue(static_cast<T>(Module::template getSaved<Save::Global, int, SaveKey>(static_cast<int>(pair.first))));

        geode::Button::initWithNode(bg, [this] (auto sender) {
            m_index++;
            if (m_index >= m_values.size()) {
                m_index = 0;
            }

            auto& pair = m_values[m_index];

            m_label->setText(fmt::format("{}: {}", m_type, pair.second));
            m_label->setPosition(getContentSize() / 2.f + cocos2d::CCPoint{0.f, 2.f * m_label->getScale()});

            m_value = pair.first;

            Module::template setSaved<Save::Global, SaveKey>(static_cast<int>(m_value));

            m_callback(m_value);
        });

        return true;
    }

    unsigned int m_index;
    std::vector<std::pair<T, std::string>> m_values{};
    std::string m_type = "";
    geode::Function<void(T value)> m_callback = nullptr;
    geode::Label* m_label = nullptr;
    T m_value{};
};

class GroupView : public cocos2d::CCNode, public TouchDelegate {
public:
    static GroupView* create(const cocos2d::CCSize& size, geode::Function<void()> generateCallback);

    void generate();
    GroupLayout* getGroupLayout();
    void addButton(geode::Button* button);

    void onEnter() override;
    void onExit() override;

    void checkButtons();
    void hoverDisplay(geode::Button* button, bool hovering);

    void setBGScale(float bgScale);

    bool hoverEntered(Hover* hover) override;
	void hoverMoved(Hover* hover) override;
	void hoverExited(Hover* hover) override;

protected:
    bool init(const cocos2d::CCSize& size, geode::Function<void()> generateCallback);

    GroupLayout* m_layout = nullptr;
    AdvancedScrollLayer* m_scrollLayer = nullptr;
    AdvancedScrollBar* m_scrollBar = nullptr;
    geode::Button* m_hoveredButton = nullptr;
    geode::Function<void()> m_generateCallback = nullptr;
    float m_bgScale = 1.f;
};

}

class $module(ImprovedGroupView) {
    struct GroupData {
        std::vector<int> groups{};
        std::vector<int> parentGroups{};
        std::vector<int> tempParentGroups{};
    };

    enum class GroupMode {
        IDs,
        Names
    };

    enum class NameType {
        Group = 1,
        Item = 2,
        Block = 3,
        Timer = 4,
        Effect = 5,
        Control = 6,
        Gradient = 7,
        Enter = 8
    };

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};

class $modify(IGVIDNameBrowserPopup, IDNameBrowserPopup) {
    $registerHooks(ImprovedGroupView)

    class ClosedEvent : public geode::Event<ClosedEvent, bool()> {
    public:
        using Event::Event;
    };

    void onClose(cocos2d::CCObject* sender);
};

class $modify(IGVSetGroupIDLayer, SetGroupIDLayer) {
    $registerHooks(ImprovedGroupView)

    struct Fields {
        tinker::ui::GroupView* m_groupView = nullptr;
        geode::Label* m_groupCountLabel = nullptr;

        tinker::ui::CycleButton<ImprovedGroupView, "id-layer/group-mode", ImprovedGroupView::GroupMode>* m_modeButton = nullptr;
    };

    bool init(GameObject* obj, cocos2d::CCArray* objs);
    void updateGroupIDButtons();

    ImprovedGroupView::GroupData parseObjGroups(GameObject* obj);
};

class $modify(IGVSetupSpawnPopup, SetupSpawnPopup) {
    $registerHooks(ImprovedGroupView)

    struct Fields {
        tinker::ui::GroupView* m_groupView = nullptr;
        geode::Label* m_groupCountLabel = nullptr;

        tinker::ui::CycleButton<ImprovedGroupView, "spawn/group-mode", ImprovedGroupView::GroupMode>* m_modeButton = nullptr;
        tinker::ui::Dropdown* m_typeDropdown = nullptr;

        ImprovedGroupView::NameType m_nameType = ImprovedGroupView::NameType::Group;
    };

    bool init(EffectGameObject* object, cocos2d::CCArray* objects);
    void updateRemapButtons(float dt);
};

class $modify(IGVSetupRandAdvTriggerPopup, SetupRandAdvTriggerPopup) {
    $registerHooks(ImprovedGroupView)

    struct Fields {
        tinker::ui::GroupView* m_groupView = nullptr;
        geode::Label* m_groupCountLabel = nullptr;

        tinker::ui::CycleButton<ImprovedGroupView, "advanced-random/group-mode", ImprovedGroupView::GroupMode>* m_modeButton = nullptr;
    };

    bool init(RandTriggerGameObject* object, cocos2d::CCArray* objects);
    void updateGroupIDButtons();
};

class $modify(IGVSetupSequenceTriggerPopup, SetupSequenceTriggerPopup) {
    $registerHooks(ImprovedGroupView)

    struct Fields {
        tinker::ui::GroupView* m_groupView = nullptr;
        geode::Label* m_groupCountLabel = nullptr;
        geode::Button* m_selectedButton = nullptr;
        geode::Ref<CCMenuItemSpriteExtra> m_fakeButton = nullptr;

        tinker::ui::CycleButton<ImprovedGroupView, "sequence/group-mode", ImprovedGroupView::GroupMode>* m_modeButton = nullptr;
    };

    bool init(SequenceTriggerGameObject* object);
    void onSelect(cocos2d::CCObject* sender);
    void onDeleteSelected(cocos2d::CCObject* sender);
    void updateGroupIDButtons();
};