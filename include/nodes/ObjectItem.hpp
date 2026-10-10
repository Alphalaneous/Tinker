#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>

using namespace geode::prelude;

namespace tinker::ui {
    class ObjectItem : public CCNode {
    protected:
    public:
        static ObjectItem* create(int objectID, int count);
    protected:
        bool init(int objectID, int count);
    };
}
