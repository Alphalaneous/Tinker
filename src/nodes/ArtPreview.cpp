#include "nodes/ArtPreview.hpp"

using namespace tinker::ui;

PreviewGameLayer* PreviewGameLayer::create(int background, int ground, int groundLine, int middleground) {
    auto ret = new PreviewGameLayer();
    if (ret->init(background, ground, groundLine, middleground)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool PreviewGameLayer::init(int background, int ground, int groundLine, int middleground) {
    if (!CCLayer::init()) return false;
    ignoreAnchorPointForPosition(false);

    updateBG(background);
    updateGround(ground, groundLine);
    updateMiddleground(middleground);

    scheduleUpdate();
    return true;
}

void PreviewGameLayer::update(float dt) {
    constexpr float rate = 60.f;
    constexpr float modifier = 5.77f;

    float modDt = dt * rate * modifier;

    m_bgOffset = m_bgOffset - modDt * 0.1f;
    if (-m_bgOffset >= m_backgroundWidth) {
        m_bgOffset += -m_bgOffset;
    }

    m_backgroundSprite->setPositionX(m_bgOffset);

    m_groundOffset = m_groundOffset - modDt * 0.9f;
    if (-m_groundOffset >= m_groundLayer->m_textureWidth) {
        m_groundOffset += -m_groundOffset;
    }

    m_groundLayer->updateGroundPos({m_groundOffset, 0});

    if (m_middleGround) {
        m_mgOffset = m_mgOffset - modDt * 0.3f;
        if (-m_mgOffset >= m_middleGround->m_textureWidth) {
            m_mgOffset += -m_mgOffset;
        }

        m_middleGround->updateGroundPos({m_mgOffset, 0});
    }
}

void PreviewGameLayer::updateBG(int background) {
    if (m_backgroundSprite) {
        m_backgroundSprite->removeFromParent();
    }

    GameManager::get()->loadBackground(background);
    m_backgroundSprite = CCSprite::create(fmt::format("game_bg_{:02d}_001.png", background).c_str());

    ccTexParams params = {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
    m_backgroundSprite->getTexture()->setTexParameters(&params);
    m_backgroundSprite->setZOrder(-100);
    m_backgroundSprite->setScale(1.185f);
    m_backgroundSprite->setAnchorPoint({0.f, 0.f});
    m_backgroundSprite->setColor({40, 125, 255});
    m_backgroundSprite->setID("background"_spr);
    m_backgroundWidth = m_backgroundSprite->getScaledContentWidth();

    auto textureRect = m_backgroundSprite->getTextureRect();
    textureRect.size.width = textureRect.size.width * 2.f;
    m_backgroundSprite->setTextureRect(textureRect);

    updateBGColor(m_bgColor);

    addChild(m_backgroundSprite);
}

void PreviewGameLayer::updateGround(int ground, int groundLine) {
    if (m_groundLayer) {
        m_groundLayer->removeFromParent();
    }

    GameManager::get()->loadGround(ground);
    m_groundLayer = GJGroundLayer::create(ground, groundLine);
    m_groundLayer->setPositionY(90.f);
    m_groundLayer->setZOrder(100);
    m_groundLayer->setID("ground"_spr);

    updateGroundColor(m_groundColor1, m_groundColor2, m_lineColor, m_lineOpacity, m_lineBlend);

    addChild(m_groundLayer);
}

void PreviewGameLayer::updateMiddleground(int middleground) {
    if (m_middleGround) {
        m_middleGround->removeFromParent();
        m_middleGround = nullptr;
    }
    if (middleground == 0) {
        return;
    }

    GameManager::get()->loadMiddleground(middleground);
    m_middleGround = GJMGLayer::create(middleground);
    m_middleGround->setPositionY(42.f);
    m_middleGround->setID("middleground"_spr);

    updateMiddlegroundColor(m_mgColor1, m_mgOpacity1, m_mgBlend1, m_mgColor2, m_mgOpacity2, m_mgBlend2);

    addChild(m_middleGround);
}

void PreviewGameLayer::updateBGColor(const ccColor3B& color) {
    m_bgColor = color;

    m_backgroundSprite->setColor(color);
}

void PreviewGameLayer::updateGroundColor(const ccColor3B& color1, const ccColor3B& color2, const ccColor3B& lineColor, GLubyte lineOpacity, bool lineBlend) {
    m_groundColor1 = color1;
    m_groundColor2 = color2;

    m_lineColor = lineColor;
    m_lineOpacity = lineOpacity;
    m_lineBlend = lineBlend;

    m_groundLayer->updateGround01Color(color1);
    m_groundLayer->updateGround02Color(color2);
    m_groundLayer->updateLineBlend(lineBlend);

    m_groundLayer->m_lineSprite->setColor(lineColor);
    m_groundLayer->m_lineSprite->setOpacity(lineOpacity);
}

void PreviewGameLayer::updateMiddlegroundColor(const ccColor3B& color1, GLubyte opacity1, bool blend1, const ccColor3B& color2, GLubyte opacity2, bool blend2) {
    m_mgColor1 = color1;
    m_mgColor2 = color2;

    m_mgOpacity1 = opacity1;
    m_mgOpacity2 = opacity2;

    m_mgBlend1 = blend1;
    m_mgBlend2 = blend2;

    if (!m_middleGround) return;

    m_middleGround->updateGroundColor(color1, true);
    m_middleGround->updateGroundColor(color2, false);

    m_middleGround->updateGroundOpacity(opacity1, true);
    m_middleGround->updateGroundOpacity(opacity2, false);

    m_middleGround->updateMG01Blend(blend1);
    m_middleGround->updateMG02Blend(blend2);
}

ArtPreview* ArtPreview::create(CCSize size, int background, int ground, int groundLine, int middleground) {
    auto ret = new ArtPreview();
    if (ret->init(size, background, ground, groundLine, middleground)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ArtPreview::init(CCSize size, int background, int ground, int groundLine, int middleground) {
    setContentSize(size);

    m_container = CCNode::create();
    m_container->setAnchorPoint({0.f, 0.f});
    m_container->setContentSize(size);
    m_container->setID("preview-layer-container"_spr);

    m_preview = PreviewGameLayer::create(background, ground, groundLine, middleground);
    m_preview->setScale(0.7f);
    m_preview->setAnchorPoint({0.5f, 0.f});
    m_preview->setPosition({size.width / 2.f, 0.f});
    m_preview->setID("preview-layer"_spr);

    m_container->addChild(m_preview);

    m_render = alpha::ui::RenderNode::create(m_container, true);
    m_render->setAnchorPoint({0.f, 0.f});
    m_render->setID("preview-render"_spr);

    addChild(m_render);

    return true;
}

void ArtPreview::updateBG(int background) {
    m_preview->updateBG(background);
}

void ArtPreview::updateGround(int ground, int groundLine) {
    m_preview->updateGround(ground, groundLine);
}

void ArtPreview::updateMiddleground(int middleground) {
    m_preview->updateMiddleground(middleground);
}

void ArtPreview::updateBGColor(const ccColor3B& color) {
    m_preview->updateBGColor(color);
}

void ArtPreview::updateGroundColor(const ccColor3B& color1, const ccColor3B& color2, const ccColor3B& lineColor, GLubyte lineOpacity, bool lineBlend) {
    m_preview->updateGroundColor(color1, color2, lineColor, lineOpacity, lineBlend);
}

void ArtPreview::updateMiddlegroundColor(const ccColor3B& color1, GLubyte opacity1, bool blend1, const ccColor3B& color2, GLubyte opacity2, bool blend2) {
    m_preview->updateMiddlegroundColor(color1, opacity1, blend1, color2, opacity2, blend2);
}