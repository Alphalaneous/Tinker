#include "modules/AdvancedGridControls.hpp"
#include "MainHooks.hpp"
#include <alphalaneous.good_grid/DrawLayers.hpp>

using namespace geode::prelude;

namespace tinker::ui {

SizeContainer* SizeContainer::create(ZStringView label, float value, float min, float max, float hardMin, float hardMax, bool compact, geode::Function<void(float size)> sizeCallback) {
    auto ret = new SizeContainer();
    if (ret->init(label, value, min, max, hardMin, hardMax, compact, std::move(sizeCallback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SizeContainer::init(ZStringView label, float value, float min, float max, float hardMin, float hardMax, bool compact, geode::Function<void(float size)> sizeCallback) {
    m_compact = compact;
    m_sizeCallback = std::move(sizeCallback);
    m_value = value;
    m_min = min;
    m_max = max;

    setAnchorPoint({0.5f, 0.5f});
    setContentSize({compact ? 130.f : 160.f, 60.f});
    setScale(0.9f);

    auto sizeContainerMenu = CCMenu::create();
    sizeContainerMenu->setContentSize(getContentSize());
    sizeContainerMenu->ignoreAnchorPointForPosition(false);
    sizeContainerMenu->setAnchorPoint({0.5f, 0.5f});
    sizeContainerMenu->setPosition(getContentSize() / 2.f);

    addChild(sizeContainerMenu);

    auto sizeLabel = geode::Label::create(label, "goldFont.fnt");
    sizeLabel->setScale(0.45f);
    sizeLabel->setAnchorPoint({0.5f, 1.f});
    sizeLabel->setPosition({getContentWidth() / 2.f, getContentHeight()});

    addChild(sizeLabel);

    m_sizeInput = geode::TextInput::create(60.f, "Size");
    m_sizeInput->setCallback([this, hardMin, hardMax] (std::string const& str) {
        if (!m_sizeSlider) return;
        auto size = numFromString<float>(str).unwrapOrDefault();
        size = std::clamp(size, hardMin, hardMax);

        m_skipSliderCallback = true;
        m_sizeSlider->setValue(size);
        m_skipSliderCallback = false;

        m_sizeCallback(size);
        m_value = size;

        AdvancedGridControls::get()->updateGrid();
    });
    m_sizeInput->setScale(0.7f);
    m_sizeInput->setPosition(getContentSize() / 2.f + CCPoint{0.f, 0.5f});
    m_sizeInput->setString(fmt::format("{:.1f}", value));
    if (hardMin < 0) {
        m_sizeInput->setFilter("0123456789.-");
    }
    else {
        m_sizeInput->setFilter("0123456789.");
    }

    addChild(m_sizeInput);

    m_sizeSlider = SliderNode::create([this, min, max] (SliderNode* sender, float value) {
        if (m_skipSliderCallback) return;

        if (value < min || value > max) {
            m_sizeSlider->setValue(std::clamp(value, min, max));
            return;
        }

        m_sizeCallback(value);
        m_value = value;

        AdvancedGridControls::get()->updateGrid();
        m_sizeInput->setString(fmt::format("{:.1f}", value));
    });
    m_sizeSlider->setSliderBypass(true);
    m_sizeSlider->setPosition(getContentSize() / 2.f - CCPoint{0.f, 22.f});
    m_sizeSlider->setMin(min);
    m_sizeSlider->setMax(max);
    m_sizeSlider->setScale(0.55f);
    m_sizeSlider->setValue(value);
    m_sizeSlider->setSnapStep(0.5f);
    m_sizeSlider->getBar()->setVisible(false);

    addChild(m_sizeSlider);

    auto largeArrowLeft = CCMenuItemExt::createSpriteExtraWithFrameName("edit_leftBtn2_001.png", 0.8f, [this] (auto sender) {
        m_value = (std::ceil(m_value / 15.f) - 1) * 15.f;
        updateValue();
    });
    largeArrowLeft->setPosition({largeArrowLeft->getScaledContentWidth() / 2.f, getContentHeight() / 2.f});

    sizeContainerMenu->addChild(largeArrowLeft);

    auto largeArrowRight = CCMenuItemExt::createSpriteExtraWithFrameName("edit_rightBtn2_001.png", 0.8f, [this] (auto sender) {
        m_value = (std::floor(m_value / 15.f) + 1) * 15.f;
        updateValue();
    });
    largeArrowRight->setPosition({getContentWidth() - largeArrowRight->getScaledContentWidth() / 2.f, getContentHeight() / 2.f});

    sizeContainerMenu->addChild(largeArrowRight);

    float distance = compact ? 8.f : 15.f;

    auto smallArrowLeft = CCMenuItemExt::createSpriteExtraWithFrameName("edit_leftBtn_001.png", 0.8f, [this] (auto sender) {
        m_value = (std::ceil(m_value / 7.5f) - 1) * 7.5f;
        updateValue();
    });
    smallArrowLeft->setPosition({m_sizeInput->boundingBox().getMinX() - smallArrowLeft->getScaledContentWidth() / 2.f - distance, getContentHeight() / 2.f});

    sizeContainerMenu->addChild(smallArrowLeft);

    auto smallArrowRight = CCMenuItemExt::createSpriteExtraWithFrameName("edit_rightBtn_001.png", 0.8f, [this] (auto sender) {
        m_value = (std::floor(m_value / 7.5f) + 1) * 7.5f;
        updateValue();
    });
    smallArrowRight->setPosition({m_sizeInput->boundingBox().getMaxX() + smallArrowRight->getScaledContentWidth() / 2.f + distance, getContentHeight() / 2.f});

    sizeContainerMenu->addChild(smallArrowRight);

    return true;
}

void SizeContainer::updateValue() {
    m_value = std::clamp(m_value, m_min, m_max);

    m_skipSliderCallback = true;
    m_sizeInput->setString(fmt::format("{:.1f}", m_value));
    m_sizeSlider->setValue(m_value);
    m_skipSliderCallback = false;

    m_sizeCallback(m_value);
    AdvancedGridControls::get()->updateGrid();
}

void SizeContainer::setValue(float value) {
    m_value = std::clamp(value, m_min, m_max);

    m_skipSliderCallback = true;
    m_sizeInput->setString(fmt::format("{:.1f}", value));
    m_sizeSlider->setValue(value);
    m_skipSliderCallback = false;
}

void SizeContainer::setEnabled(bool enabled) {
    m_sizeSlider->setReadOnly(!enabled);
}

AdvancedGridSettingsLayer* AdvancedGridSettingsLayer::create() {
    auto ret = new AdvancedGridSettingsLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool AdvancedGridSettingsLayer::init() {
    m_allowDraggingAnywhere = true;
    m_startCentered = true;
    m_resizeMode = 2;
    m_minScale = 0.6f;
    m_maxScale = 1.f;

    auto size = CCSize{300.f, 135.f};

    if (!EditorWindowPopup::initWindow(EditorUI::get(), size.width, size.height, 4, 0)) return false;

    auto control = AdvancedGridControls::get();

    m_contentContainer = CCNode::create();
    m_contentContainer->setAnchorPoint({0.5f, 0.5f});
    m_contentContainer->setPosition(m_mainLayer->getContentSize() / 2.f);
    m_contentContainer->setContentSize(size);

    m_mainLayer->addChild(m_contentContainer);

    m_contentMenu = CCMenu::create();
    m_contentMenu->setAnchorPoint({0.5f, 0.5f});
    m_contentMenu->setPosition(m_mainLayer->getContentSize() / 2.f);
    m_contentMenu->ignoreAnchorPointForPosition(false);
    m_contentMenu->setContentSize(size);

    m_mainLayer->addChild(m_contentMenu);

    m_titleLabel = geode::Label::create("Grid Size", "bigFont.fnt");
    m_titleLabel->setScale(0.5f);
    m_titleLabel->setAnchorPoint({0.5f, 1.f});
    m_titleLabel->setPosition({size.width / 2.f, size.height - 6.f});

    m_contentContainer->addChild(m_titleLabel);

    m_background = geode::NineSlice::create("square02_001-uhd.png");
    m_background->setAnchorPoint({0.5f, 1.f});
    m_background->setPosition({size.width / 2.f, size.height - 35.f});
    m_background->setOpacity(50);
    m_background->setContentSize({size.width - 30.f, 65.f});
    m_background->setScaleMultiplier(0.5f);

    auto bgMenu = CCMenu::create();
    bgMenu->setContentSize(m_background->getContentSize());
    bgMenu->ignoreAnchorPointForPosition(false);
    bgMenu->setAnchorPoint({0.5f, 0.5f});
    bgMenu->setPosition(m_background->getContentSize() / 2.f);

    m_background->addChild(bgMenu);

    m_contentContainer->addChild(m_background);

    auto info = InfoAlertButton::create("Grid Size", "<cg>Enable</c> to override the editor grid with a custom size.\n<cl>Shift+G</c> toggles these controls.\n<cl>Ctrl+G</c> cycles the preset sizes (7.5, 15, 30, 60, 120).\nPress <co>Obj</c> with an object selected to align the grid to it.\n<cb>XY Mode</c> lets you resize each axis of the grid.\n<cb>Scale Objects</c> scales the objects to fit each grid space.", 0.75f);

    info->setPosition(size - info->getScaledContentSize() / 2.f - CCSize{6.f, 2.f});

    m_contentMenu->addChild(info);

    m_clearBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_trashBtn_001.png", 0.5f, [this, control] (auto sender) {
        if (m_page == 0) {
            control->m_gridX = 30.f;
            control->m_gridY = 30.f;

            AdvancedGridControls::setSaved<Save::Level, "size">(30.f);
            AdvancedGridControls::setSaved<Save::Level, "size-x">(30.f);
            AdvancedGridControls::setSaved<Save::Level, "size-y">(30.f);

            m_sizeContainer->setValue(30.f);
            m_sizeXContainer->setValue(30.f);
            m_sizeYContainer->setValue(30.f);
        }
        else if (m_page == 1) {
            control->setSaved<Save::Level, "offset-x">(0.f);
            control->setSaved<Save::Level, "offset-y">(0.f);

            m_offsetXContainer->setValue(0.f);
            m_offsetYContainer->setValue(0.f);
        }

        AdvancedGridControls::get()->updateGrid();
    });

    bgMenu->addChild(m_clearBtn);

    m_enabledLabel = geode::Label::create("Enabled", "bigFont.fnt");
    m_enabledLabel->setScale(0.35f);
    m_enabledLabel->setAnchorPoint({0.f, 1.f});

    m_background->addChild(m_enabledLabel);

    m_enabledCheckbox = CCMenuItemExt::createTogglerWithStandardSprites(0.75f, [control] (CCMenuItemToggler* toggler) {
        AdvancedGridControls::setSaved<Save::Level, "enabled">(!toggler->isOn());
        control->updateGrid();
    });

    auto enabled = AdvancedGridControls::getSaved<Save::Level, bool, "enabled">();
    m_enabledCheckbox->toggle(enabled);

    bgMenu->addChild(m_enabledCheckbox);

    setupSizePage();
    setupOffsetPage();

    m_xyModeCheckbox = CCMenuItemExt::createTogglerWithStandardSprites(0.7f, [this, control] (CCMenuItemToggler* toggler) {
        AdvancedGridControls::setSaved<Save::Level, "separate-xy">(!toggler->isOn());
        
        updateMode();
        control->updateGrid();
    });

    auto xyModeEnabled = AdvancedGridControls::getSaved<Save::Level, bool, "separate-xy">();
    m_xyModeCheckbox->toggle(xyModeEnabled);
    m_xyModeCheckbox->setPosition({26.f, 16.f});

    m_contentMenu->addChild(m_xyModeCheckbox);

    auto xyModeLabel = geode::Label::create("XY Mode", "bigFont.fnt");
    xyModeLabel->setScale(0.4f);
    xyModeLabel->setAnchorPoint({0.f, 0.5f});
    xyModeLabel->setPosition({m_xyModeCheckbox->boundingBox().getMaxX() + 5.f, m_xyModeCheckbox->boundingBox().getMidY() + 0.5f});
    xyModeLabel->validate();

    m_contentContainer->addChild(xyModeLabel);

    m_scaleCheckbox = CCMenuItemExt::createTogglerWithStandardSprites(0.7f, [this, control] (CCMenuItemToggler* toggler) {
        AdvancedGridControls::setSaved<Save::Level, "scale-objects">(!toggler->isOn());

        control->updateGrid();
    });

    auto scaleEnabled = AdvancedGridControls::getSaved<Save::Level, bool, "scale-objects">();
    m_scaleCheckbox->toggle(scaleEnabled);
    m_scaleCheckbox->setPosition({xyModeLabel->boundingBox().getMaxX() + 16.f + m_scaleCheckbox->getScaledContentWidth() / 2.f, 16.f});

    m_contentMenu->addChild(m_scaleCheckbox);

    auto scaleLabel = geode::Label::create("Scale Objects", "bigFont.fnt");
    scaleLabel->setScale(0.4f);
    scaleLabel->setAnchorPoint({0.f, 0.5f});
    scaleLabel->setPosition({m_scaleCheckbox->boundingBox().getMaxX() + 5.f, m_scaleCheckbox->boundingBox().getMidY() + 0.5f});

    m_contentContainer->addChild(scaleLabel);

    auto leftArrow = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_arrow_01_001.png", 0.75f, [this] (auto sender) {
        m_page--;
        if (m_page < 0) {
            m_page = 1;
        }
        updateMode();
    });
    leftArrow->setPosition({-5.f - leftArrow->getScaledContentWidth() / 2.f, m_contentMenu->getContentHeight() / 2.f});

    m_contentMenu->addChild(leftArrow);

    auto rightArrow = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_arrow_01_001.png", 0.75f, [this] (auto sender) {
        m_page++;
        if (m_page > 1) {
            m_page = 0;
        }
        updateMode();
    });
    rightArrow->setPosition({m_contentMenu->getContentWidth() + 5.f + rightArrow->getScaledContentWidth() / 2.f, m_contentMenu->getContentHeight() / 2.f});

