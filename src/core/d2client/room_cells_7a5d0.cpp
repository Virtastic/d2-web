// Callees go through D2cHost host in retail's call order (D2cCall: eax/ecx/edx/esi/s[], results in return and out[]).
// Seam ids are the import thunks: 0xCB20 #11124 room rect (s0=room, out[0..3]=x0,y0,x1,y1), 0xCB0E #10388 / 0xCB26 #10781 cell lists
// (s0=room; ret=list of 0x30-byte entries, out[0]=count), 0xC106 D2CMP#10083 (s0=cel ref; ret ptr, u16 at +4 = height),
// 0xCB2C #10512 world->tile (s0=x,s1=y; out[0],out[1]=tile, out[2]=screen-y slack), 0xCB08 #10667 first unit, 0xC1D2 #10651 unit x,
// 0xC1F6 #11142 unit y, 0xA6180 predicate, 0x7A040 (ecx=square,s0=tx,s1=ty), 0x79C80/0x79D30/0x79E20/0x7A340 (ecx=square,edx=cell,s0=room),
// 0x7A500 (ecx=square), 0x7A180/0x7A260 (eax=unit,edx=cell). Decompile register recovery is partial: 
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
namespace {
inline u32 rd(u32 a) { return *(const u32*)(uintptr_t)a; }
inline u32 go(D2cHost h, D2cCall& c, u32 rva) { c.rva = rva; return h(&c); }
inline u32 call(D2cHost h, u32 rva, u32 eax = 0, u32 ecx = 0, u32 edx = 0, u32 s0 = 0, u32 s1 = 0, D2cCall* o = nullptr) {
    D2cCall c = {}; c.eax = eax; c.ecx = ecx; c.edx = edx; c.s[0] = s0; c.s[1] = s1;
    u32 r = go(h, c, rva); if (o) *o = c; return r;
}
}
extern "C" {
// 1.13c D2Client+0x7A5D0
// ctx = param_2: [0]=env base (grid origin at +0xEAA0/+0xEAA4, cell array +0xEAA8, stride +0xEAB0, square at +0x38),
// [1..4]=view left/top/right/bottom, [6]=skip-units flag. Returns 1.
EMSCRIPTEN_KEEPALIVE u32 d2c_7a5d0(D2cHost h, u32 room, const i32* ctx) {
    const u32 env = (u32)ctx[0], sq = env + 0x38;
    const i32 gx = (i32)rd(env + 0xeaa0), gy = (i32)rd(env + 0xeaa4), stride = (i32)rd(env + 0xeab0);
    const u32 cells = rd(env + 0xeaa8);
    D2cCall o;
    call(h, 0xCB20, 0, 0, 0, room, 0, &o);
    const i32 rx0 = (i32)o.out[0], ry0 = (i32)o.out[1], rx1 = (i32)o.out[2], ry1 = (i32)o.out[3];
    if (ctx[3] < rx0 || ctx[4] + 400 < ry0 || rx1 < ctx[1] - 200 || ry1 < ctx[2]) return 1;
    for (int pass = 0; pass < 2; pass++) {
        u32 n = 0; D2cCall c;
        u32 e = call(h, pass ? 0xCB26 : 0xCB0E, 0, 0, 0, room, 0, &c); n = c.out[0];
        for (u32 i = 0; i < n; i++, e += 0x30) {
            u32 info = call(h, 0xC106, 0, 0, 0, rd(e + 0x18));
            i32 ex = (i32)rd(e), ey = (i32)rd(e + 4);
            call(h, 0xCB2C, 0, 0, 0, ex, ey, &c);
            i32 tx = (i32)c.out[0], ty = (i32)c.out[1], dy = (i32)c.out[2];
            i32 top = ey - (i32)*(const uint16_t*)(uintptr_t)(info + 4);
            if (!(ex <= ctx[3] && ctx[1] <= ex + 0xa0)) continue;
            if (pass == 0 ? !(top + dy <= ctx[4] && ctx[2] <= top - dy) : !(ey + dy <= ctx[4] && ctx[2] <= ey - dy)) continue;
            if (!call(h, 0x7A040, 0, sq, 0, (u32)tx, (u32)ty)) continue;
            u32 cell = cells + (u32)((stride * (ty - gy) + (tx - gx)) * 0x24);
            if (pass == 1) { call(h, 0x79C80, 0, cell, 0, room); continue; }
            *(u32*)(uintptr_t)cell = 0;
            u32 fl = rd(e + 0x14), t = rd(e + 0x1c);
            if ((fl & 0x1c000) == 0) call(h, 0x79C80, 0, cell, 0, room);
            else if (t == 0xf) call(h, 0x79D30, 0, sq, cell, room);
            else if (t < 0x10 || t > 0x13) call(h, 0x7A340, 0, sq, cell, room);
            else call(h, 0x79E20, 0, sq, cell, room);
        }
    }
    if (ctx[6] != 0) return 1;
    for (u32 u = call(h, 0xCB08, 0, 0, 0, room); u; u = rd(u + 0xe8)) {
        D2cCall c;
        i32 x = (i32)call(h, 0xC1D2, 0, 0, 0, u), y = (i32)call(h, 0xC1F6, 0, 0, 0, u);
        call(h, 0xCB2C, 0, 0, 0, x, y, &c);
        i32 tx = (i32)c.out[0] - gx, ty = (i32)c.out[1] - gy;
        if (tx < 0 || ty < 0 || tx >= stride || ty >= stride) continue;
        u32 cell = cells + (u32)((stride * ty + tx) * 0x24);
        if (call(h, 0xA6180) || (rd(u + 0xc4) >> 20 & 1)) { call(h, 0x7A260, u, 0, cell); continue; }
        call(h, 0x7A500, 0, sq);
        if (rd(u + 200) >> 7 & 1) call(h, 0x7A180, u, 0, cell);
    }
    return 1;
}
}
