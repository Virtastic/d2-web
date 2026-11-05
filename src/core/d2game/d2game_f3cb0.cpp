// 1.13c D2Game+0xF3CB0: the MonStats2 byte +0x10B (below 0x10) of a unit, else 1.
// Retail: EAX = flag, EDI = unit (plain ret). A zero flag returns 1. The row is 0x1320(unit ? unit+4 : -1); a null row
// returns 1. When the row's word +0x10C is nonzero and below the data tables' count at +0xBA0 (signed), it calls
// #10630 (thunk 0xA3F4, ECX = unit) and hands its result to #11103 (thunk 0xA496, stack (unit, result)).
#include <cstdint>
#include <cstring>

struct D2GameF3CB0Env {
    uint32_t (*row)(int32_t classId);              // 0x1320 MonStats2 row (0 = none)
    int32_t  (*dtCount)();                         // sgptDataTables +0xBA0
    uint32_t (*o10630)(uint32_t ecxUnit);          // 0xA3F4
    void     (*o11103)(uint32_t unit, uint32_t v); // 0xA496 (stdcall 2)
};

extern "C" uint32_t d2game_f3cb0_impl(uint32_t flag, uint32_t unit, const D2GameF3CB0Env* e)
{
    if (flag == 0) return 1;
    int32_t id = -1;
    if (unit) std::memcpy(&id, (const void*)(uintptr_t)(unit + 4), 4);
    const uint32_t row = e->row(id);
    if (row == 0) return 1;
    uint16_t w; std::memcpy(&w, (const void*)(uintptr_t)(row + 0x10C), 2);
    if (w != 0 && (int32_t)w < e->dtCount()) e->o11103(unit, e->o10630(unit));
    const uint8_t b = *(const uint8_t*)(uintptr_t)(row + 0x10B);
    return b < 0x10 ? b : 1;
}