    static_cast<CCSprite*>(rightArrow->getNormalImage())->setFlipX(true);

    m_contentMenu->addChild(rightArrow);

    updateMode();

    return true;
}

void AdvancedGridSettingsLayer::updateValues() {
    auto enabled = AdvancedGridControls::getSaved<Save::Level, bool, "enabled">();
    m_enabledCheckbox->toggle(enabled);

    auto xyModeEnabled = AdvancedGridControls::getSaved<Save::Level, bool, "separate-xy">();
    m_xyModeCheckbox->toggle(xyModeEnabled);

    auto scaleEnabled = AdvancedGridControls::getSaved<Save::Level, bool, "scale-objects">();
    m_scaleCheckbox->toggle(scaleEnabled);

    auto size = AdvancedGridControls::getSaved<Save::Level, float, "size">(30.f);
    m_sizeContainer->setValue(size);

    auto sizeX = AdvancedGridControls::getSaved<Save::Level, float, "size-x">(30.f);
    m_sizeXContainer->setValue(sizeX);

    auto sizeY = AdvancedGridControls::getSaved<Save::Level, float, "size-y">(30.f);
    m_sizeYContainer->setValue(sizeY);

    auto offsetX = AdvancedGridControls::getSaved<Save::Level, float, "offset-x">(0.f);
    m_offsetXContainer->setValue(offsetX);

    auto offsetY = AdvancedGridControls::getSaved<Save::Level, float, "offset-y">(0.f);
    m_offsetYContainer->setValue(offsetY);

    updateMode();
}

