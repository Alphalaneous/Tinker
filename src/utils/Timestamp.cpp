#include "utils/Timestamp.hpp"

using namespace geode::prelude;

Timestamp::Timestamp(ZStringView id) : m_id(id) {
    m_start = m_last = std::chrono::steady_clock::now();
}

void Timestamp::snapshot(ZStringView label) {
    auto now = std::chrono::steady_clock::now();

    auto total = std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_start).count();
    auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_last).count();

    log::debug("[{}] ({}) +{:.2f} ms (total {:.2f} ms)", m_id, label, delta / 1000000.f, total / 1000000.f);

    m_last = now;
}

Timestamp::~Timestamp() {
    snapshot("end");
}