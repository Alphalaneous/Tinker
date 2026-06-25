#include "Timestamp.hpp"

Timestamp::Timestamp(ZStringView id) : m_id(id) {
    m_start = m_last = std::chrono::steady_clock::now();
}

void Timestamp::snapshot(ZStringView label) {
    auto now = std::chrono::steady_clock::now();

    auto total = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start).count();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last).count();

    log::debug("[{}] ({}) +{} ms (total {} ms)", m_id, label, delta, total);

    m_last = now;
}

Timestamp::~Timestamp() {
    snapshot("end");
}