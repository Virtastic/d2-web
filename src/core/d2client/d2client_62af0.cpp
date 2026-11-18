// Per-frame near-room refresh, no inputs (the caller's EBX is saved and restored, never read).
// [C204] counts down; otherwise the player ([BBFC], fatal when 0) position C1D2/C1F6 is compared to the last refresh ([D346C], [D3468]); a move of >= 0x50 (octagonal metric)
// refreshes: the player's room list is filtered to rooms on the same level, and each room gets the refresh call with the level's context;
// ctx stays in EBX across the loop, so EBX at each refresh call is ctx.
#include <cstdint>
typedef int32_t i32; typedef uint32_t u32;
struct D2c62af0G { u32 c204, bbfc, d3460, d3464, d3468, d346c; };   // D3460..D346C are consecutive in the image
struct D2c62af0Env {
    u32 (*c1d2)(u32 a); u32 (*c1f6)(u32 a); u32 (*c1f0)(u32 a); u32 (*c1de)(u32 a);
    void (*c3dc)(u32 room, u32* arr, u32* cnt);
    u32 (*cc3a)(u32 ecx);                // thiscall, returns a record pointer
    u32 (*f62710)(u32 eax);
    void (*f62580)(u32 room, u32 zero, u32 ctx);   // stdcall, ret 0xC
    void (*fatal)();                     // bf92(0x696), bf80(str, r), exit(-1)
};
static inline u32 r32(u32 a) { return *(const u32*)(uintptr_t)a; }
// 1.13c D2Client+0x62AF0
void d2c_62af0(D2c62af0G* g, const D2c62af0Env* e) {
    if (g->c204) { g->c204--; return; }
    if (!g->bbfc) { e->fatal(); return; }
    u32 x = e->c1d2(g->bbfc); g->d3464 = x;
    u32 y = e->c1f6(g->bbfc);
    g->d3460 = y;
    i32 dx = (i32)(g->d346c - x), dy = (i32)(g->d3468 - y);
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    i32 d = dx > dy ? (i32)((u32)dy + (u32)dx * 2) : (i32)((u32)dx + (u32)dy * 2);
    d = (d - (d >> 31)) >> 1;
    if (d < 0x50) return;
    g->d346c = x; g->d3468 = y;
    if (!g->bbfc) return;
    u32 room = e->c1f0(g->bbfc);
    if (!room) return;
    u32 arr = 0, cnt = 0;
    e->c3dc(room, &arr, &cnt);
    u32 lvl = r32(e->cc3a(e->c1de(room)) + 8);
    u32 ctx = e->f62710(lvl);
    for (u32 i = 0; i < cnt; i++) {
        u32 rm = r32(arr + i * 4);
        if (lvl == r32(e->cc3a(e->c1de(rm)) + 8)) e->f62580(rm, 0, ctx);
    }
}
