#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>

namespace tinker::ui {

class ObjectItem : public cocos2d::CCNode {
public:
    static ObjectItem* create(int objectID, int count);
protected:
    bool init(int objectID, int count);
};

}
