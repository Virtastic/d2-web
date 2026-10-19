#pragma once
#include <cstddef>
#include <cstdint>

namespace d2::client::ui_record_bounds {
// Retail array starts at D2Client+0x10B238. It contains no native pointers.
#pragma pack(push, 1)
struct Record {
    std::uint32_t active;
    std::uint32_t preserved;
    std::uint16_t string_id;
    std::uint32_t style;
    std::uint16_t frame;
    std::uint32_t count;
};
#pragma pack(pop)
static_assert(sizeof(Record) == 20);
static_assert(offsetof(Record, active) == 0);
static_assert(offsetof(Record, preserved) == 4);
static_assert(offsetof(Record, string_id) == 8);
static_assert(offsetof(Record, style) == 10);
static_assert(offsetof(Record, frame) == 14);
static_assert(offsetof(Record, count) == 16);

// Pass the selected record by native reference. Retail selects it using EAX*20;
// the owning caller keeps its array in native memory rather than a 32-bit address.
void configure_8b9c0(Record& record);
void configure_8b9f0(Record& record);
void configure_8ba30(Record& record);
void configure_8ba70(Record& record);
void configure_8bab0(Record& record);

struct Bounds {
    std::int32_t left, right, top, bottom;
};
static_assert(sizeof(Bounds) == 16);
static_assert(offsetof(Bounds, left) == 0);
static_assert(offsetof(Bounds, right) == 4);
static_assert(offsetof(Bounds, top) == 8);
static_assert(offsetof(Bounds, bottom) == 12);
// Original internal ABI: x in EAX, y in ECX, result in EAX.
std::int32_t contains_8c440(const Bounds&, std::int32_t x, std::int32_t y);
std::int32_t contains_8c520(const Bounds&, std::int32_t x, std::int32_t y);
std::int32_t contains_8c580(const Bounds&, std::int32_t x, std::int32_t y);
}
