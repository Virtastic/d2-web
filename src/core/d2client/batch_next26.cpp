// D2Client next-26: net-poll / title-text helpers. Globals are passed in (each names its RVA); callees go through D2cHost.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
namespace {
inline u32 go(D2cHost h, u32 rva, u32 a = 0, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = a; c.ecx = a; c.edx = a; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; return h(&c);
}
inline u32 ord(D2cHost h, u32 n, u32 a = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0) { return go(h, 0x80000000u | n, a, s1, s2, s3); }
}  // namespace

extern "C" {
// 1.13c D2Client+0x435D0  (g394 state=DAT_6fbcc394, g3a4/g3a0/g3a8/g39c flags, g9884/g37c = tick stamps)
EMSCRIPTEN_KEEPALIVE void d2c_435d0(D2cHost h, u32 now, i32 g394, i32 g3a0, i32 g3a4, i32 g3a8, i32 g39c, u32 g9884, u32 g37c) {
    if (g394 == 0 || g394 == 1) return;
    if (ord(h, 10004) == 0 && g3a4 == 0 && g3a0 != 0) go(h, 0x43550, 7);
    if (g3a8 != 0 && 180000u < now - g9884) go(h, 0x43550, 7);
    if (30000u < now - g37c && g3a0 == 0 && g3a4 == 0 && g3a8 == 0) go(h, 0x43550, 6);
    if (g39c == 0 && g3a0 == 0 && g3a4 == 0 && g3a8 == 0 && (g394 == 9 || g394 == 7)) go(h, 0x43550, 6);
}

// 1.13c D2Client+0x43710  (wide = DAT_6fbc9854, half = DAT_6fb8bc48 / 2)
EMSCRIPTEN_KEEPALIVE void d2c_43710(D2cHost h, i32 wide, i32 half) {
    // each line: s = 0x42C10(id, half/2) [stdcall]; o = 0xD1EC(ecx=id; y, 4, 0) [fastcall]; 0xD37E(ecx=o, edx=s)
    auto line = [&](u32 id, u32 y) {
        D2cCall c = {}; c.rva = 0x42C10; c.s[0] = id; c.s[1] = (u32)(half / 2); u32 s = h(&c);
        D2cCall d = {}; d.rva = 0xD1EC; d.ecx = id; d.s[0] = y; d.s[1] = 4; d.s[2] = 0; u32 o = h(&d);
        D2cCall e = {}; e.rva = 0xD37E; e.ecx = o; e.edx = s; h(&e);
    };
    line(0x13ea, wide ? 0x64 : 0x3e);
    if (!wide) line(0x13eb, 0x6b);
    line(0x13ec, wide ? 0x110 : 0xe8);
}

// 1.13c D2Client+0x437E0  (player = DAT_6fbcbbfc; null-record fatal path (_exit) returns 0xFFFFFFFF here)
EMSCRIPTEN_KEEPALIVE u32 d2c_437e0(D2cHost h, u32 player, const uint8_t* (*rec)(u32)) {
    // 0xC1F0 (Ordinal_10331), 0xC1DE (10826), 0xC214 (10014): stdcall, one stack word each
    auto one = [&](u32 rva, u32 v) { D2cCall c = {}; c.rva = rva; c.s[0] = v; return h(&c); };
    if (!player) return 0;
    u32 u = one(0xC1F0, player);
    if (!u) return 0;
    const uint8_t* r = rec(one(0xC214, one(0xC1DE, u)));
    if (!r) return 0xFFFFFFFFu;
    return *(const u32*)(r + 0x218);
}

// 1.13c D2Client+0x43830  (g3b8 = DAT_6fbcc3b8, player = DAT_6fbcbbfc; returns new DAT_6fbcc400 flag, -1 = unchanged)
EMSCRIPTEN_KEEPALIVE i32 d2c_43830(D2cHost h, u32 g3b8, u32 player) {
    if (!g3b8) return 1;
    D2cCall p = {}; p.rva = 0xC1F0; p.s[0] = player;
    u32 unit = player ? h(&p) : 0;
    D2cCall c = {}; c.rva = 0xC556; c.s[0] = g3b8; c.s[1] = unit; c.s[2] = 5; c.s[3] = 0; h(&c);   // 5th stack word (1) not logged
    return -1;
}

// 1.13c D2Client+0x43920  (returns new DAT_6fbcc39c: -1 unchanged, else 0/1)
EMSCRIPTEN_KEEPALIVE i32 d2c_43920(D2cHost h, uint8_t* buf /*512*/) {
    i32 flag = -1;
    for (;;) {
        i32 n = (i32)ord(h, 10020, (u32)(uintptr_t)buf, 0x200);
        if (n != -1) switch (buf[0]) {
            case 0xaf: flag = 1; break;
            case 0xb0: flag = 0; break;
            case 0xb3: go(h, 0xACD70); break;
            case 0xb4: go(h, 0xACBF0); break;
        }
        go(h, 0x80000000u | 0);  // Sleep(0) placeholder: host yield
        if (n == -1) break;
    }
    for (;;) {
        i32 n = (i32)ord(h, 10008, (u32)(uintptr_t)buf, 0x200);
        go(h, 0xACE20, (u32)n);
        if (n == -1) break;
    }
    return flag;
}
}
