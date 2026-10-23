// D2Client sub_A1CF0 (item/skill use-ish dispatcher, id -> 0x23c row). Callees go through D2cHost in retail call order;
// decompile stack args are partly unreliable, so unrecovered call args are left 0 (partial seam, no vector yet).
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
inline u32 ord(D2cHost h, u32 n, u32 a = 0, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0) { return go(h, 0x80000000u | n, a, s0, s1, s2); }
inline i32 rd(const uint8_t* p, u32 off, int n) { i32 v = 0; memcpy(&v, p + off, n); return n == 2 ? (int16_t)v : v; }
}  // namespace
extern "C" {
// 1.13c D2Client+0xA1CF0
// row = *(sgptDataTables+0xB98)+id*0x23c (null if id out of [0,*(dt+0xBA0))); player = in_EAX (unit, 0 ok); localPlayer = DAT_6fbcbbfc;
// masks = gdwBitMasks_exref (u32[]); info = unit-data record (local_24); bc0 = *(dt+0xBC0); nCbs/cbs = DAT_6fb8ea40/DAT_6fb8e928
EMSCRIPTEN_KEEPALIVE i32 d2c_a1cf0(D2cHost h, i32 id, u32 arg2, u32 player, u32 localPlayer, const uint8_t* row, const u32* masks,
                                   const uint8_t* info, i32 iStack20, u32 bc0, i32 nCbs, const u32* cbs, u32 playerType, u32 playerClass) {
    if (id < 0 || !row) return 0;
    if (row[399]) {
        u32 o = 0;
        if (go(h, 0xA1340, 0, 0, 0, 0, 0, &o)) {
            if (row[399] < 1 || row[399] > 5) return 0;
            if (!ord(h, 10479, player, o)) return 0;
        }
    }
    if (!go(h, 0xA11C0)) return 0;
    u32 unit = ord(h, 10511);
    if (!unit || (i32)ord(h, 10601, unit, 0x504) != id) {
        unit = ord(h, 10708, player, (u32)id);
        if (!unit) {
            ord(h, 10008, player, (u32)id);
            unit = ord(h, 10708);
            if (unit && player == localPlayer) ord(h, 10137, unit, 8);
            ord(h, 10302);
            if ((i32)go(h, 0x17B0) < (i32)arg2) ord(h, 11109);
            if (!unit) return 0;
        }
        ord(h, 11103, player, unit);
    }
    // id != -1 path: Ordinal_10537 fills local_28 / info; caller supplies info already resolved
    if (!ord(h, 10537, unit) ) return 0;
    u32 sub = 0;
    if (player == localPlayer) sub = ord(h, 10306, player, unit, 1);
    else { if ((i32)go(h, 0x17B0) < (i32)arg2) { ord(h, 10302, 0, 0x53a); ord(h, 11109); } else ord(h, 10302, 0, 0x53f, arg2); }
    if (go(h, 0xA5F70) && !(masks[8] & info[7]) && (uint8_t)go(h, 0xA5B90)) return 0;
    if (!(masks[0] & info[5])) {
        u32 t = ord(h, 10331);
        if (ord(h, 10057, t)) {
            ord(h, 11103);
            u32 z[4] = {0, 0, 0, 0}; go(h, 0xA2750, 7, (u32)(uintptr_t)z);
            if (player != localPlayer) return 0;
            go(h, 0xA3200, player, 0x18);
            return 0;
        }
    }
    u32 r;
    if (!player || rd((const uint8_t*)(uintptr_t)player, 0, 1) != 0 || id != 0) r = ord(h, 10952);
    else r = go(h, 0xA1260);
    if (r || ((!ord(h, 10494) || !(masks[0xc] & info[5])) && player && (playerType == 0 || playerType == 1))) go(h, 0xADA6C0);
    i32 s = rd(info, 0xf2, 2); i32 ret = 1;
    if (s >= 0 && s < nCbs && cbs[s]) {
        ret = (i32)go(h, cbs[s], sub);
        if (!ret) {
            ord(h, 11103);
            if (player && (playerType == 0 || playerType == 1)) go(h, 0xADA6C0);
            if (masks[0x10] & info[5]) return ret;
        }
    }
    i32 a = rd(info, 0xfc, 2);  // local_24+0x3f*4
    if (iStack20 != -1 && rd(info, 0x122, 2) > 0) a = rd(info, 0x122, 2);
    auto lvl = [&]() { ord(h, 10862); u32 v = go(h, 0x22220, player); return (masks[0x14] & info[5]) ? (u32)((i32)(v - 3) < 0 ? 0 : v - 3) : 0u; };
    if (a > 0) go(h, 0x88A70, player, lvl(), 0, 0);
    if (masks[0x18] & info[5]) {
        ord(h, 10862);
        if (go(h, 0x22280, player)) { ord(h, 10862); i32 v = (i32)go(h, 0x22220, player); go(h, 0x88A70, player, v < 0 ? 0 : v, 0, 0); }
    }
    if ((iStack20 == -1 || rd(info, 0x122, 2) < 1) && rd(info, 0xfe, 2) > 0 && player && playerType == 0 && playerClass == info[12])
        go(h, 0x88A70, player, lvl(), 0, 0);
    i32 b = rd(info, 0x106, 2);
    if (iStack20 != -1 && rd(info, 0x124, 2) > 0) b = rd(info, 0x124, 2);
    if (b > 0 && (u32)b < bc0 && !go(h, 0x69130, (u32)b)) go(h, 0x6B300, player, 2, 0, 0, 0);
    if (ret && rd(info, 0xe8, 2) > 0) go(h, 0xA0F60, player);
    return ret;
}
}