void AdvancedGridSettingsLayer::onClose(cocos2d::CCObject* sender) {
    EditorWindowPopup::onClose(sender);
}

void AdvancedGridSettingsLayer::onBuildWindowContent() {
    
}

void AdvancedGridSettingsLayer::setupSizePage() {
    m_sizeContainer = SizeContainer::create("Size:", AdvancedGridControls::getSaved<Save::Level, float, "size">(30.f), 7.5f, 120.f, 1.f, 300.f, false, [] (float value) {
        AdvancedGridControls::setSaved<Save::Level, "size">(value);
    });

    m_sizeContainer->setPosition(m_background->getContentWidth() / 2.f + 15.f, m_background->getContentHeight() / 2.f);

    m_background->addChild(m_sizeContainer);

    m_sizeXContainer = SizeContainer::create("Size X:", AdvancedGridControls::getSaved<Save::Level, float, "size-x">(30.f), 7.5f, 120.f, 1.f, 300.f, true, [] (float value) {
        AdvancedGridControls::setSaved<Save::Level, "size-x">(value);
    });

    m_sizeXContainer->setPosition(m_background->getContentWidth() / 2.f - m_sizeXContainer->getScaledContentWidth() / 2.f - 5.f, m_background->getContentHeight() / 2.f);

    m_background->addChild(m_sizeXContainer);

    m_sizeYContainer = SizeContainer::create("Size Y:", AdvancedGridControls::getSaved<Save::Level, float, "size-y">(30.f), 7.5f, 120.f, 1.f, 300.f, true, [] (float value) {
        AdvancedGridControls::setSaved<Save::Level, "size-y">(value);
    });

    m_sizeYContainer->setPosition(m_background->getContentWidth() / 2.f + m_sizeYContainer->getScaledContentWidth() / 2.f + 5.f, m_background->getContentHeight() / 2.f);

    m_background->addChild(m_sizeYContainer);
}

