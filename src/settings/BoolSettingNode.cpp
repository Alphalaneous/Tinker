#include <Geode/Geode.hpp>
#include "settings/SettingNode.hpp"

using namespace geode::prelude;

class BoolSettingNode : public tinker::settings::SettingNode<BoolSettingNode, "boolA"> {
public:

    bool init(float width, std::shared_ptr<SettingV3> setting) {
        if (!SettingNodeBase::init(width, setting)) return false;


        return true;
    }
};