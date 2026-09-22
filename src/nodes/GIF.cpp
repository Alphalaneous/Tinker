#include <nodes/GIF.hpp>

namespace tinker::ui {

GIF* GIF::create(ZStringView gif) {
    auto ret = new GIF();
    if (ret->init(gif)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool GIF::init(ZStringView gif) {
    std::string suffix;
#ifdef GEODE_IS_ANDROID32
    suffix = ".png";
#else
    suffix = ".gif";
#endif
    return CCSprite::initWithFile(fmt::format("{}{}"_spr, gif, suffix).c_str());
}

}