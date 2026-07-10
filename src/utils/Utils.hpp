#pragma once

#include <Geode/Geode.hpp>
#include "utils/HijackCallback.hpp"

using namespace geode::prelude;

namespace tinker::utils {

    struct ColorData {
		ccColor3B color;
		bool blending;
		GLubyte opacity;
	};

    float getToolbarHeight();
    bool isColorable(GameObject* object);
    ColorData getActiveColor(LevelEditorLayer* editorLayer, int colorID);
    void updateCreateButtonColor(LevelEditorLayer* levelEditorLayer, CCNode* btn, int color1ID, int color2ID, const cocos2d::ccHSVValue& hsv1, const cocos2d::ccHSVValue& hsv2);
    void hijackButton(CCMenuItem* btn, HijackCallback::Hijack method);
    std::pair<std::string, std::string> splitIntoPair(const std::string& str);
    void forEachObject(GJBaseGameLayer const* game, std::function<void(GameObject*)> const& callback);
    CCPoint rotatePointAroundPivot(CCPoint point, CCPoint pivot, float angleDegrees);
    std::string capitalize(std::string_view input);
    std::vector<std::string> split(const std::string& str, const std::string& delimiter, int limit = -1);

    template<geode::utils::string::ConstexprString ID>
    inline Mod* getMod() {
        static auto mod = Loader::get()->getLoadedMod(ID.data());
        return mod;
    }

    template <geode::utils::string::ConstexprString A, geode::utils::string::ConstexprString B>
    consteval auto concat2() {
        geode::utils::string::ConstexprString<> out{};
        for (size_t i = 0; i < A.size(); ++i) {
            out.push(A.data()[i]);
        }
        for (size_t i = 0; i < B.size(); ++i) {
            out.push(B.data()[i]);
        }
        return out;
    }

    template <geode::utils::string::ConstexprString First, geode::utils::string::ConstexprString... Rest>
    consteval auto concat() {
        if constexpr (sizeof...(Rest) == 0) {
            return First;
        }
        else {
            return concat2<First, concat<Rest...>()>();
        }
    }

    template <geode::utils::string::ConstexprString a, geode::utils::string::ConstexprString b>
    constexpr bool equals() {
        if (a.size() != b.size()) {
            return false;
        }
        for (std::size_t i = 0; i < a.size(); ++i) {
            if (a.data()[i] != b.data()[i]) {
                return false;
            }
        }
        return true;
    }

    template<class S, geode::utils::string::ConstexprString key>
    const S& getSetting() {
        static auto setting = Mod::get()->getSettingValue<S>(key.data());
        static auto listener = listenForSettingChanges<S>(key.data(), [] (S value) {
            setting = std::move(value);
        });
        return setting;
    }

    template<class S, geode::utils::string::ConstexprString key, auto Callback = nullptr>
    void onSetting() {
        static auto listener = listenForSettingChanges<S>(key.data(), [] (S value) {
            if constexpr (Callback) {
                Callback(value);
            }
        });
        if constexpr (Callback) {
            Callback(getSetting<S, key>());
        }
    }

    namespace color {
        struct RGB {
            float r, g, b;
        };

        struct HSV {
            float h;
            float s;
            float v;
        };

        constexpr float clamp01(float v);
        float wrapDegrees(float h);
        HSV rgbToHsv(RGB in);
        RGB hsvToRgb(HSV in);
        cocos2d::ccColor4B hueShift(cocos2d::ccColor4B color, float shiftDegrees);
    }
}
