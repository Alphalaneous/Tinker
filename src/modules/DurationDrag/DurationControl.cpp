#include "DurationControl.hpp"
#include <Geode/ui/NineSlice.hpp>
#include "Utils.hpp"

using namespace tinker::ui;

DurationControl* DurationControl::create() {
    auto ret = new DurationControl();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCMenuItemSpriteExtra* DurationControl::createButton(const std::string& text, cocos2d::SEL_MenuHandler method) {

    auto container = CCNode::create();
    container->setContentSize({80, 30});
    container->ignoreAnchorPointForPosition(false);
    container->setAnchorPoint({0.5f, 0.5f});
    container->setID("container"_spr);

    auto bg = NineSlice::create("GJ_square07.png");
    bg->setPosition(container->getContentSize()/2);
    bg->setScale(0.75f);
    bg->setContentSize(container->getContentSize() / bg->getScale());
    bg->setID("background"_spr);
    container->addChild(bg);

    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");

    label->setPosition({container->getContentWidth()/2, container->getContentHeight()/2 + 1.5f});
    label->setScale(0.7f);
    label->setID("value-label"_spr);
    container->addChild(label);

    return CCMenuItemSpriteExtra::create(container, this, method);
}

bool DurationControl::init() {
    setZOrder(10000);

    m_buttonsMenu = CCMenu::create();
    m_buttonsMenu->ignoreAnchorPointForPosition(false);
    m_buttonsMenu->setID("buttons-menu"_spr);
    auto layout = RowLayout::create();
    layout->ignoreInvisibleChildren(true);

    m_buttonsMenu->setLayout(layout);
    m_buttonsMenu->setContentSize({120, 10});
    m_buttonsMenu->setScale(0.5f);

    m_alignAllButton = createButton("Align", menu_selector(DurationControl::onAlign));
    m_alignAllButton->setVisible(false);
    m_alignAllButton->setID("align-all-button"_spr);

    m_buttonsMenu->addChild(m_alignAllButton);
    m_buttonsMenu->updateLayout();

    addChild(m_buttonsMenu);

    return true;
}

EffectGameObject* DurationControl::getEndObject() {
    std::vector<EffectGameObject*> objects;
    objects.reserve(m_objects.size());

    std::transform(m_objects.begin(), m_objects.end(), std::back_inserter(objects),
                    [](const auto& pair) { return pair.first; });

    CCPoint refStart = objects[0]->getPosition();
    CCPoint refEnd   = objects[0]->m_endPosition;
    CCPoint refDir   = refEnd - refStart;
    float refLen = std::sqrt(refDir.x * refDir.x + refDir.y * refDir.y);
    if (refLen == 0.f) refLen = 0.00001f;

    CCPoint unitRefDir = { refDir.x / refLen, refDir.y / refLen };

    return tinker::utils::duration_drag::getFurthestEndObject(objects, unitRefDir);
}

void DurationControl::onAlign(CCObject* obj) {
    createQuickPopup("Align All?", "Align all to the furthest duration?", "Cancel", "Yes", [&](FLAlertLayer*, bool btn){
        if (btn) {

            auto endObj = getEndObject();
            auto endPos = tinker::utils::duration_drag::getEndPos(endObj);

            auto dgl = LevelEditorLayer::get()->m_drawGridLayer;

            const auto* settings = dgl->m_editorLayer->m_levelSettings;
            const int startSpeed = static_cast<int>(settings->m_startSpeed);

            const float endTime = LevelTools::timeForPos(
                endPos,
                dgl->m_speedObjects,
                startSpeed,
                endObj->m_ordValue,
                endObj->m_channelValue,
                false,
                dgl->m_editorLayer->m_isPlatformer,
                true,
                false,
                false
            );

            for (auto& [k, v] : m_objects) {

                auto start = k->getPosition();
				auto end = k->m_endPosition;

				if (end == CCPointZero) end = start;

				bool isLesser = end.x < start.x;

				if (!k->m_isSpawnTriggered && !isLesser) {
					start.x = std::max(start.x, 0.f);
					end.x = std::max(end.x, 0.f);
				}

                const float objStartTime = LevelTools::timeForPos(
                    start,
                    dgl->m_speedObjects,
                    startSpeed,
                    k->m_ordValue,
                    k->m_channelValue,
                    false,
                    dgl->m_editorLayer->m_isPlatformer,
                    true,
                    false,
                    false
                );

                auto newTime = std::abs(endTime - objStartTime);

                if (k->m_objectID == 1006) {
                    k->m_holdDuration = newTime - k->m_fadeInDuration - k->m_fadeOutDuration;
                }
                else {
                    k->m_duration = newTime;
                }
                k->m_endPosition = tinker::utils::duration_drag::getEndPos(k);
            }
        }
    }, true);
}

void DurationControl::hideAllButtons() {
    m_alignAllButton->setVisible(false);
    m_buttonsMenu->updateLayout();
}

void DurationControl::showMultiButtons() {
    m_alignAllButton->setVisible(true);
    m_buttonsMenu->updateLayout();
}

void DurationControl::showDurationButtons() {
    m_alignAllButton->setVisible(false);
    m_buttonsMenu->updateLayout();
}

void DurationControl::addObject(EffectGameObject* object) {
    DurationSlider* slider = DurationSlider::create(object);
    slider->setID("duration-slider"_spr);
    addChild(slider);
    m_objects[object] = slider;
}

void DurationControl::setMenuPosition(const CCPoint& point, const CCPoint& anchor) {
    m_buttonsMenu->setPosition(point);
    m_buttonsMenu->setAnchorPoint(anchor);
}

void DurationControl::updateObjects(EditorUI* editorUI) {
    if (!editorUI || m_lockUpdateObjects) return;

    if (editorUI->m_selectedObjects->count() > 100) {
        for (auto& [k, v] : m_objects) {
            v->removeFromParent();
        }
        if (m_multiSlider) {
            m_multiSlider->removeFromParent();
        }

        hideAllButtons();

        m_objects.clear();
        return;
    }

    m_lockUpdateObjects = true;
    runAction(CallFuncExt::create([this, editorUI] {
        m_lockUpdateObjects = false;

        for (auto& [k, v] : m_objects) {
            v->removeFromParent();
        }
        if (m_multiSlider) {
            m_multiSlider->removeFromParent();
        }

        hideAllButtons();

        m_objects.clear();

        if (auto object = editorUI->m_selectedObject) {
            if (object->m_dontIgnoreDuration && object->m_objectID != 3602) {
                addObject(static_cast<EffectGameObject*>(object));
            }
        }

        for (auto object : CCArrayExt<GameObject*>(editorUI->m_selectedObjects)) {
            if (object->m_dontIgnoreDuration && object->m_objectID != 3602) {
                addObject(static_cast<EffectGameObject*>(object));
            }
        }

        if (m_objects.size() > 1) {

            bool first = true;
            int refChannel;

            for (auto& [k, v] : m_objects) {
                if (first) {
                    refChannel = k->m_channelValue;
                    first = false;
                } else if (k->m_channelValue != refChannel) {
                    return;
                }
            }

            auto endObj = getEndObject();

            m_multiSlider = DurationThumb::create(this, endObj, ThumbType::Multi);
            m_multiSlider->setID("multi-slider"_spr);
            addChild(m_multiSlider);

            showMultiButtons();
        }
    }));
}