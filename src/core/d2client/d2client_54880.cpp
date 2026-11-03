// 1.13c D2Client+0x54880 -- item property line for a table row. stdcall(unit, dest, rows): for every row of `rows` (stride 16, count 0x6FBA2128) whose id equals
// Ordinal_10620(unit) it reads stat 0x47 (arithmetic >> 8 for ids 6..0xB), and appends  tail + str(0xF9B) + [str(0xFA2) if > 0] + decimal + str(0xF9E)  to dest.
// tail = str(row.w8) when the value is positive, str(row.wA) when negative; a zero value skips the row. A row flag != 1 or a global-table id != 2 is a fatal exit (not modelled).
// Global table rows 0x6FBA20D4 (stride 16, [+0] must be 2), count dword at 0x6FBA2128 (= table + 0x54).
#include "d2c_wstr.h"
struct D2c54880Env { uint32_t (*str)(uint32_t id); int32_t (*want)(uint32_t unit); int32_t (*stat)(uint32_t unit); };
extern "C" void d2c_54880r(uint32_t unit, uint32_t dst, const uint8_t* rows, const uint8_t* gt, const D2c54880Env& e) {
    uint16_t tail[256] = {}, val[256] = {};
    const uint32_t sA = e.str(0xf9e), sB = e.str(0xf9b), sC = e.str(0xfa2);
    const int32_t want = e.want(unit);
    const uint32_t n = *(const uint32_t*)(gt + 0x54);
    for (uint32_t i = 0; i < n; ++i, rows += 16) {
        const int32_t id = *(const int32_t*)rows;
        if (id != want) continue;
        int32_t v = e.stat(unit);
        if (id == 8 || id == 9) v >>= 8;
        if (id == 6 || id == 7) v >>= 8;
        if (id == 10 || id == 11) v >>= 8;
        uint32_t t = 0;
        if (v > 0) t = e.str(*(const uint16_t*)(rows + 8)); else if (v < 0) t = e.str(*(const uint16_t*)(rows + 10));
        if (v == 0) continue;
        uint16_t num[10]; d2c::wdec(num, v);
        if (*(const int32_t*)(rows + 4) != 1) __builtin_trap();
        const uint32_t V = (uint32_t)(uintptr_t)val, N = (uint32_t)(uintptr_t)num, T = (uint32_t)(uintptr_t)tail;
        if (v < 1) d2c::wcpy(V, N); else { d2c::wcpy(V, sC); d2c::wcat(V, N); }
        if (*(const int32_t*)(gt + 16 * i) != 2) __builtin_trap();
        d2c::wcpy(T, t); d2c::wcat(T, sB); d2c::wcat(T, V); d2c::wcat(T, sA);
        d2c::wcat(dst, T);
    }
}
