// 1.13c D2Client+0x16A90 -- the HUD centre-cel draw. No stack args, plain ret; EBX is the caller's
// and reaches the route handler unchanged. gl block = the HUD globals 0x11B824..0x11B867: +0 W (0x11B824), +4 X (0x11B828),
// +8 Y (0x11B82C), +0x10 the state object (0x11B834), +0x30 the route index (0x11B854), +0x40 the mode (0x11B864).
// onSlot = the address of the on flag 0x11C4F0, tbl = the route handler table 0xF856C (stride 0x1C, code pointer at +0).
// Off: nothing. State object null, its dword +0 not 4, or mode >= 6 (signed): call tbl[route] with EAX = the on flag (or the
// mode when only the mode failed), EDX = route * 0x1C. Otherwise a zeroed 0x48-byte record {byte +5 = 1} is filled by
// 0x6E2C0(state, &rec, &out) (EAX = -1) and opened by D2CMP #10005 (&rec, 0, 1); either returning 0 ends it. Cel = rec+0x3C
// (null: Fog #10265(0x275), #10024(file, that), _exit(-1)); w = D2CMP #10000(cel), h = D2CMP #10060(rec+0x3C), then
// 0x6B5A0(Y - h/2 + X, W - w/2) with EBX = the state object; both halvings are unsigned shifts.
#include <cstring>
#include "cl_env.h"
extern "C" void d2c_16a90(uint32_t ebx, uint32_t onSlot, uint32_t tbl, const cl::Env& e) {
    const uint32_t g = e.gl;
    const uint32_t on = e.R(onSlot);
    if (!on) return;
    const uint32_t st = e.R(g + 0x10);
    uint32_t eax = on;
    if (st && e.R(st) == 4) {
        const int32_t mode = (int32_t)e.R(g + 0x40);
        if (mode < 6) {
            uint32_t rec[18] = {}, out = 0, snap[18];
            ((uint8_t*)rec)[5] = 1;
            const uint32_t p = (uint32_t)(uintptr_t)rec, sp = (uint32_t)(uintptr_t)snap;
            memcpy(snap, rec, sizeof rec);
            const uint32_t ok = e.H(0x6e2c0, st, p, (uint32_t)(uintptr_t)&out, 0, 0, 0xffffffffu);
            if (e.blk) e.blk(e.ctx, 0x6e2c0, sp, 18);
            if (!ok) return;
            memcpy(snap, rec, sizeof rec);
            const uint32_t op = e.H(0xc0d0, p, 0, 1);
            if (e.blk) e.blk(e.ctx, 0xc0d0, sp, 18);
            if (!op) return;
            uint32_t cel = rec[15];
            if (!cel) {
                const uint32_t m = e.H(0xbf92, 0x275);
                e.H(0xbf80, 0xB20D359Fu, m);
                cel = e.H(0x3664, 0xffffffffu);
            }
            const uint32_t w = e.H(0xc0e2, cel);
            const uint32_t a = e.R(g + 0) - (w >> 1);
            const uint32_t h = e.H(0xc0d6, rec[15]);
            e.H(0x6b5a0, e.R(g + 8) - (h >> 1) + e.R(g + 4), a, 0, 0, 0, 0, 0, 0, e.R(g + 0x10));
            return;
        }
        eax = (uint32_t)mode;
    }
    const uint32_t d = e.R(g + 0x30) * 0x1cu;
    e.H(e.R(tbl + d), 0, 0, 0, 0, 0, eax, 0, d, ebx);
}
