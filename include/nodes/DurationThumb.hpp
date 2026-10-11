#pragma once


#include <Geode/cocos/actions/CCAction.h>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/cocos/touch_dispatcher/CCTouch.h>
#include <Geode/cocos/touch_dispatcher/CCTouchDelegateProtocol.h>
#include <Geode/ui/Label.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <unordered_map>

namespace tinker::ui {

class DurationControl;

enum class ThumbType {
	Default,
	FadeIn,
	FadeOut,
	Multi
};

class DurationThumb : public cocos2d::CCNodeRGBA, public cocos2d::CCTouchDelegate {
public:
	static DurationThumb* create(CCNode* parent, EffectGameObject* object, ThumbType thumbType = ThumbType::Default);

	bool init(CCNode* parent, EffectGameObject* object, ThumbType thumbType);
	void resetText();
	void hideText(float dt);
	void showText(float dt);
	void positionPulseSlider();
	void update(float dt) override;
	void checkFreeMoving();
	void select(bool select);
	void checkForNewLongest();

	bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
	void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
	void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
	void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;

	void registerWithTouchDispatcher();
	void onEnter() override;
	void onExit() override;

	ThumbType m_thumbType = ThumbType::Default;

	float m_labelY = 0.f;
	float m_labelPosMultiplier = 1.f;
	cocos2d::CCNodeRGBA* m_thumbContainer = nullptr;
	cocos2d::CCSprite* m_thumbSpr = nullptr;
	cocos2d::CCSprite* m_thumbSelectedSpr = nullptr;
	CCNode* m_parent = nullptr;
	DurationControl* m_control = nullptr;
	EffectGameObject* m_object = nullptr;
	geode::Label* m_durationLabel = nullptr;
	geode::Ref<cocos2d::CCAction> m_fadeInAction = nullptr;
	geode::Ref<cocos2d::CCAction> m_fadeOutAction = nullptr;

	cocos2d::CCPoint m_startingEndPos{};
	cocos2d::CCPoint m_start{};
	cocos2d::CCPoint m_offset{};

	bool m_disabled = false;
	bool m_dragging = false;
	bool m_overlapping = false;

	float m_startingDuration = 0.f;
	float m_startingHoldDuration = 0.f;
	std::unordered_map<EffectGameObject*, float> m_selectedStartingDurations{};
};

}
