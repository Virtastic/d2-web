// 1.13c D2Client+0x167D0: UI animation dispatcher. Globals and callees passed in.
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
struct UiAnim { uint32_t idx854, acc858, pad85c, tick860, state864; };  // 0x11B854..
static_assert(offsetof(UiAnim, tick860) == 0xC && offsetof(UiAnim, state864) == 0x10, "layout");
// tbl = 0xF8558, stride 0x1C, 7 entries; mode (0xF861C) = dword at tbl+0xC4. Returns 0, or 1 on the _exit(-1) paths.
extern "C" int d2_client_167d0(UiAnim* g, const uint8_t* tbl, uint32_t now, void (*c16250)(), void (*c164f0)()) {
    if (*(const uint32_t*)(tbl + 0xC4) != 7 || g->idx854 >= 7) return 1;
    if (*(const uint32_t*)(tbl + g->idx854 * 0x1c) != 0) { if (g->state864 == 3) c16250(); else c164f0(); }
    if (g->state864 == 1 && g->tick860 + 5000u < now) { g->state864 = 2; g->idx854 = 2; g->acc858 = 0; }
    return 0;
}
