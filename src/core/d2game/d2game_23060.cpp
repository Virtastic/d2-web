// 1.13c D2Game+0x23060 -- class-id filter copy (EAX = source unit, ECX = destination unit, [esp+4] = mode, ret 4).
// Both units must be type 1 with a +0x14 data record; the 9-byte class-id lists live at record +0x1C. Mode 0 appends
// at the destination's NUL (scanning at most 9 bytes), any other mode starts at 0. Source ids are copied while the
// table count (sgptDataTables+0xC54) allows them and the 0x20-byte row's byte +7 is non-zero; the walk stops at 9
// positions, at the source NUL, at an out-of-range id or a null row.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game23060Env { uint32_t dataTables; };   // *sgptDataTables
static inline uint32_t g23060_u32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
static inline uint8_t* g23060_list(uint32_t unit) {
    if (unit == 0 || g23060_u32(unit, 0) != 1 || g23060_u32(unit, 0x14) == 0) return nullptr;
    return (uint8_t*)(uintptr_t)g23060_u32(unit, 0x14) + 0x1C;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_23060(uint32_t src, uint32_t dst, uint32_t mode, const D2Game23060Env* e)
{
    const uint8_t* s = g23060_list(src);
    uint8_t* d = g23060_list(dst);
    if (!s || !d) return;
    int32_t at = 0;
    if (mode == 0) {
        while (at < 9 && d[at] != 0) ++at;
    }
    if (at >= 9) return;
    uint8_t* w = d + at;
    while (*s) {
        const uint32_t id = *s;
        if ((int32_t)g23060_u32(e->dataTables, 0xC54) <= (int32_t)id) return;
        const uint32_t row = g23060_u32(e->dataTables, 0xC50) + id * 0x20;
        if (row == 0) return;
        if (*((const uint8_t*)(uintptr_t)row + 7) != 0) *w++ = (uint8_t)id;
        ++s;
        if (++at > 8) return;
    }
}
