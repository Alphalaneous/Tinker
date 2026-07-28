#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tinker::ui {
	class DurationControl;

	enum class ThumbType {
		Default,
		FadeIn,
		FadeOut,
		Multi
	};

	class DurationThumb : public CCNodeRGBA, public CCTouchDelegate {
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

		bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
		void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
		void ccTouchEnded(CCTouch* touch, CCEvent* event) override;
		void ccTouchCancelled(CCTouch* touch, CCEvent* event) override;

		void registerWithTouchDispatcher();
		void onEnter() override;
		void onExit() override;

		ThumbType m_thumbType = ThumbType::Default;

		float m_labelY = 0.f;
		float m_labelPosMultiplier = 1.f;
		CCNodeRGBA* m_thumbContainer;
		CCSprite* m_thumbSpr;
		CCSprite* m_thumbSelectedSpr;
		CCNode* m_parent;
		DurationControl* m_control;
		EffectGameObject* m_object;
		CCLabelBMFont* m_durationLabel;
		Ref<CCAction> m_fadeInAction;
		Ref<CCAction> m_fadeOutAction;

		CCPoint m_startingEndPos;
		CCPoint m_start;
		CCPoint m_offset;

		bool m_disabled;
		bool m_dragging;
		bool m_overlapping;

		float m_startingDuration = 0.f;
		float m_startingHoldDuration = 0.f;
		std::unordered_map<EffectGameObject*, float> m_selectedStartingDurations;
	};
}