void AdvancedGridSettingsLayer::setupOffsetPage() {
    m_offsetXContainer = SizeContainer::create("Offset X:", AdvancedGridControls::getSaved<Save::Level, float, "offset-x">(), -150.f, 150.f, -150.f, 150.f, true, [] (float value) {
        AdvancedGridControls::setSaved<Save::Level, "offset-x">(value);
    });

    m_offsetXContainer->setPosition(m_background->getContentWidth() / 2.f - m_offsetXContainer->getScaledContentWidth() / 2.f - 5.f, m_background->getContentHeight() / 2.f);

    m_background->addChild(m_offsetXContainer);

    m_offsetYContainer = SizeContainer::create("Offset Y:", AdvancedGridControls::getSaved<Save::Level, float, "offset-y">(), -150.f, 150.f, -150.f, 150.f, true, [] (float value) {
        AdvancedGridControls::setSaved<Save::Level, "offset-y">(value);
    });

    m_offsetYContainer->setPosition(m_background->getContentWidth() / 2.f + m_offsetYContainer->getScaledContentWidth() / 2.f + 5.f, m_background->getContentHeight() / 2.f);

    m_background->addChild(m_offsetYContainer);

    auto spr = ButtonSprite::create("Obj");
    spr->setScale(0.5f);

    m_alignBtn = CCMenuItemExt::createSpriteExtra(spr, [this] (auto sender) {
        auto editor = MainEditorUI::get();
        if (!editor || !editor->m_selectedObject) return;

        auto control = AdvancedGridControls::get();

        AdvancedGridControls::setSaved<Save::Level, "offset-x">(0.f);
        AdvancedGridControls::setSaved<Save::Level, "offset-y">(0.f);

        control->updateGrid();

        auto offsetForKey = ::offsetForKey(editor->m_selectedObject->m_objectID);

        int rot = static_cast<int>(std::round(editor->m_selectedObject->getRotation()));
        if (rot % 90 == 0) {
            switch(rot) {
                case 90: {
                    offsetForKey = CCPoint{offsetForKey.y, offsetForKey.x};
                    break;
                }
                case 180: {
                    offsetForKey = CCPoint{-offsetForKey.x, -offsetForKey.y};
                    break;
                }
                case 270: {
                    offsetForKey = CCPoint{-offsetForKey.y, -offsetForKey.x};
                    break;
                }
            }
        }
        else {
            offsetForKey = CCPoint{0.f, 0.f};
        }

        auto gridSnapped = editor->getGridSnappedPos(editor->m_selectedObject->getPosition() - offsetForKey);
        auto offset = editor->m_selectedObject->getPosition() - offsetForKey - gridSnapped;

        AdvancedGridControls::setSaved<Save::Level, "offset-x">(offset.x);
        AdvancedGridControls::setSaved<Save::Level, "offset-y">(offset.y);

        m_offsetXContainer->setValue(offset.x);
        m_offsetYContainer->setValue(offset.y);

        control->updateGrid();
    });

    m_alignBtn->setPosition({m_contentMenu->getContentWidth() - m_alignBtn->getScaledContentWidth() / 2.f - 12.f, 8.f + m_alignBtn->getScaledContentHeight() / 2.f});

    m_contentMenu->addChild(m_alignBtn);
}

