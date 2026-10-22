// D2Client batch b59-cli-b: effect-entry tick/draw/spawn loops. Globals are passed as pointers (each names its RVA);
// callees (ordinals, sub_18D0) go through D2cHost.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
namespace {
inline u32 go(D2cHost h, u32 rva, u32 a = 0, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0, u32* o = nullptr) {
    D2cCall c = {}; c.rva = rva; c.eax = a; c.ecx = a; c.edx = a; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3;
    u32 r = h(&c); if (o) *o = c.out[0]; return r;
}
inline u32 ord(D2cHost h, u32 n, u32 a = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0, u32 s4 = 0) { return go(h, 0x80000000u | n, a, a, s1, s2, s3, nullptr) + 0 * s4; }
inline i32 rd(const void* p, u32 off) { i32 v; memcpy(&v, (const char*)p + off, 4); return v; }
inline void wr(void* p, u32 off, i32 v) { memcpy((char*)p + off, &v, 4); }
// shared body of 0x800D0 / 0x80160: obj = DAT_6fbcbf64 / DAT_6fbcbf60, frames = DAT_6fbc3880 / DAT_6fbc391c
void tick(D2cHost h, uint8_t* obj, const i32* frames, const i32* dx, const i32* dy) {
    i32 skip = (i32)ord(h, 10013);
    uint8_t* e = (uint8_t*)(uintptr_t)rd(obj, 0x11c);
    for (i32 i = 0; i <= rd(obj, 0x110); i++, e += 0x18) {
        if (*(int16_t*)e == 0) continue;
        if (skip == 0) { wr(e, 4, rd(e, 4) - *dx); wr(e, 8, rd(e, 8) - *dy); }
        i32 t = rd(e, 0x14) - 1; wr(e, 0x14, t);
        if (t < 1) {
            i32 f = rd(e, 0x10); wr(e, 0x10, f + 1);
            *(int16_t*)(e + 0x14) = 2; *(int16_t*)(e + 0x16) = 0;
            if (frames[rd(e, 0xc)] <= f + 1) ord(h, 10065);
        }
    }
}
}  // namespace

extern "C" {
// 1.13c D2Client+0x800D0
EMSCRIPTEN_KEEPALIVE void d2c_800d0(D2cHost h, uint8_t* obj /*1CBF64*/, const i32* frames /*1C3880*/, const i32* dx /*1CC408*/, const i32* dy /*1CC40C*/) { tick(h, obj, frames, dx, dy); }
// 1.13c D2Client+0x80160
EMSCRIPTEN_KEEPALIVE void d2c_80160(D2cHost h, uint8_t* obj /*1CBF60*/, const i32* frames /*1C391C*/, const i32* dx /*1CC408*/, const i32* dy /*1CC40C*/) { tick(h, obj, frames, dx, dy); }

// 1.13c D2Client+0x80450  (player = DAT_6fbcbbfc, flag = DAT_6fbcbf78)
EMSCRIPTEN_KEEPALIVE bool d2c_80450(D2cHost h, i32 flag, u32 player) {
    if (flag != 0) return false;
    bool r = false;
    if (player) {
        u32 a = ord(h, 10331, player);
        a = ord(h, 10826, a);
        r = (uint8_t)ord(h, 10215, a) != 0;
        ord(h, 10499, a);
    }
    return r;
}

// 1.13c D2Client+0x80940  (player = DAT_6fbcbbfc, obj = DAT_6fbcbf64; fatal assert path (null player) not modelled)
EMSCRIPTEN_KEEPALIVE void d2c_80940(D2cHost h, uint8_t* player, uint8_t* obj, i32 p1, i32 p2, i32 p3, i32 p4) {
    if (!player) return;
    uint8_t* seed = player + 0x20;
    i32 slot = (i32)ord(h, 10064);
    if (slot == -1) return;
    uint8_t* e = (uint8_t*)(uintptr_t)rd(obj, 0x11c) + (int16_t)((u32)slot >> 16) * 0x18;
    auto step = [&]() { uint64_t v = (uint64_t)(u32)rd(seed, 0) * 0x6ac690c5u + (u32)rd(player, 0x24); memcpy(seed, &v, 8); return (u32)v; };
    i32 y;
    if (ord(h, 10013) == 0) {
        u32 v = step();
        wr(e, 4, (i32)go(h, 0x18D0, (u32)(uintptr_t)seed) + p1);
        y = (i32)(v % 0x50) - 0x1e + p2;
    } else {
        wr(e, 4, ((i32)(step() % 5) - 2) * 0x10000 + p3);
        y = ((i32)(step() % 5) - 2) * 0x10000 + p4;
    }
    wr(e, 8, y); wr(e, 0xc, 2); wr(e, 0x10, 0); wr(e, 0x14, 2);
}

// 1.13c D2Client+0x7FC70  (g414 = DAT_6fbcc414, gW/gH = 1B8BC48/4C, tbl = param_2 palette/cel table)
EMSCRIPTEN_KEEPALIVE void d2c_7fc70(D2cHost h, const uint8_t* obj, const i32* tbl, i32 g414, i32 gW, i32 gH) {
    i32 skip = (i32)ord(h, 10013), maxY = gH - 0x2f, lo = 0, ox = 0, hiX = gW;
    if (g414 == 1) { i32 q = (gW + ((gW >> 31) & 3)) >> 2; ox = -q; hiX = gW - q * 2; }
    else if (g414 == 2) { ox = (gW + ((gW >> 31) & 3)) >> 2; lo = ox * 2; }
    uint8_t* e = (uint8_t*)(uintptr_t)rd(obj, 0x11c);
    for (i32 i = 0; i <= rd(obj, 0x110); i++, e += 0x18) {
        if (*(int16_t*)e == 0) continue;
        u32 rec[18] = {};   // auStack_48; [0]=+0x20 of the draw record is rec[0], palette/cel word at rec[13]
        if (skip == 0) {
            i32 y = rd(e, 8), x = rd(e, 4) + ox;
            if (lo <= x && x < hiX && y >= 0 && y < maxY) {
                rec[13] = (u32)tbl[rd(e, 0xc)]; rec[0] = (u32)rd(e, 0x10);
                ord(h, 10041, (u32)(uintptr_t)rec, (u32)x, (u32)y, 0xffffffffu, 3);
            }
        } else {
            bool done = false;
            if (ord(h, 10012, (u32)rd(e, 4), (u32)rd(e, 8))) {
                u32 sx = 0, sy = 0; D2cCall c = {}; c.rva = 0x80000000u | 10023; c.s[0] = (u32)rd(e, 4); c.s[1] = (u32)rd(e, 8);
                c.s[2] = 0; c.s[3] = 0; h(&c); sx = c.out[0]; sy = c.out[1];
                i32 x = (i32)sx + ox;
                if (lo <= x && x < hiX && (i32)sy >= 0 && (i32)sy < maxY) {
                    rec[13] = (u32)tbl[rd(e, 0xc)]; rec[0] = (u32)rd(e, 0x10);
                    done = ord(h, 10041, (u32)(uintptr_t)rec, (u32)x, sy, 0xffffffffu, 3) != 0;
                }
            }
            if (!done) ord(h, 10065);
        }
    }
}
}
