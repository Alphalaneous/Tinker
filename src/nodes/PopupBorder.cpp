#include "nodes/PopupBorder.hpp"

namespace tinker::ui {

PopupBorder* PopupBorder::create(const ccColor3B& topColor, GLubyte topOpacity, const ccColor3B& bottomColor, GLubyte bottomOpacity) {
    auto ret = new PopupBorder();
    if (ret->init(topColor, topOpacity, bottomColor, bottomOpacity)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool PopupBorder::init(const ccColor3B& topColor, GLubyte topOpacity, const ccColor3B& bottomColor, GLubyte bottomOpacity) {
    m_border = NineSlice::create("GJ_square07.png");
    m_border->setZOrder(1);
    m_border->setID("border"_spr);

    addChild(m_border);

    m_innerTopBorder = NineSlice::create("inner-border.png"_spr);
    m_innerTopBorder->setZOrder(0);
    m_innerTopBorder->setColor(topColor);
    m_innerTopBorder->setOpacity(topOpacity);
    m_innerTopBorder->setID("inner-top-border"_spr);

    addChild(m_innerTopBorder);

    m_innerBottomBorder = NineSlice::create("inner-border.png"_spr);
    m_innerBottomBorder->setZOrder(0);
    m_innerBottomBorder->setColor(bottomColor);
    m_innerBottomBorder->setScaleY(-1.f);
    m_innerBottomBorder->setOpacity(bottomOpacity);
    m_innerBottomBorder->setID("inner-bottom-border"_spr);

    addChild(m_innerBottomBorder);

    return true;
}

void PopupBorder::setContentSize(const CCSize& contentSize) {
    CCNode::setContentSize(contentSize);

    auto posOffset = CCPoint{0.f, 0.f};
    auto sizeOffset = CCSize{0.f, 0.f};
    if (m_hideLeft) {
        posOffset.x -= m_border->getInsetLeft() / 2.f;
        sizeOffset.width += m_border->getInsetLeft();
    }
    if (m_hideRight) {
        posOffset.x += m_border->getInsetRight() / 2.f;
        sizeOffset.width += m_border->getInsetRight();
    }
    if (m_hideTop) {
        posOffset.y += m_border->getInsetTop() / 2.f;
        sizeOffset.height += m_border->getInsetTop();
    }
    if (m_hideBottom) {
        posOffset.y -= m_border->getInsetBottom() / 2.f;
        sizeOffset.height += m_border->getInsetBottom();
    }

    m_border->setPosition(contentSize / 2.f + posOffset);
    m_innerTopBorder->setPosition(contentSize / 2.f + posOffset + CCPoint{0.f, 0.5f});
    m_innerBottomBorder->setPosition(contentSize / 2.f + posOffset - CCPoint{0.f, 0.5f});

    m_border->setContentSize(contentSize + sizeOffset);
    m_innerTopBorder->setContentSize(contentSize + sizeOffset);
    m_innerBottomBorder->setContentSize(contentSize + sizeOffset);
}

void PopupBorder::hideLeft(bool hide) {
    m_hideLeft = hide;

    m_border->getBottomLeft()->setVisible(!(hide || m_hideBottom));
    m_border->getLeft()->setVisible(!hide);
    m_border->getTopLeft()->setVisible(!(hide || m_hideTop));

    m_innerTopBorder->getBottomLeft()->setVisible(!(hide || m_hideBottom));
    m_innerTopBorder->getLeft()->setVisible(!hide);
    m_innerTopBorder->getTopLeft()->setVisible(!(hide || m_hideTop));

    m_innerBottomBorder->getBottomLeft()->setVisible(!(hide || m_hideBottom));
    m_innerBottomBorder->getLeft()->setVisible(!hide);
    m_innerBottomBorder->getTopLeft()->setVisible(!(hide || m_hideTop));

    setContentSize(getContentSize());
}

void PopupBorder::hideRight(bool hide) {
    m_hideRight = hide;

    m_border->getBottomRight()->setVisible(!(hide || m_hideBottom));
    m_border->getRight()->setVisible(!hide);
    m_border->getTopRight()->setVisible(!(hide || m_hideTop));

    m_innerTopBorder->getBottomRight()->setVisible(!(hide || m_hideBottom));
    m_innerTopBorder->getRight()->setVisible(!hide);
    m_innerTopBorder->getTopRight()->setVisible(!(hide || m_hideTop));

    m_innerBottomBorder->getBottomRight()->setVisible(!(hide || m_hideBottom));
    m_innerBottomBorder->getRight()->setVisible(!hide);
    m_innerBottomBorder->getTopRight()->setVisible(!(hide || m_hideTop));

    setContentSize(getContentSize());
}

void PopupBorder::hideTop(bool hide) {
    m_hideTop = hide;

    m_border->getTopLeft()->setVisible(!(hide || m_hideLeft));
    m_border->getTop()->setVisible(!hide);
    m_border->getTopRight()->setVisible(!(hide || m_hideRight));

    m_innerTopBorder->getTopLeft()->setVisible(!(hide || m_hideLeft));
    m_innerTopBorder->getTop()->setVisible(!hide);
    m_innerTopBorder->getTopRight()->setVisible(!(hide || m_hideRight));

    m_innerBottomBorder->getBottomLeft()->setVisible(!(hide || m_hideLeft));
    m_innerBottomBorder->getBottom()->setVisible(!hide);
    m_innerBottomBorder->getBottomRight()->setVisible(!(hide || m_hideRight));

    setContentSize(getContentSize());
}

void PopupBorder::hideBottom(bool hide) {
    m_hideBottom = hide;

    m_border->getBottomLeft()->setVisible(!(hide || m_hideLeft));
    m_border->getBottom()->setVisible(!hide);
    m_border->getBottomRight()->setVisible(!(hide || m_hideRight));

    m_innerTopBorder->getBottomLeft()->setVisible(!(hide || m_hideLeft));
    m_innerTopBorder->getBottom()->setVisible(!hide);
    m_innerTopBorder->getBottomRight()->setVisible(!(hide || m_hideRight));

    m_innerBottomBorder->getTopLeft()->setVisible(!(hide || m_hideLeft));
    m_innerBottomBorder->getTop()->setVisible(!hide);
    m_innerBottomBorder->getTopRight()->setVisible(!(hide || m_hideRight));

    setContentSize(getContentSize());
}

}