#include "ui_record_bounds.hpp"

namespace d2::client::ui_record_bounds {
// Prior art used: UI/HUD and RE-map rows; D2MOO has no
// D2Client module. Community pointer tables do not name these internals.
// Source: cached 1.13c bodies, checked against the local DLL's instructions.
// Caller 0x8F430 selects EAX indices; all five helpers preserve record bytes 4..7.

// 1.13c D2Client+0x8B9C0
void configure_8b9c0(Record& r) {
    r.string_id = 0; r.style = 1; r.count = 1; r.frame = 0; r.active = 0;
}

// 1.13c D2Client+0x8B9F0
void configure_8b9f0(Record& r) {
    r.string_id = 0xD08; r.style = 3; r.count = 4; r.frame = 4; r.active = 1;
}

// 1.13c D2Client+0x8BA30
void configure_8ba30(Record& r) {
    r.string_id = 0xD07; r.style = 2; r.count = 3; r.frame = 2; r.active = 1;
}

// 1.13c D2Client+0x8BA70
void configure_8ba70(Record& r) {
    r.string_id = 0xD0A; r.style = 4; r.count = 1; r.frame = 6; r.active = 1;
}

// 1.13c D2Client+0x8BAB0
void configure_8bab0(Record& r) {
    r.string_id = 0x276F; r.style = 1; r.count = 8; r.frame = 0x12; r.active = 1;
}

// Bounds are consecutive signed dwords in retail. Their addresses below identify
// the source globals; callers supply the corresponding state, without callbacks.
// 1.13c D2Client+0x8C440
std::int32_t contains_8c440(const Bounds& b, std::int32_t x, std::int32_t y) {
    // D2Client+0x10B3CC..0x10B3D8; callers: 0x958A0.
    return b.left <= x && x < b.right && b.top <= y && y <= b.bottom;
}

// 1.13c D2Client+0x8C520
std::int32_t contains_8c520(const Bounds& b, std::int32_t x, std::int32_t y) {
    // D2Client+0x10B28C..0x10B298; callers: 0x958A0. Left edge is strict.
    return b.left < x && x < b.right && b.top <= y && y <= b.bottom;
}

// 1.13c D2Client+0x8C580
std::int32_t contains_8c580(const Bounds& b, std::int32_t x, std::int32_t y) {
    // D2Client+0x10B2F8..0x10B304; callers: 0x958A0, 0x98DC0.
    return b.left <= x && x < b.right && b.top <= y && y <= b.bottom;
}
}