void AdvancedGridSettingsLayer::updateMode() {
    bool separateXY = AdvancedGridControls::getSaved<Save::Level, bool, "separate-xy">();
    
    m_sizeContainer->setVisible(false);
    m_sizeXContainer->setVisible(false);
    m_sizeYContainer->setVisible(false);
    m_enabledLabel->setVisible(false);

    m_sizeContainer->setEnabled(false);
    m_sizeXContainer->setEnabled(false);
    m_sizeYContainer->setEnabled(false);

    m_offsetXContainer->setVisible(false);
    m_offsetYContainer->setVisible(false);

    m_offsetXContainer->setEnabled(false);
    m_offsetYContainer->setEnabled(false);

    m_enabledCheckbox->setVisible(false);
    m_enabledCheckbox->setScale(0.7f);
    m_enabledCheckbox->setPosition({m_enabledCheckbox->getScaledContentWidth() / 2.f + 3.f, m_background->getContentHeight() - m_enabledCheckbox->getScaledContentHeight() / 2.f - 3.f});

    m_clearBtn->setScale(0.9f);
    m_clearBtn->m_baseScale = 0.9f;
    m_clearBtn->setPosition({m_background->getContentWidth() - m_clearBtn->getScaledContentWidth() / 2.f - 3.f, m_background->getContentHeight() - m_clearBtn->getScaledContentHeight() / 2.f - 3.f});
    
    m_alignBtn->setVisible(false);

    if (m_page == 0) {
        m_titleLabel->setText("Grid Size");
        m_enabledCheckbox->setVisible(true);

        m_sizeXContainer->setVisible(separateXY);
        m_sizeYContainer->setVisible(separateXY);

        m_sizeXContainer->setEnabled(separateXY);
        m_sizeYContainer->setEnabled(separateXY);

        m_sizeContainer->setVisible(!separateXY);
        m_enabledLabel->setVisible(!separateXY);

        m_sizeContainer->setEnabled(!separateXY);

        if (!separateXY) {

            m_enabledCheckbox->setScale(1.f);

            m_enabledLabel->setPosition({12.f, m_background->getContentHeight() - 10.f});
            m_enabledCheckbox->setPosition({m_enabledLabel->getPositionX() + m_enabledLabel->getScaledContentWidth() / 2.f, m_enabledLabel->getPositionY() - m_enabledLabel->getScaledContentHeight() - m_enabledCheckbox->getScaledContentHeight() / 2.f - 6.f});

            m_clearBtn->setScale(1.f);
            m_clearBtn->m_baseScale = 1.f;
            m_clearBtn->setPosition({m_background->getContentWidth() - 25.f, m_background->getContentHeight() / 2.f});
        }
    }
    else if (m_page == 1) {
        m_titleLabel->setText("Grid Offset");

        m_offsetXContainer->setVisible(true);
        m_offsetYContainer->setVisible(true);

        m_offsetXContainer->setEnabled(true);
        m_offsetYContainer->setEnabled(true);

        auto editor = MainEditorUI::get();
        if (!editor) return;

        if (editor->m_selectedObject) {
            m_alignBtn->setVisible(true);
        }
    }
}

};

