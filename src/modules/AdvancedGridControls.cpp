#include "modules/AdvancedGridControls.hpp"
#include "MainHooks.hpp"
#include <alphalaneous.good_grid/DrawLayers.hpp>

using namespace geode::prelude;

/*namespace tinker::ui {

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

    auto bgSprite = m_sizeInput->getBGSprite();
    bgSprite->setContentWidth(140.f);
    bgSprite->setPositionY(bgSprite->getPositionY() - 0.5f);
    bgSprite->setScaleMultiplier(0.7);
    bgSprite->setOpacity(100);

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

GridSizePopup* GridSizePopup::create() {
    auto ret = new GridSizePopup();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool GridSizePopup::init() {
    if (!geode::Popup::init({300.f, 150.f})) return false;
    auto control = AdvancedGridControls::get();

    m_noElasticity = true;

    m_closeBtn->removeFromParent();

    m_titleLabel = geode::Label::create("Grid Size", "bigFont.fnt");
    m_titleLabel->setScale(0.5f);
    m_titleLabel->setAnchorPoint({0.5f, 1.f});
    m_titleLabel->setPosition({m_size.width / 2.f, m_size.height - 6.f});

    m_mainLayer->addChild(m_titleLabel);

    m_background = geode::NineSlice::create("square02_001-uhd.png");
    m_background->setAnchorPoint({0.5f, 1.f});
    m_background->setPosition({m_size.width / 2.f, m_size.height - 35.f});
    m_background->setOpacity(50);
    m_background->setContentSize({m_size.width - 30.f, 65.f});
    m_background->setScaleMultiplier(0.5f);

    auto bgMenu = CCMenu::create();
    bgMenu->setContentSize(m_background->getContentSize());
    bgMenu->ignoreAnchorPointForPosition(false);
    bgMenu->setAnchorPoint({0.5f, 0.5f});
    bgMenu->setPosition(m_background->getContentSize() / 2.f);

    m_background->addChild(bgMenu);

    m_mainLayer->addChild(m_background);

    auto okSpr = ButtonSprite::create("OK", 40.f, 40.f, 0.8f, true);
    okSpr->setID("ok-button-sprite"_spr);

    auto okBtn = CCMenuItemExt::createSpriteExtra(okSpr, [this] (auto sender) {
        onClose(sender);
    });
    okBtn->setID("ok-button"_spr);

    okBtn->setPosition({m_size.width / 2.f, 25.f});

    m_buttonMenu->addChild(okBtn);

    auto info = InfoAlertButton::create("Help", "TODO", 1.f);

    info->setPosition(m_size - CCSize{2.f, 2.f});

    m_buttonMenu->addChild(info);

    m_clearBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_trashBtn_001.png", 0.5f, [this, control] (auto sender) {
        if (m_page == 0) {
            control->m_gridX = 30.f;
            control->m_gridY = 30.f;

            Mod::get()->setSavedValue("grid-size", 30.f);
            Mod::get()->setSavedValue("grid-size-x", 30.f);
            Mod::get()->setSavedValue("grid-size-y", 30.f);

            m_sizeContainer->setValue(30.f);
            m_sizeXContainer->setValue(30.f);
            m_sizeYContainer->setValue(30.f);
        }
        else if (m_page == 1) {
            Mod::get()->setSavedValue("grid-offset-x", 0.f);
            Mod::get()->setSavedValue("grid-offset-y", 0.f);

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
        Mod::get()->setSavedValue("grid-size-enabled", !toggler->isOn());
        control->updateGrid();
    });

    auto enabled = Mod::get()->getSavedValue<bool>("grid-size-enabled");
    m_enabledCheckbox->toggle(enabled);

    bgMenu->addChild(m_enabledCheckbox);

    setupSizePage();
    setupOffsetPage();

    auto xyModeCheckbox = CCMenuItemExt::createTogglerWithStandardSprites(0.5f, [this, control] (CCMenuItemToggler* toggler) {
        Mod::get()->setSavedValue("grid-size-separate-xy", !toggler->isOn());

        updateMode();
        control->updateGrid();
    });

    auto xyModeEnabled = Mod::get()->getSavedValue<bool>("grid-size-separate-xy");
    xyModeCheckbox->toggle(xyModeEnabled);
    xyModeCheckbox->setPosition({17.f, 17.f});

    m_buttonMenu->addChild(xyModeCheckbox);

    auto xyModeLabel = geode::Label::create("XY Mode", "bigFont.fnt");
    xyModeLabel->setScale(0.3f);
    xyModeLabel->setAnchorPoint({0.f, 0.5f});
    xyModeLabel->setPosition({xyModeCheckbox->boundingBox().getMaxX() + 4.f, xyModeCheckbox->boundingBox().getMidY() + 0.5f});

    m_mainLayer->addChild(xyModeLabel);

    auto scaleCheckbox = CCMenuItemExt::createTogglerWithStandardSprites(0.5f, [this, control] (CCMenuItemToggler* toggler) {
        Mod::get()->setSavedValue("grid-size-scale-objects", !toggler->isOn());

        control->updateGrid();
    });

    auto scaleEnabled = Mod::get()->getSavedValue<bool>("grid-size-scale-objects");
    scaleCheckbox->toggle(scaleEnabled);
    scaleCheckbox->setPosition({17.f, 36.f});

    m_buttonMenu->addChild(scaleCheckbox);

    auto scaleLabel = geode::Label::create("Scale Objects", "bigFont.fnt");
    scaleLabel->setScale(0.3f);
    scaleLabel->setAnchorPoint({0.f, 0.5f});
    scaleLabel->setPosition({scaleCheckbox->boundingBox().getMaxX() + 4.f, scaleCheckbox->boundingBox().getMidY() + 0.5f});

    m_mainLayer->addChild(scaleLabel);

    auto leftArrow = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_arrow_01_001.png", 0.75f, [this] (auto sender) {
        m_page--;
        if (m_page < 0) {
            m_page = 1;
        }
        updateMode();
    });
    leftArrow->setPosition({-5.f - leftArrow->getScaledContentWidth() / 2.f, m_buttonMenu->getContentHeight() / 2.f});

    m_buttonMenu->addChild(leftArrow);

    auto rightArrow = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_arrow_01_001.png", 0.75f, [this] (auto sender) {
        m_page++;
        if (m_page > 1) {
            m_page = 0;
        }
        updateMode();
    });
    rightArrow->setPosition({m_buttonMenu->getContentWidth() + 5.f + rightArrow->getScaledContentWidth() / 2.f, m_buttonMenu->getContentHeight() / 2.f});

    static_cast<CCSprite*>(rightArrow->getNormalImage())->setFlipX(true);

    m_buttonMenu->addChild(rightArrow);

    updateMode();

    return true;
}

void GridSizePopup::setupSizePage() {
    m_sizeContainer = SizeContainer::create("Size:", Mod::get()->getSavedValue<float>("grid-size", 30.f), 7.5f, 120.f, 1.f, 300.f, false, [] (float value) {
        Mod::get()->setSavedValue("grid-size", value);
    });

    m_sizeContainer->setPosition(m_background->getContentWidth() / 2.f + 15.f, m_background->getContentHeight() / 2.f);

    m_background->addChild(m_sizeContainer);

    m_sizeXContainer = SizeContainer::create("Size X:", Mod::get()->getSavedValue<float>("grid-size-x", 30.f), 7.5f, 120.f, 1.f, 300.f, true, [] (float value) {
        Mod::get()->setSavedValue("grid-size-x", value);
    });

    m_sizeXContainer->setPosition(m_background->getContentWidth() / 2.f - m_sizeXContainer->getScaledContentWidth() / 2.f - 5.f, m_background->getContentHeight() / 2.f);

    m_background->addChild(m_sizeXContainer);

    m_sizeYContainer = SizeContainer::create("Size Y:", Mod::get()->getSavedValue<float>("grid-size-y", 30.f), 7.5f, 120.f, 1.f, 300.f, true, [] (float value) {
        Mod::get()->setSavedValue("grid-size-y", value);
    });

    m_sizeYContainer->setPosition(m_background->getContentWidth() / 2.f + m_sizeYContainer->getScaledContentWidth() / 2.f + 5.f, m_background->getContentHeight() / 2.f);

    m_background->addChild(m_sizeYContainer);
}

void GridSizePopup::setupOffsetPage() {
    m_offsetXContainer = SizeContainer::create("Offset X:", Mod::get()->getSavedValue<float>("grid-offset-x"), -150.f, 150.f, -150.f, 150.f, true, [] (float value) {
        Mod::get()->setSavedValue("grid-offset-x", value);
    });

    m_offsetXContainer->setPosition(m_background->getContentWidth() / 2.f - m_offsetXContainer->getScaledContentWidth() / 2.f - 5.f, m_background->getContentHeight() / 2.f);

    m_background->addChild(m_offsetXContainer);

    m_offsetYContainer = SizeContainer::create("Offset Y:", Mod::get()->getSavedValue<float>("grid-offset-y"), -150.f, 150.f, -150.f, 150.f, true, [] (float value) {
        Mod::get()->setSavedValue("grid-offset-y", value);
    });

    m_offsetYContainer->setPosition(m_background->getContentWidth() / 2.f + m_offsetYContainer->getScaledContentWidth() / 2.f + 5.f, m_background->getContentHeight() / 2.f);

    m_background->addChild(m_offsetYContainer);

    auto spr = ButtonSprite::create("To Object");
    spr->setScale(0.5f);

    // TODO FIX OFFSETFORKEY

    m_alignBtn = CCMenuItemExt::createSpriteExtra(spr, [this] (auto sender) {
        auto editor = MainEditorUI::get();
        if (!editor->m_selectedObject) return;

        auto control = AdvancedGridControls::get();

        Mod::get()->setSavedValue("grid-offset-x", 0.f);
        Mod::get()->setSavedValue("grid-offset-y", 0.f);

        control->updateGrid();

        auto offsetForKey = editor->offsetForKey(editor->m_selectedObject->m_objectID);

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

        Mod::get()->setSavedValue("grid-offset-x", offset.x);
        Mod::get()->setSavedValue("grid-offset-y", offset.y);

        m_offsetXContainer->setValue(offset.x);
        m_offsetYContainer->setValue(offset.y);

        control->updateGrid();
    });

    m_alignBtn->setPosition({m_buttonMenu->getContentWidth() - m_alignBtn->getScaledContentWidth() / 2.f - 12.f, 12.f + m_alignBtn->getScaledContentHeight() / 2.f});

    m_buttonMenu->addChild(m_alignBtn);
}

void GridSizePopup::updateMode() {
    bool separateXY = Mod::get()->getSavedValue<bool>("grid-size-separate-xy");
    
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

        if (editor->m_selectedObject) {
            m_alignBtn->setVisible(true);
        }
    }
}

};

void AdvancedGridControls::onEditor() {
    auto editorUI = getEditor();

    auto settingsMenu = editorUI->getChildByID("settings-menu");

    auto base = CircleButtonSprite::createWithSprite("grid-controls.png"_spr, 1.f, CircleBaseColor::Green, CircleBaseSize::Small);
    base->setScale(0.7f);

    auto btn = CCMenuItemExt::createSpriteExtra(base, [] (auto sender) {
        tinker::ui::GridSizePopup::create()->show();
    });

    editorUI->m_uiItems->addObject(btn);

    settingsMenu->addChild(btn);
    settingsMenu->updateLayout();

    updateGrid();
}

void AdvancedGridControls::updateGrid() {
    if (m_updatingGridSize) return;

    auto dgl = getEditorLayer()->m_drawGridLayer;
    auto grid = static_cast<alpha::grid::Grid*>(dgl->getChildByID("alphalaneous.good_grid/grid"));

    m_enabled = Mod::get()->getSavedValue<bool>("grid-size-enabled", true);
    m_separateXY = Mod::get()->getSavedValue<bool>("grid-size-separate-xy");
    m_gridScale = Mod::get()->getSavedValue<bool>("grid-size-scale-objects");

    if (!m_enabled) {
        m_gridX = 30.f;
        m_gridY = 30.f;
        m_offsetX = 0.f;
        m_offsetY = 0.f;
    }
    else {
        if (m_separateXY) {
            m_gridX = Mod::get()->getSavedValue<float>("grid-size-x", 30.f);
            m_gridY = Mod::get()->getSavedValue<float>("grid-size-y", 30.f);
        }
        else {
            m_gridX = Mod::get()->getSavedValue<float>("grid-size", 30.f);
            m_gridY = m_gridX;
        }
    }

    m_offsetX = Mod::get()->getSavedValue<float>("grid-offset-x");
    m_offsetY = Mod::get()->getSavedValue<float>("grid-offset-y");

    m_updatingGridSize = true;
    getEditor()->updateGridNodeSize();
    m_updatingGridSize = false;

    grid->offsetGrid(CCPoint{m_offsetX, m_offsetY});
}

float AdvancedGridControls::getGridMultiplierX() {
    if (!m_gridScale || !m_enabled) return 1.f;

    if (m_gridX <= 0.f) {
        return 1.f;
    }

    return m_gridX / 30.f;
}

float AdvancedGridControls::getGridMultiplierY() {
    if (!m_gridScale || !m_enabled) return 1.f;

    if (m_gridY <= 0.f) {
        return 1.f;
    }

    return m_gridY / 30.f;
}

GameObject* AGCEditorUI::createObject(int objectID, cocos2d::CCPoint position) {
    auto ret = EditorUI::createObject(objectID, position);

    if (ret) {
        auto multX = AdvancedGridControls::get()->getGridMultiplierX();
        auto multY = AdvancedGridControls::get()->getGridMultiplierY();

        ret->updateCustomScaleX(ret->m_pixelScaleX * multX);
        ret->updateCustomScaleY(ret->m_pixelScaleY * multY);
    }

    return ret;
}

cocos2d::CCPoint AGCEditorUI::getLimitedPosition(cocos2d::CCPoint position) {
    auto fields = m_fields.self();
    if (fields->m_regularLimited) {
        return EditorUI::getLimitedPosition(position);
    }

    auto size = m_editorLayer->m_drawGridLayer->m_gridSize;
    auto oldPoint = (position / size - CCPoint{0.5f, 0.5f}) * size;

    auto control = AdvancedGridControls::get();

    float mult = m_gridSize / 30.f;
    float sizeX = control->m_gridX * mult;
    float sizeY = control->m_gridY * mult;

    auto xVal = std::floorf((oldPoint.x - control->m_offsetX) / sizeX);
    auto yVal = std::floorf((oldPoint.y - control->m_offsetY) / sizeY);

    return EditorUI::getLimitedPosition(CCPoint{(xVal + 0.5f) * sizeX + control->m_offsetX, (yVal + 0.5f) * sizeY + control->m_offsetY});
}

void AGCEditorUI::moveObject(GameObject* object, cocos2d::CCPoint offset) {
    auto fields = m_fields.self();
    fields->m_regularLimited = true;
    EditorUI::moveObject(object, offset);
    fields->m_regularLimited = false;
}

cocos2d::CCPoint AGCEditorUI::offsetForKey(int id) {
    auto ret = EditorUI::offsetForKey(id);
    auto multX = AdvancedGridControls::get()->getGridMultiplierX();
    auto multY = AdvancedGridControls::get()->getGridMultiplierY();

    ret.x *= multX;
    ret.y *= multY;
    return ret;
}

void AGCEditorUI::updateGridNodeSize() {
    EditorUI::updateGridNodeSize();

    auto dgl = m_editorLayer->m_drawGridLayer;
    auto grid = static_cast<alpha::grid::Grid*>(dgl->getChildByID("alphalaneous.good_grid/grid"));
    auto control = AdvancedGridControls::get();

    auto size = dgl->m_gridSize;
    float mult = size / 30.f;

    grid->overrideGridSize(CCSize{control->m_gridX * mult, control->m_gridY * mult});

    dgl->m_gridSize = 0.01f;
}*/