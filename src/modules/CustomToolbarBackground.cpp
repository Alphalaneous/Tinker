#include "CustomToolbarBackground.hpp"
#include "../third-party/BlurAPI.hpp"
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>

bool CustomToolbarBackground::onSettingChanged(std::string_view key, const matjson::Value& value) {
    
    if (key == "blur-behind") {
        return false;
    }
    
    auto colorRes = value.as<ccColor4B>();
    if (!colorRes.isOk()) return true;
    auto color = colorRes.unwrap();

    if (key == "gradient-start-color") {
        m_gradient->setStartColor({color.r, color.g, color.b});
        m_gradient->setStartOpacity(color.a);
    }
    if (key == "gradient-end-color") {
        m_gradient->setEndColor({color.r, color.g, color.b});
        m_gradient->setEndOpacity(color.a);
    }
    if (key == "line-color") {
        m_line->setColor({color.r, color.g, color.b});
        m_line->setOpacity(color.a);
    }

    return true;
}

void CustomToolbarBackground::onEditor() {
    
    auto toolbarBG = static_cast<CCSprite*>(m_editorUI->getChildByID("background-sprite"));
    
    auto oldContentSize = toolbarBG->getContentSize();
    toolbarBG->setTextureRect({-1, -1, 0, 0});
    toolbarBG->setContentSize(oldContentSize);

    m_gradient = CCLayerGradient::create(getSetting<ccColor4B, "gradient-start-color">(), getSetting<ccColor4B, "gradient-end-color">());
    m_gradient->setContentSize(oldContentSize + CCSize{0, -1});
    m_gradient->ignoreAnchorPointForPosition(false);
    m_gradient->setAnchorPoint({0, 0});

    toolbarBG->addChild(m_gradient);

    m_line = CCLayerColor::create(getSetting<ccColor4B, "line-color">());
    m_line->setContentSize({oldContentSize.width, 1});
    m_line->ignoreAnchorPointForPosition(false);
    m_line->setAnchorPoint({0, 1});
    m_line->setPosition({0, oldContentSize.height});
    m_line->setZOrder(1);

    toolbarBG->addChild(m_line);

    if (getSetting<bool, "blur-behind">()) {
	    BlurAPI::addBlur(toolbarBG);
	    BlurAPI::addBlur(m_editorUI->getChildByID("build-tabs-menu"));
    }
}

class $nodeModify(CTBGroup, Group) {
    void modify() {
        if (!CustomToolbarBackground::isEnabled()) return;
        if (!CustomToolbarBackground::getSetting<bool, "blur-behind">()) return;
        if (getID() != "RaZooM") return;
        
	    BlurAPI::addBlur(this);
    }
};