AdvancedGridControls::AdvancedGridControls() {
    m_offsetForKeyHook = Mod::get()->hook(
        reinterpret_cast<void*>(
            geode::addresser::getNonVirtual(&offsetForKey)
        ),
        &offsetForKey_h,
        "offsetForKey",
        tulip::hook::TulipConvention::Cdecl
    ).unwrapOrDefault();

    if (m_offsetForKeyHook) {
        m_offsetForKeyHook->setAutoEnable(false);
    }
}

AdvancedGridControls::~AdvancedGridControls() {
    if (m_offsetForKeyHook) {
        (void) Mod::get()->disownHook(m_offsetForKeyHook);
    }
}

bool AdvancedGridControls::onToggled(bool state) {
    auto editor = getEditor();

    auto it = editor->m_unkMap.find(4);
    if (it != editor->m_unkMap.end() && it->second) {
        editor->toggleWindow(4);
    }

    if (state) {
        if (m_offsetForKeyHook) {
            (void) m_offsetForKeyHook->enable();
        }

        AdvancedGridControls::setSaved<Save::Level, "enabled">(editor->m_customGridEnabled);
        AdvancedGridControls::setSaved<Save::Level, "separate-xy">(false);
        AdvancedGridControls::setSaved<Save::Level, "size">(editor->m_customGridSize);

        updateGrid();
    }
    else {
        if (m_offsetForKeyHook) {
            (void) m_offsetForKeyHook->disable();
        }

        auto dgl = getEditorLayer()->m_drawGridLayer;
        auto grid = static_cast<alpha::grid::Grid*>(dgl->getChildByID("alphalaneous.good_grid/grid"));

        grid->overrideGridSize(std::nullopt);
        grid->offsetGrid(std::nullopt);

        editor->runAction(CallFuncExt::create([this, editor] {
            float size = getSaved<Save::Level, float, "size">();
            if (size > 0.f) {
                size = std::max(5.f, std::min(size, 300.f));
            }

            editor->m_customGridSize = size;
            editor->m_customGridEnabled = getSaved<Save::Level, bool, "enabled">();

            GameManager::get()->setIntGameVariable("0193", static_cast<int>(size * 10.f));
            GameManager::get()->setGameVariable("0194", editor->m_customGridEnabled);

            editor->updateGridNodeSize();
        }));
    }

    if (it != editor->m_unkMap.end() && it->second) {
        editor->runAction(CallFuncExt::create([this, editor] {
            editor->toggleWindow(4);
            auto it = editor->m_unkMap.find(4);
            if (it != editor->m_unkMap.end() && it->second) {
                // fixing a goofy bug where adding it while another popup is visible takes priority, keyboard and keypad aren't needed anyways
                CCDirector::get()->m_pKeypadDispatcher->removeDelegate(static_cast<EditorWindowPopup*>(it->second));
                CCKeyboardDispatcher::get()->removeDelegate(static_cast<EditorWindowPopup*>(it->second));
            }
        }));
    }

    return true;
}

void AdvancedGridControls::onEditor() {
    if (m_offsetForKeyHook) {
        (void) m_offsetForKeyHook->enable();
    }
    updateGrid();
}

void AdvancedGridControls::updateGrid() {
    if (m_updatingGridSize) return;

    auto dgl = getEditorLayer()->m_drawGridLayer;
    if (!dgl) return;

    auto grid = static_cast<alpha::grid::Grid*>(dgl->getChildByID("alphalaneous.good_grid/grid"));
    if (!grid) return;

    auto editor = getEditor();

    m_separateXY = getSaved<Save::Level, bool, "separate-xy">();
    m_gridScale = getSaved<Save::Level, bool, "scale-objects">();

    if (!getSaved<Save::Level, bool, "enabled">()) {
        m_gridX = 30.f;
        m_gridY = 30.f;
        m_offsetX = 0.f;
        m_offsetY = 0.f;
    }
    else {
        if (m_separateXY) {
            m_gridX = getSaved<Save::Level, float, "size-x">();
            m_gridY = getSaved<Save::Level, float, "size-y">();
        }
        else {
            m_gridX = getSaved<Save::Level, float, "size">();
            m_gridY = m_gridX;
        }
    }

    m_offsetX = getSaved<Save::Level, float, "offset-x">();
    m_offsetY = getSaved<Save::Level, float, "offset-y">();

    m_updatingGridSize = true;
    editor->updateGridNodeSize();
    m_updatingGridSize = false;

    grid->offsetGrid(CCPoint{m_offsetX, m_offsetY});

    editor->m_customGridEnabled = false;
    editor->m_customGridSize = 30.f;
    editor->m_gridSize = 0.01f;
}

void AdvancedGridControls::updateWindow() {
    auto editor = getEditor();

    auto it = editor->m_unkMap.find(4);
    if (it != editor->m_unkMap.end() && it->second) {
        static_cast<tinker::ui::AdvancedGridSettingsLayer*>(it->second)->updateValues();
        return;
    }
}

