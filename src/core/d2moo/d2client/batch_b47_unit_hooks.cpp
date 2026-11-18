// D2Client batch b47-d2client-k1: unit state hooks. Callees go through the D2cHost seam; the hidden
// register arguments of the decompile (the unit in EAX, passed on to the callees) are put in eax and ecx.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
typedef int32_t i32; typedef uint32_t u32;
namespace {
inline u32 W(const void* p, u32 w) { u32 v; memcpy(&v, (const char*)p + w * 4, 4); return v; }
inline void SW(void* p, u32 w, u32 v) { memcpy((char*)p + w * 4, &v, 4); }
inline u32 go(D2cHost h, u32 rva, u32 eax, u32 ecx, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0) {
    D2cCall c = {}; c.rva = rva; c.eax = eax; c.ecx = ecx; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3;
    return h(&c);
}
inline u32 ord(D2cHost h, u32 n, u32 unit, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0) {
    return go(h, 0x80000000u | n, unit, unit, s0, s1, s2, s3);   // D2Common/D2Game ordinal, rva = 0x80000000|ordinal
}
// sgptDataTables record: count at +cntOff, table at +tblOff.
inline const uint8_t* rec(const uint8_t* dt, u32 cntOff, u32 tblOff, u32 size, i32 i) {
    if (i < 0 || i >= (i32)W(dt, cntOff / 4)) return nullptr;
    return (const uint8_t*)(uintptr_t)W(dt, tblOff / 4) + (u32)i * size;
}
}  // namespace

