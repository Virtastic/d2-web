// 1.13c D2Game+0x69970 -- dispatch a unit through the per-mode handler table.
// ECX = game, EDX = unit; plain ret (the table[2] branches tail-jump). level = #A556(#A478(unit)); mode = byte [[unit+0x14]+4] & 0x7f.
// Table at RVA 0x107B8C, count (signed) at 0x107BB4. A mode at or past the count does nothing. Mode 8 at level >= 0x4B, mode 3 at
// level < 0x28 except 0x19, and modes 1|4 at level < 0x28 run table[2] (RVA 0x107B94) instead of table[mode]. A bad handler pointer is fatal.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game69970Env {
    void     (*fatal)(uint32_t line);
    uint32_t (*unitRoom)(uint32_t unit);
    uint32_t (*roomLevel)(uint32_t room);
    uint32_t (*isBadCodePtr)(uint32_t fn);
    void     (*call)(uint32_t fn, uint32_t ecx, uint32_t edx);
};

static inline uint32_t r32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

// 1.13c D2Game+0x69970
void D2Game_69970(uint32_t game, uint32_t unit, uint32_t tbl, int32_t count, const D2Game69970Env& e)
{
    const int32_t lvl = (int32_t)e.roomLevel(e.unitRoom(unit));
    const uint32_t mode = r32(r32(unit + 0x14) + 4) & 0x7f;
    if ((int32_t)mode >= count) return;
    uint32_t line = 0x17b, fn = r32(tbl + mode * 4);
    if (mode == 8 && lvl >= 0x4b) { line = 0x161; fn = r32(tbl + 8); }
    else if (mode == 3 && lvl < 0x28 && lvl != 0x19) { line = 0x16c; fn = r32(tbl + 8); }
    else if ((mode == 1 || mode == 4) && lvl < 0x28) { line = 0x176; fn = r32(tbl + 8); }
    if (e.isBadCodePtr(fn)) { e.fatal(line); return; }
    e.call(fn, game, unit);
}