float AdvancedGridControls::getGridMultiplierX() {
    if (!m_gridScale || !getSaved<Save::Level, bool, "enabled">()) return 1.f;

    if (m_gridX <= 0.f) {
        return 1.f;
    }

    return m_gridX / 30.f;
}

float AdvancedGridControls::getGridMultiplierY() {
    if (!m_gridScale || !getSaved<Save::Level, bool, "enabled">()) return 1.f;

    if (m_gridY <= 0.f) {
        return 1.f;
    }

    return m_gridY / 30.f;
}

cocos2d::CCPoint offsetForKey_h(int id) {
    auto ret = ::offsetForKey(id);

    auto control = AdvancedGridControls::get();
    if (control->m_getOrigOffset) return ret;

    auto multX = control->getGridMultiplierX();
    auto multY = control->getGridMultiplierY();

    ret.x *= multX;
    ret.y *= multY;
    return ret;
}

void AGCEditorUI::toggleWindow(int windowID) {
    auto it = m_unkMap.find(windowID);
    if (it != m_unkMap.end() && it->second) {
        static_cast<EditorWindowPopup*>(it->second)->onClose(nullptr);
        return;
    }

    if (windowID == 4) {
        auto window = tinker::ui::AdvancedGridSettingsLayer::create();
        m_unkMap[windowID] = window;
        window->show();
        return;
    }
    EditorUI::toggleWindow(windowID);
}

bool AGCEditorUI::handleAction(int action, bool down, double timestamp) {
    m_customGridSize = AdvancedGridControls::getSaved<Save::Level, float, "size">();
    m_customGridEnabled = AdvancedGridControls::getSaved<Save::Level, bool, "enabled">();

    auto ret = EditorUI::handleAction(action, down, timestamp);
    if (down && ret) {
        auto control = AdvancedGridControls::get();

        switch (action) {
            case 20118: 
            case 20084:
            case 20085:
            case 20145:
            case 20146: {
                AdvancedGridControls::setSaved<Save::Level, "enabled">(m_customGridEnabled);
                AdvancedGridControls::setSaved<Save::Level, "separate-xy">(false);
                AdvancedGridControls::setSaved<Save::Level, "size">(m_customGridSize);
                AdvancedGridControls::setSaved<Save::Level, "size-x">(m_customGridSize);
                AdvancedGridControls::setSaved<Save::Level, "size-y">(m_customGridSize);
                break;
            }
        }
        control->updateGrid();
        control->updateWindow();
    }

    m_customGridSize = 30.f;
    m_customGridEnabled = false;

    return ret;
}

GameObject* AGCEditorUI::onCreateObject(int id) {
    auto ret = EditorUI::onCreateObject(id);
    
    if (ret) {
        auto multX = AdvancedGridControls::get()->getGridMultiplierX();
        auto multY = AdvancedGridControls::get()->getGridMultiplierY();

        ret->updateCustomScaleX(ret->m_pixelScaleX * multX);
        ret->updateCustomScaleY(ret->m_pixelScaleY * multY);

        removeOffset(ret);

        CCPoint offset = GameToolbox::getRelativeOffset(ret, ::offsetForKey(ret->m_objectID));
        ret->setPosition(ret->getPosition() + offset);
        m_editorLayer->removeObjectFromSection(ret);
        m_editorLayer->addToSection(ret);
    }

    return ret;
}

cocos2d::CCPoint AGCEditorUI::getLimitedPosition(cocos2d::CCPoint position) {
    // `this` is invalid?
    auto editor = static_cast<AGCEditorUI*>(EditorUI::get());

    auto fields = editor->m_fields.self();
    if (fields->m_regularLimited) {
        return EditorUI::getLimitedPosition(position);
    }

    if (!editor->m_editorLayer || !editor->m_editorLayer->m_drawGridLayer) {
        return EditorUI::getLimitedPosition(position);
    }

    auto size = editor->m_editorLayer->m_drawGridLayer->m_gridSize;
    auto oldPoint = (position / size - CCPoint{0.5f, 0.5f}) * size;

    auto control = AdvancedGridControls::get();

    float mult = editor->getGridMultiplier();
    
    float sizeX = control->m_gridX * mult;
    float sizeY = control->m_gridY * mult;

    auto xVal = std::floor((oldPoint.x - control->m_offsetX) / sizeX);
    auto yVal = std::floor((oldPoint.y - control->m_offsetY - 90.f) / sizeY);

    return EditorUI::getLimitedPosition(CCPoint{(xVal + 0.5f) * sizeX + control->m_offsetX, (yVal + 0.5f) * sizeY + control->m_offsetY + 90.f});
}

void AGCEditorUI::moveObject(GameObject* object, cocos2d::CCPoint offset) {
    auto fields = m_fields.self();
    fields->m_regularLimited = true;
    EditorUI::moveObject(object, offset);
    fields->m_regularLimited = false;
}

