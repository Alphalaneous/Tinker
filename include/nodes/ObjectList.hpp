#pragma once

#include <Geode/Geode.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

using namespace alpha::prelude;
using namespace geode::prelude;

namespace tinker::ui {
    enum class SummarySortType {
        None = -1,
        Solids,
        Hazards,
        Decoration,
        Portals,
        PadsOrbs,
        Triggers
    };

    struct SortOptions {
        SummarySortType sortType = SummarySortType::None;
        bool isAscending = false;
        bool showHidden = true;
    };

    class ObjectList : public CCNode {
    public:
        static ObjectList* create(const std::map<int, int>& objects, SortOptions sortOptions, const CCSize& size);
    protected:
        bool init(const std::map<int, int>& objects, SortOptions sortOptions, const CCSize& size);

        AdvancedScrollLayer* m_list;
        geode::NineSlice* m_bg;
    };
}
