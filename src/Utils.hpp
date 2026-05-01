#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tinker::utils {

    template <class T>
    struct FakeClass final {
        alignas(T) std::byte storage[sizeof(T)];

        T* get() noexcept {
            return std::launder(reinterpret_cast<T*>(storage));
        }

        const T* get() const noexcept {
            return std::launder(reinterpret_cast<const T*>(storage));
        }

        T* operator->() noexcept { return get(); }
        const T* operator->() const noexcept { return get(); }

        T& operator*() noexcept { return *get(); }
        const T& operator*() const noexcept { return *get(); }
    };

    template<class T>
    struct Singleton {
        static T* get() {
            static T instance;
            return &instance;
        }
    };

    struct HijackCallback : public CCObject {

        using Hijack = std::function<void(std::function<void(CCObject* sender)> orig, CCObject* sender)>;

        Hijack m_method;
        SEL_MenuHandler m_selector;

        static HijackCallback* create(Hijack method, SEL_MenuHandler originalSelector) {
            auto ret = new HijackCallback();
            if (ret->init(method, originalSelector)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        bool init(Hijack method, SEL_MenuHandler originalSelector) {
            m_method = method;
            m_selector = originalSelector;
            return true;
        }

        void callback(CCObject* sender) {
            auto btn = static_cast<CCMenuItem*>(sender);
            auto hijack = static_cast<HijackCallback*>(btn->getUserObject("hijack"_spr));
            
            if (hijack->m_method) hijack->m_method([btn, hijack] (CCObject* sender) {
                (btn->m_pListener->*hijack->m_selector)(sender);
            }, sender);
        }
    };

    inline void hijackButton(CCMenuItem* btn, HijackCallback::Hijack method) {
        if (btn->getUserObject("hijack"_spr)) return;

        auto hijack = HijackCallback::create(method, btn->m_pfnSelector);
        btn->setUserObject("hijack"_spr, hijack);
        btn->m_pfnSelector = menu_selector(HijackCallback::callback);
    }

    template<geode::utils::string::ConstexprString ID>
    inline Mod* getMod() {
        static auto mod = Loader::get()->getLoadedMod(ID.data());
        return mod;
    }

    inline std::pair<std::string, std::string> splitIntoPair(const std::string& str) {
        auto split = geode::utils::string::split(str, ":");
        auto& key = split[0];
        if (split.size() < 2) return {key, ""};
        auto value = str.substr(key.size() + 1);

        return {key, value};
    }

    inline void forEachObject(GJBaseGameLayer const* game, std::function<void(GameObject*)> const& callback) {
        int count = game->m_sections.empty() ? -1 : game->m_sections.size();
        for (int i = game->m_leftSectionIndex; i <= game->m_rightSectionIndex && i < count; ++i) {
            auto leftSection = game->m_sections[i];
            if (!leftSection) continue;

            auto leftSectionSize = leftSection->size();
            for (int j = game->m_bottomSectionIndex; j <= game->m_topSectionIndex && j < leftSectionSize; ++j) {
                auto section = leftSection->at(j);
                if (!section) continue;

                auto sectionSize = game->m_sectionSizes[i]->at(j);
                for (int k = 0; k < sectionSize; ++k) {
                    auto obj = section->at(k);
                    if (!obj) continue;

                    callback(obj);
                }
            }
        }
    }

    inline CCPoint rotatePointAroundPivot(CCPoint point, CCPoint pivot, float angleDegrees) {
        float angleRadians = CC_DEGREES_TO_RADIANS(angleDegrees);

        float sinA = std::sinf(angleRadians);
        float cosA = std::cosf(angleRadians);

        point.x -= pivot.x;
        point.y -= pivot.y;

        float xNew = point.x * cosA - point.y * sinA;
        float yNew = point.x * sinA + point.y * cosA;

        return CCPoint(xNew + pivot.x, yNew + pivot.y);
    }

    inline CCTouchHandler* findHandler(CCTouchDelegate* delegate) {
        auto mainNode = typeinfo_cast<CCNode*>(delegate);
        for (auto handler : CCArrayExt<CCTouchHandler*>(CCTouchDispatcher::get()->m_pTargetedHandlers)) {
            if (auto node = typeinfo_cast<CCNode*>(handler->getDelegate())) {
                if (mainNode == node) return handler;
            }
        }
        return nullptr;
    }

    inline void collectHandlers(CCNode* node, std::vector<CCTouchHandler*>& out) {
        for (auto child : node->getChildrenExt()) {
            if (auto delegate = typeinfo_cast<CCTouchDelegate*>(child)) {
                if (auto handler = findHandler(delegate)) {
                    out.emplace_back(handler);
                }
            }
            collectHandlers(child, out);
        }
    }

    inline void offsetTouchPrio(CCNode* node, int offset) {
        std::vector<CCTouchHandler*> handlers;
        collectHandlers(node, handlers);

        if (auto delegate = typeinfo_cast<CCTouchDelegate*>(node)) {
            if (auto handler = findHandler(delegate)) {
                handlers.emplace_back(handler);
            }
        }

        for (const auto& handler : handlers) {
            CCTouchDispatcher::get()->setPriority(handler->getPriority() + offset, handler->getDelegate());
        }
    }

    inline std::vector<std::string> split(const std::string& str, const std::string& delimiter, int limit = -1) {
        std::vector<std::string> result;
        size_t start = 0;
        size_t end = str.find(delimiter);
        int count = 0;

        while (end != std::string::npos && (count < limit - 1 || limit == -1)) {
            result.push_back(str.substr(start, end - start));
            start = end + delimiter.length();
            end = str.find(delimiter, start);
            count++;
        }

        result.push_back(str.substr(start));
        return result;
    }

    template <geode::utils::string::ConstexprString A, geode::utils::string::ConstexprString B>
    consteval auto concat2() {
        geode::utils::string::ConstexprString<> out{};
        for (size_t i = 0; i < A.size(); ++i)
            out.push(A.data()[i]);
        for (size_t i = 0; i < B.size(); ++i)
            out.push(B.data()[i]);
        return out;
    }

    template <geode::utils::string::ConstexprString First, geode::utils::string::ConstexprString... Rest>
    consteval auto concat() {
        if constexpr (sizeof...(Rest) == 0)
            return First;
        else
            return concat2<First, concat<Rest...>()>();
    }

    template <geode::utils::string::ConstexprString a, geode::utils::string::ConstexprString b>
    constexpr bool equals() {
        if (a.size() != b.size()) return false;
        for (std::size_t i = 0; i < a.size(); ++i) {
            if (a.data()[i] != b.data()[i]) return false;
        }
        return true;
    }

    template<class S, geode::utils::string::ConstexprString key>
    S getSetting() {
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

        constexpr float clamp01(float v) {
            return std::clamp(v, 0.0f, 1.0f);
        }

        inline float wrapDegrees(float h) {
            h = std::fmod(h, 360.0f);
            if (h < 0.0f) h += 360.0f;
            return h;
        }

        inline HSV rgbToHsv(RGB in) {
            const float max = std::max({in.r, in.g, in.b});
            const float min = std::min({in.r, in.g, in.b});
            const float delta = max - min;

            HSV out{};
            out.v = max;

            if (max == 0.0f) {
                out.s = 0.0f;
                out.h = 0.0f;
                return out;
            }

            out.s = delta / max;

            if (delta == 0.0f) {
                out.h = 0.0f;
            } else if (max == in.r) {
                out.h = 60.0f * std::fmod((in.g - in.b) / delta, 6.0f);
            } else if (max == in.g) {
                out.h = 60.0f * (((in.b - in.r) / delta) + 2.0f);
            } else {
                out.h = 60.0f * (((in.r - in.g) / delta) + 4.0f);
            }

            if (out.h < 0.0f) out.h += 360.0f;
            return out;
        }

        inline RGB hsvToRgb(HSV in) {
            const float c = in.v * in.s;
            const float hPrime = in.h / 60.0f;
            const float x = c * (1.0f - std::fabs(std::fmod(hPrime, 2.0f) - 1.0f));
            const float m = in.v - c;

            float r1 = 0.0f, g1 = 0.0f, b1 = 0.0f;

            if (hPrime < 1.0f)       { r1 = c; g1 = x; }
            else if (hPrime < 2.0f)  { r1 = x; g1 = c; }
            else if (hPrime < 3.0f)  { g1 = c; b1 = x; }
            else if (hPrime < 4.0f)  { g1 = x; b1 = c; }
            else if (hPrime < 5.0f)  { r1 = x; b1 = c; }
            else                     { r1 = c; b1 = x; }

            return { r1 + m, g1 + m, b1 + m };
        }

        inline cocos2d::ccColor4B hueShift(cocos2d::ccColor4B color, float shiftDegrees) {
            RGB rgb {
                color.r / 255.0f,
                color.g / 255.0f,
                color.b / 255.0f
            };

            HSV hsv = rgbToHsv(rgb);
            hsv.h = wrapDegrees(hsv.h + shiftDegrees);

            RGB out = hsvToRgb(hsv);

            return cocos2d::ccColor4B{
                static_cast<std::uint8_t>(clamp01(out.r) * 255.0f),
                static_cast<std::uint8_t>(clamp01(out.g) * 255.0f),
                static_cast<std::uint8_t>(clamp01(out.b) * 255.0f),
                color.a
            };
        }

    }

    class Timestamp {
    public:
        Timestamp(ZStringView id) : m_id(id) {
            m_start = m_last = std::chrono::steady_clock::now();
        }

        void snapshot(ZStringView label) {
            auto now = std::chrono::steady_clock::now();

            auto total = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start).count();
            auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last).count();

            log::debug("[{}] ({}) +{} ms (total {} ms)", m_id, label, delta, total);

            m_last = now;
        }

        ~Timestamp() {
            snapshot("end");
        }

    private:
        std::string m_id;
        std::chrono::time_point<std::chrono::steady_clock> m_start;
        std::chrono::time_point<std::chrono::steady_clock> m_last;
    };
}