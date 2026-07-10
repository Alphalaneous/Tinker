#include "OldColorTriggers.hpp"
#include "utils/Constants.hpp"

using namespace tinker::constants::objects;

std::unordered_map<int, std::string> OldColorTriggers::s_textureMap = {
    {BackgroundColorTrigger, "edit_eTintBGBtn_001.png"_spr},
    {Ground1ColorTrigger, "edit_eTintGBtn_001.png"_spr},
    {ObjColorTrigger, "edit_eTintObjBtn_001.png"_spr},
    {Line3DColorTrigger, "edit_eTint3DLBtn_001.png"_spr},
    {Ground2ColorTrigger, "edit_eTintG2Btn_001.png"_spr},
    {LineColorTrigger, "edit_eTintLBtn_001.png"_spr},
};

void OCTEffectGameObject::customSetup() {
    EffectGameObject::customSetup();
    if (!OldColorTriggers::s_textureMap.contains(m_objectID)) return;

    if (auto newSpr = CCSprite::create(OldColorTriggers::s_textureMap[m_objectID].c_str())) {
        m_addToNodeContainer = true;
        setTexture(newSpr->getTexture());
        setTextureRect(newSpr->getTextureRect());
    }
}