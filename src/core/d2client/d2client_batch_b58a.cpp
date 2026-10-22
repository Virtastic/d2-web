// D2Client batch b58a: env-seam ports (callees/globals injected, no retail data).
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32; typedef uint8_t u8;

static inline i32 rd32(const void* p, u32 off) { i32 v; memcpy(&v, (const u8*)p + off, 4); return v; }
static inline int16_t rd16(const void* p, u32 off) { int16_t v; memcpy(&v, (const u8*)p + off, 2); return v; }
static inline const u8* rec3c(const u8* tbl, i32 cnt, i32 i) {
    return (!tbl || i < 0 || i >= cnt) ? nullptr : tbl + i * 0x3c;
}

extern "C" {
// 1.13c D2Client+0x6DCA0
EMSCRIPTEN_KEEPALIVE void d2c_6dca0(i32 p1, i32* p4, u32* p5, i32* p6, i32* p7, i32 p8, i32 p9,
    char (*f6d960)(i32), void (*f6d220)(i32), void (*f6d590)(u32), i32 (*ord10020)(u32, i32)) {
    if (*p5 > 0x3f) { *p7 = 1; *p6 = 0; return; }
    char c = f6d960(p1);
    *p7 = (i32)c;
    if (p9 != 0) { f6d220(*p7); f6d590(*p5); }
    *p4 = ord10020(*p5, p8);
}

// 1.13c D2Client+0x72080  (esi = unit)
EMSCRIPTEN_KEEPALIVE i32 d2c_72080(i32 unit, i32 (*ord10787)(), i32 g_bbfc, i32 g_97f8, i32 g_985c) {
    i32 r = ord10787();
    if (r == 0) {
        if (unit == g_bbfc && g_97f8 < g_985c) return 8;
        r = 0;
    }
    return r;
}

// 1.13c D2Client+0x720B0  (eax = idx, ecx = param_1); tbl/cnt = sgptDataTables +0xbc/+0xc4
EMSCRIPTEN_KEEPALIVE void d2c_720b0(i32 idx, i32 p1, const u8* tbl, i32 cnt, void (*const* hooks)(),
    i32 (*ord10871)(i32), void (*f21440)(i32)) {
    if (!p1 || idx < 0 || idx >= cnt) return;
    const u8* r = rec3c(tbl, cnt, idx);
    if (!r) return;
    int16_t s = rd16(r, 0x1c);
    if (s >= 0 && s < 0x1e && hooks[s]) hooks[s]();
    if (rd16(r, 0x30) >= 0 && ord10871(p1) != 0) f21440(idx);
}

// 1.13c D2Client+0x72130
EMSCRIPTEN_KEEPALIVE void d2c_72130(u8* unit, const u8* tbl, i32 cnt, i32 g_bbfc,
    i32 (*ord10494)(void*, i32), i32 (*ord10077)(), u8* (*getp64)(void*)) {
    i32 best = -1; u8 bestv = 0;
    for (i32 i = 0; i < cnt; i++) {
        const u8* r = rec3c(tbl, cnt, i);
        if (r && ord10494(unit, i) != 0 && bestv < r[0x20]) { best = i; bestv = r[0x20]; }
    }
    bool self = (i32)(intptr_t)unit == g_bbfc;
    u8* p = getp64(unit);
    if (best > 0) {
        const u8* r = rec3c(tbl, cnt, best);
        if (!r) return;
        i32 v = (r[0x21] == 'h' && self && ord10077() != 0) ? 0 : r[0x21];
        memcpy(unit + 0x6c, &v, 4);
        if (!self || !p) return;
        p[0x25] = r[0x22]; p[0x26] = r[0x23]; p[0x27] = r[0x24];
        return;
    }
    i32 z = 0; memcpy(unit + 0x6c, &z, 4);
    if (self && p) { p[0x25] = 0xff; p[0x26] = 0xff; p[0x27] = 0xff; }
}

// 1.13c D2Client+0x72530  (eax = unit dwords, param_1 = idx); mask28 = gdwBitMasks[0x1c]
EMSCRIPTEN_KEEPALIVE void d2c_72530(const i32* unit, i32 idx, const u8* tbl, i32 cnt, u8 mask28,
    void (*const* hooks)(), i32 (*ord10871)(), i32 (*ord10680)(i32, i32, i32), void (*f50840)(i32, i32, i32)) {
    const u8* r = unit ? rec3c(tbl, cnt, idx) : nullptr;
    if (!r) return;
    if ((mask28 & r[0x14]) != 0 && ((unit[0] == 1 && unit[4] == 0xc) || (unit[0] == 0 && unit[4] == 0x11))) return;
    int16_t s = rd16(r, 0x1a);
    if (s >= 0 && s < 0x1f && hooks[s]) hooks[s]();
    if (rd16(r, 0x30) >= 0) {
        i32 u = ord10871();
        if (u != 0) { i32 a = ord10680(u, 0x15e, 0), b = ord10680(u, 0x15f, 0); f50840(rd16(r, 0x30), a, b); }
    }
}
}
