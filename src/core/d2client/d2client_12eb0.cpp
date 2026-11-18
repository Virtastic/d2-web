// 1.13c D2Client+0x12EB0 (358 B, caller 0x130B0): the UI reset. Every callee and the two imports go through the Env; the globals ride in `g`.
//   gate (0x11C540) := 1; if 0x42090() or 0x420E0() is nonzero: 0x3CC60(eax=1, fog(), 0xD0774)   [stack: fog result, data pointer; ret 8]
//   0x3CC60(eax=0xD, fog(), 0xD06D8); 0x12DD0(0) (ret 4); ebx = fog(); ebx == 0 -> string 0x179, exit (the fatal path).
//   for the six 12-byte records at 0xF8BF0: 0x3CA80(eax = ebx, esi = rec[0], ecx = rec[1], edx = rec[2]).
//   sel = *0x11C3CC, mode = *0x119898, sub = *0x11988C, sub2 = *0x119890:
//   sel != 0 and (mode == 1, or mode == 2 and sub in {4,5}) -> 0x3CCC0(1, fog(), 0xF8C38)   [each call pushes the data pointer, then the fog result]
//   sel != 0 and mode == 2 and sub == 5 -> 0x3CCC0(1, fog(), 0xF8C44)
//   GetSystemMetrics(0x4B) is called whatever sel is; != 0 and sel != 0 and (mode == 1 ? sub2 != 0 : mode == 2 and sub in {4,5}) -> 0x3CCC0(1, fog(), 0xF8C50).
#include <cstdint>
#include <emscripten.h>

struct D2Client12eb0Globals { uint32_t gate; uint32_t ids[6][3]; uint32_t sel, mode, sub, sub2; };   // gate 0x11C540, records 0xF8BF0.., 0x11C3CC, 0x119898, 0x11988C, 0x119890
struct D2Client12eb0Env {
    void* ctx;
    uint32_t (*pred_42090)(void*);
    uint32_t (*pred_420e0)(void*);
    uint32_t (*fog_10048)(void*);                                       // Fog ordinal 10048 through the thunk 0xD0B4
    void (*draw_3cc60)(void*, uint32_t eax, uint32_t fog, uint32_t data_va);
    void (*draw_3ccc0)(void*, uint32_t eax, uint32_t fog, uint32_t data_va);
    void (*close_12dd0)(void*, uint32_t arg);
    void (*feed_3ca80)(void*, uint32_t eax, uint32_t esi, uint32_t ecx, uint32_t edx);
    int32_t (*metrics)(void*, int32_t index);                           // user32 GetSystemMetrics
    void (*fatal)(void*, uint32_t strId);                               // string 0x179, then _exit(-1)
    uint32_t base;                                                      // the module's load address (data pointers are absolute)
};

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_12eb0(D2Client12eb0Globals* g, const D2Client12eb0Env* e) {
    void* c = e->ctx; const uint32_t b = e->base;
    g->gate = 1;
    if (e->pred_42090(c) != 0 || e->pred_420e0(c) != 0) e->draw_3cc60(c, 1, e->fog_10048(c), b + 0xD0774);
    e->draw_3cc60(c, 0xD, e->fog_10048(c), b + 0xD06D8);
    e->close_12dd0(c, 0);
    const uint32_t fog = e->fog_10048(c);
    if (fog == 0) { e->fatal(c, 0x179); return; }
    for (int i = 0; i < 6; ++i) e->feed_3ca80(c, fog, g->ids[i][0], g->ids[i][1], g->ids[i][2]);
    if (g->sel != 0) {
        if (g->mode == 1 || (g->mode == 2 && (g->sub == 4 || g->sub == 5))) e->draw_3ccc0(c, 1, e->fog_10048(c), b + 0xF8C38);
        if (g->mode == 2 && g->sub == 5) e->draw_3ccc0(c, 1, e->fog_10048(c), b + 0xF8C44);
    }
    if (e->metrics(c, 0x4B) == 0 || g->sel == 0) return;
    if (g->mode == 1) { if (g->sub2 == 0) return; }
    else { if (g->mode != 2) return; if (g->sub != 4 && g->sub != 5) return; }
    e->draw_3ccc0(c, 1, e->fog_10048(c), b + 0xF8C50);
}
