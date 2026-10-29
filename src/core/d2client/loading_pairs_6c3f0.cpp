// D24: the ecx,edx pair D2Client 0xC1760 (the loading-screen frame) is entered with, for each room slot of the 0x6D010 walk. The pair is the register leftover of the previous slot's last unit: a monster leaves what 1.13c D2Client+0x6C3F0 left (its
// last 0x6C020 call's ecx, with the low byte replaced by MonStats2 row[0xF1]; edx = 3), the player 0x6C3B0 leaves (4,8), anything else a pointer (0,0 here).
#include <cstdint>
#include <cstdio>
#include <map>
#include <string>
#include <emscripten.h>
extern "C" int32_t d2_monstats2_modeflags(int32_t classId);
namespace {
const int kList[15] = {12, 0, 1, 2, 15, 4, 3, 5, 7, 6, 13, 8, 9, 10, 11};   // D2Client .data 0xE28B0, count 15 at 0xE28EC
struct Cell { bool ready = false, warm = false; };
std::map<int, Cell> g_cells;   // (class, mode bit): the 0x6C020 per-cell table at class*0x28 + b (+0x14 = ready)
// 1.13c D2Client+0x6C3F0 for one unit of `cls`. Returns ecx. A warm cell (6C092 jne) leaves b+40*cls; otherwise the 0x6AC60 path (first visit: ready) or
// the D2Gfx 10055 path (second: returns 0, sets warm) leaves 5*cls.
uint32_t visit(int cls) {
    const int32_t fl = d2_monstats2_modeflags(cls);
    uint32_t last = 0;
    if (fl < 0) return 0;
    for (int b : kList) {
        if (!((fl >> b) & 1)) continue;
        Cell& c = g_cells[cls * 16 + b];
        if (c.warm) last = (uint32_t)(b + 40 * cls);
        else { last = (uint32_t)(5 * cls); if (c.ready) c.warm = true; else c.ready = true; }
    }
    return (last & ~0xFFu) | (uint32_t)((fl >> 8) & 0xFF);
}
std::string g_out;
}  // namespace
// mons: (class, subtile x, subtile y) per client monster in assign order; rects: the hero room's near list (tile x,y,w,h per slot); hero: subtile x,y.
// Returns "e:d" hex for walk entries ebx=1..8 (slot ebx-1's leftover), space separated.
extern "C" EMSCRIPTEN_KEEPALIVE const char* d2_client_loading_pairs(const int32_t* mons, int32_t nMons, const int32_t* rects, int32_t nRects, int32_t heroX, int32_t heroY) {
    g_cells.clear(); g_out.clear();
    auto inSlot = [&](int s, int32_t x, int32_t y) { if (s >= nRects) return false; const int32_t* r = rects + s * 4; x /= 5; y /= 5; return x >= r[0] && x < r[0] + r[2] && y >= r[1] && y < r[1] + r[3]; };
    for (int i = 0; i < nMons; ++i) visit(mons[i * 3]);   // the 0xAC assigns reach 0x6CB30 -> 0x6C3F0 once each, before the walk
    for (int s = 0; s < 8; ++s) {
        uint32_t e = 0, d = 0;
        if (inSlot(s, heroX, heroY)) { e = 4; d = 8; }
        else for (int i = 0; i < nMons; ++i) if (inSlot(s, mons[i * 3 + 1], mons[i * 3 + 2])) { e = visit(mons[i * 3]); d = 3; }
        char b[24]; std::snprintf(b, sizeof b, "%s%x:%x", s ? " " : "", e, d); g_out += b;
    }
    return g_out.c_str();
}