extern "C" {
// 1.13c D2Client+0x4DA60  (EAX = unit)
EMSCRIPTEN_KEEPALIVE void d2c_4da60(D2cHost h, uint8_t* unit, const uint8_t* dt) {
    if (!unit || W(unit, 0) != 1) return;
    u32 type = W(unit, 4);
    if (type > 0xf) return;
    i32 id = (i32)go(h, 0x1700, (u32)(uintptr_t)unit, (u32)(uintptr_t)unit);
    if (id < 0 || id >= (i32)W(dt, 0xa80 / 4)) id = -1;
    u32 U = (u32)(uintptr_t)unit;
    if (type == 0xd && id != 0x4e) {
        u32 p = W(unit, 0xb);
        if (!p) return;
        ord(h, 10770, p); ord(h, 10559, p, 0); ord(h, 10498, p);
        go(h, 0x2A6C0, U, U);
        return;
    }
    if (go(h, 0xA5F70, U, U)) go(h, 0xA5FB0, U, U);
    u32 r = go(h, 0x1230, U, U);
    if (!r) return;
    bool skip = false, tail = true;   // skip: return at once; tail: the 0x15/0xd flag test
    if (*(const char*)(uintptr_t)(r + 0x1a7) != 0) {
        switch (type) {
        case 2: skip = (id == 0xe7 || id == 0x76); break;
        case 4:
            if (id != 0x193 && id != 0x194) {
                if (id == 0x1f1) { go(h, 0x4DA20, *(const char*)(uintptr_t)(r + 0x1a7), 0); go(h, 0x4CDD0, U, U); }
            } else { go(h, 0x2A6C0, U, U); ord(h, 10595, U); return; }
            break;
        case 5: case 9: case 8:
            if (type == 8 && id == 0x6e) return;
            if (id == 0x193 || id == 0x194) { go(h, 0x2A6C0, U, U); ord(h, 10595, U); return; }
            break;
        case 10:
            if (id == 0x88) { go(h, 0x2A6C0, U, U); ord(h, 10595, U); return; }
            break;
        case 0xb: skip = id == 0x88; break;
        case 0xc: skip = id == 0x11c; break;
        case 0xe: skip = id == 0xf7; break;
        default: break;
        }
        if (skip) return;
    }
    if (tail && (((W(unit, 0x31) >> 0x15) & 1) != 0 || ((W(unit, 0x32) >> 0xd) & 1) == 0)) {
        go(h, 0x2A6C0, U, U); ord(h, 10595, U);
    }
}

// 1.13c D2Client+0x4F0B0  (EAX = unit)
EMSCRIPTEN_KEEPALIVE void d2c_4f0b0(D2cHost h, uint8_t* unit) {
    if (!unit || W(unit, 0) != 1) return;
    u32 U = (u32)(uintptr_t)unit;
    i32 idx = (i32)ord(h, 10362, U);
    if (idx == -1) return;
    u32 q[23] = {};
    q[0] = 0x20;
    if (idx > 0x3e && idx < 0x43) { q[0] = 0x34; q[10] = 2; }
    q[12] = 1; q[4] = (u32)idx;
    u32 p = go(h, 0xA5F70, U, U);
    if (!p) {
        u32 u11 = W(unit, 0xb);
        if (!u11) return;
        q[6] = ord(h, 10983, u11); q[8] = ord(h, 10764, u11);
    } else {
        q[7] = go(h, 0x1630, p, p); q[8] = go(h, 0x1660, p, p);
    }
    if (idx == 0xee || idx == 0xef) {
        if (p) {
            u32 st[2] = {0, U};
            bool ok = W(unit, 0) == 0 || W(unit, 0) == 1;
            if (!ok) { st[1] = go(h, 0xA60D0, 0, 0); ok = st[1] != 0; }
            if (ok) go(h, 0xA1340, 0, 0, (u32)(uintptr_t)st);
        }
    } else if (idx == 0x196) {
        go(h, 0x4DA20, 0, 0); go(h, 0x4CDD0, 0, 0);
    } else {
        go(h, 0xB30F0, 0, 0, (u32)(uintptr_t)q);
    }
    go(h, 0x4C7C0, (u32)idx, (u32)idx);
}

// 1.13c D2Client+0x4F2C0  (EAX = unit)
EMSCRIPTEN_KEEPALIVE void d2c_4f2c0(D2cHost h, uint8_t* unit, const uint8_t* dt) {
    if (!unit || W(unit, 0) != 1) return;
    u32 U = (u32)(uintptr_t)unit;
    u32 s = ord(h, 10511, U);
    if (!s) return;
    i32 mi = (i32)ord(h, 10601, s, 0, 0x3b4);        // (.., "..\\Source\\D2Client\\UNIT\\Monster.cpp", 0x3b4)
    const uint8_t* mr = rec(dt, 0xba0, 0xb98, 0x23c, mi);
    if (!mr) return;
    u32 fl = ord(h, 10397, s);
    bool b3 = true;
    if ((fl & 1) && go(h, 0x2A700, U, U) != 0) {
        if (*(const int16_t*)(mr + 0x2e) != 0x43) ord(h, 10137, s, fl | 2);
        go(h, 0xA3000, U, U);
        b3 = false;
    }
    uint8_t c = unit[0x4e];
    if (c == 4 || (b3 && ((W(unit, 0x31) >> 6) & 1) == 0 && (c == 1 || c == 2 || c == 3))) {
        go(h, 0xA3000, U, U);
        return;
    }
    i32 sv = *(const int16_t*)(mr + 0x2e);
    u32 w11 = W(unit, 0x11);
    if (sv == 0x5f) {
        u32 r = go(h, 0x1260, U, U);
        if (r) {
            go(h, 0xA3000, U, U);
            if (*(const uint8_t*)(uintptr_t)(r + 0x109) < ((i32)w11 >> 8))
                SW(unit, 0x11, (u32)*(const uint8_t*)(uintptr_t)(r + 0x10a) << 8);
        }
    } else if (sv == 0x7e) {
        go(h, 0xA3000, U, U);
        if ((i32)(W(unit, 0x11) & 0xffffff00u) > 0xbff) SW(unit, 0x11, 0x800);
    } else if (sv == 0x98) {
        u32 r = go(h, 0x1260, U, U);
        if (r && ord(h, 10494, U)) {
            i32 v = (i32)W(unit, 0x11);
            if (*(const uint8_t*)(uintptr_t)(r + 0x109) <= (v >> 8)) go(h, 0xA3000, U, U);
            if (*(const uint8_t*)(uintptr_t)(r + 0x10a) <= (v >> 8)) {
                u32 u = go(h, 0x1330, U, U, (u32)(uintptr_t)(unit + 0x20));
                SW(unit, 0x11, (u - 4 + *(const uint8_t*)(uintptr_t)(r + 0x10a)) * 0x100);
            }
        }
    }
}
}