cocos2d::CCPoint AGCEditorUI::moveForCommand(EditCommand command) {
    auto control = AdvancedGridControls::get();

    auto ret = EditorUI::moveForCommand(command);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wswitch"
    switch (command) {
        case EditCommand::SmallLeft:
        case EditCommand::SmallRight:
        case EditCommand::SmallUp:
        case EditCommand::SmallDown:
        case EditCommand::TinyLeft:
        case EditCommand::TinyRight:
        case EditCommand::TinyUp:
        case EditCommand::TinyDown:
        case static_cast<EditCommand>(33):
        case static_cast<EditCommand>(34):
        case static_cast<EditCommand>(35):
        case static_cast<EditCommand>(36):
        case static_cast<EditCommand>(25):
        case static_cast<EditCommand>(26):
        case static_cast<EditCommand>(27):
        case static_cast<EditCommand>(28):
            return ret;
        default: break;
    }
    #pragma GCC diagnostic pop

    ret *= 100;
    
    float mult = getGridMultiplier();
    return CCPoint{control->m_gridX * mult * ret.x, control->m_gridY * mult * ret.y};
}

float AGCEditorUI::getGridMultiplier() {
    float mult = 1.f;
    if (m_selectedMode == 2) {
        mult = ObjectToolbox::sharedState()->gridNodeSizeForKey(m_selectedObjectIndex) / 30.f;
    }
    else if (m_selectedObject) {
        mult = ObjectToolbox::sharedState()->gridNodeSizeForKey(m_selectedObject->m_objectID) / 30.f;
    }
    else if (m_selectedObjects && m_selectedObjects->count() > 0) {
        int objectID = m_selectedObjects->asExt<GameObject>()[0]->m_objectID;
        for (auto obj : m_selectedObjects->asExt<GameObject>()) {
            if (obj->m_objectID != objectID) {
                objectID = -1;
                break;
            }
        }
        if (objectID != -1) {
            mult = ObjectToolbox::sharedState()->gridNodeSizeForKey(objectID) / 30.f;
        }
    }
    return mult;
}

void AGCEditorUI::updateGridNodeSize() {
    auto dgl = m_editorLayer->m_drawGridLayer;
    if (!dgl) return;
    
    auto grid = static_cast<alpha::grid::Grid*>(dgl->getChildByID("alphalaneous.good_grid/grid"));
    if (!grid) return;
    
    auto control = AdvancedGridControls::get();

    float mult = getGridMultiplier();

    grid->overrideGridSize(CCSize{control->m_gridX * mult, control->m_gridY * mult});
    dgl->m_gridSize = 0.01f;

    if (control->m_updatingGridSize) return; 
    control->updateWindow();
}

cocos2d::CCPoint AGCGJTransformControl::snapToGrid(cocos2d::CCPoint position, bool anchor) {
    auto space = anchor ? static_cast<CCNode*>(this) : m_mainNodeParent;
    auto ui = m_delegate->getUI();
    auto control = AdvancedGridControls::get();

    auto objectLayer = ui->m_editorLayer->m_objectLayer;
    auto local = objectLayer->convertToNodeSpace(space->convertToWorldSpace(position));

    float gridX = control->m_gridX;
    float gridY = control->m_gridY;

    float offsetY = std::fmod(90.f, gridY);

    float reachX = std::min(5.f, gridX * 0.1f);
    float reachY = std::min(5.f, gridY * 0.1f);

    auto last = CCPoint{std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};

    if (anchor) {
        reachX = std::min(gridX * 0.35f, 8.f / objectLayer->getScale());
        reachY = std::min(gridY * 0.35f, 8.f / objectLayer->getScale());

        last = m_unk27c;
    }

    float snapX = std::round(local.x / gridX) * gridX;
    float snapY = std::round((local.y - offsetY) / gridY) * gridY + offsetY;
    float distanceX = std::abs(snapX - local.x);
    float stickyX = std::abs(last.x - local.x) / 1.5f;

    if (stickyX < distanceX) {
        distanceX = stickyX;
        snapX = last.x;
    }

    float lastX = snapX;
    if (reachX <= distanceX) {
        lastX = std::numeric_limits<float>::max();
        snapX = local.x;
    }

    float distanceY = std::abs(snapY - local.y);
    float stickyY = std::abs(last.y - local.y) / 1.5f;
    if (stickyY < distanceY) {
        distanceY = stickyY;
        snapY = last.y;
    }

    float lastY = snapY;
    if (reachY <= distanceY) {
        lastY = std::numeric_limits<float>::max();
        snapY = local.y;
    }

    if (anchor) m_unk27c = CCPoint{lastX, lastY};
    return space->convertToNodeSpace(objectLayer->convertToWorldSpace({snapX, snapY}));
}