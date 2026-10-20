// src/core/d2client/d2client_batch_a15c0.cpp -- twelve UNMAPPED 1.13c D2Client.dll rows (0xA15C0..0xA2F60,
// the unit-targeting / skill-cast-effect cluster) reimplemented from the decompiles in
// build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000.
//
// Every unrecovered callee, Fog/D2Common ordinal and data table is a host seam on one env, called in
// retail's call order after the retail arguments (the d2client_batch_bf80.cpp pattern). Several bodies
// take their arguments in registers (EAX/EBX/ESI/EDI unaffected-register reads in the decompile); those
// are named extra parameters here, in decompile order. Guest 32-bit pointers held inside structs are
// widened through uintptr_t, never cast to uint32_t.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2ClientBatchA15c0Env {
    // 0xA1340: fills out[0..1] (client context pair), returns nonzero on success.
    int32_t  (*getClientCtx)(int32_t* out);                              // 0xA1340
    uint32_t (*f60450)(int32_t a);                                       // 0xB0450
    uint32_t (*f630f0)(uint32_t* particle);                              // 0xB30F0
    void     (*f69a80)(uintptr_t, uint32_t, uint32_t, uint32_t, int32_t, int32_t, uintptr_t, uint32_t, uint32_t); // 0xB9A80
    uint32_t (*f515c0)();                                                // 0xA15C0
    void     (*fatal)(uint32_t line);                                    // ord10265+ord10024+_exit(-1), noreturn
    void     (*fda0b0)();                                                // 0x2A0B0
    // unit accessors
    int32_t  (*ord10331)(uintptr_t unit);                                // room of unit
    int32_t  (*ord10057)(int32_t a);
    int32_t  (*ord10239)();
    int32_t  (*ord10064)(int32_t a);
    int32_t  (*ord10278)();
    int32_t  (*ord10494)();
    int32_t  (*ord11138)(uintptr_t unit);
    int32_t  (*ord11076)(int32_t a, const uint32_t* p, const uint32_t* q, int32_t n);
    uint32_t (*f1630)(uintptr_t unit);                                   // 0x1630 x
    uint32_t (*f1660)(uintptr_t unit);                                   // 0x1660 y
    uintptr_t (*f1580)();                                                // 0x1580
    uintptr_t (*f1230)();                                                // 0x1230
    uint32_t (*f62a0)(uintptr_t unit);                                   // 0xA62A0
    int32_t  (*f50ee0)(uintptr_t unit);                                  // 0xA0EE0
    int32_t  (*f50750)(uint32_t room, uintptr_t ctx);                    // 0xA0750
    uint32_t (*f1760)(uint32_t ecx, uint32_t id);                        // 0x1760
    uint32_t gdwBitMask0, gdwBitMask4, gdwBitMask0x1c;                   // gdwBitMasks[0], [4], [0x1c]
    int32_t  (*ord10383)(int32_t room, uint32_t** list, uint32_t* count);
    int32_t  (*ord10750)(uint32_t p);
    int32_t  (*ord10867)(uint32_t p);
    int32_t  (*ord10769)(int32_t a, int32_t b, int32_t c, int32_t d);
    int32_t  (*scanCb)(uintptr_t unit, uintptr_t ctx);                   // (*param_2)() in 0xA1860
    // data tables
    uint32_t (*dtI32)(uint32_t off);                                       // *(sgptDataTables) + off, i32
    uint8_t* (*dtPtr)(uint32_t off);                                     // pointer field at that offset
    int32_t  (*rodata)(uint32_t addr);                                   // i32 at 0x6fb84xxx
    int32_t  (*handlerCount)();                                          // DAT_6fb8ec50
    int32_t  (*callHandler)(int32_t idx, int32_t arg);                   // DAT_6fb8ea48[idx]; -1 if null
    int32_t  (*ord10286)(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    int32_t  (*ord11129)();
    uint32_t (*ord10392)(uint32_t a);
    int32_t  (*f38a70)(uintptr_t u, int, int, int);                      // 0x88A70
    uintptr_t (*f55f70)();                                               // 0xA5F70
    int32_t  (*f1b300)(uintptr_t u, int, int, int, int);                 // 0x6B300
    int32_t  (*ord10889)(uintptr_t unit);
    int32_t  (*ord10709)(uintptr_t unit, uintptr_t arg);
    int32_t  (*ord10871)();
    int32_t  (*ord11013)(int, int, int, uint32_t, uint32_t);
    void     (*ord10785)(int32_t, int32_t);
    int32_t  (*ord10807)();
    void     (*ord10188)(int32_t, int32_t, int32_t, int32_t);
    int32_t  (*ord10641)(uintptr_t unit);
    uint32_t (*ord10321)(int32_t a);
    int32_t  (*ord11088)(int32_t a);
    int32_t  (*ord10614)();
    uint32_t cb50650;                                                    // LAB_6fb50650
    uintptr_t cb509f0, cb50740;                                          // scan filters passed by 0xA1A50/0xA1AD0
};
typedef D2ClientBatchA15c0Env Env;

static inline uint32_t rd32(uintptr_t b, uint32_t o) { return *(const uint32_t*)(b + o); }
static inline void     wr32(uintptr_t b, uint32_t o, uint32_t v) { *(uint32_t*)(b + o) = v; }
static inline int16_t  rd16(const uint8_t* b, uint32_t o) { return *(const int16_t*)(b + o); }

// Retail's 64-bit LCG, state in two dwords at +0x20 (lo) / +0x24 (hi): s = lo*0x6AC690C5 + hi.
static uint32_t seedRoll(uintptr_t h, uint32_t n) {
    if ((int32_t)n < 1) return 0;
    uint64_t s = (uint64_t)rd32(h, 0x20) * 0x6AC690C5ull + rd32(h, 0x24);
    wr32(h, 0x20, (uint32_t)s); wr32(h, 0x24, (uint32_t)(s >> 32));
    return (n & (n - 1)) == 0 ? ((n - 1) & (uint32_t)s) : (uint32_t)s % n;
}

// unit width/height pair used by 0xA1860 and 0xA27B0: types 2,4,5 read the info record directly.
static void unitSize(const Env* e, uintptr_t u, int32_t* w, int32_t* h) {
    int32_t t = (int32_t)rd32(u, 0); uint32_t info = rd32(u, 0x2C);
    bool direct = t == 2 || (t > 3 && t < 6);
    *w = direct ? (int32_t)rd32(info, 0x10) : (info ? e->ord10750(info) : 0);
    *h = direct ? (int32_t)rd32(info, 0x0C) : (info ? e->ord10867(info) : 0);
}

// 1.13c D2Client+0xA15C0 (86 B): the player-owned context object, or 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_a15c0(const Env* e) {
    int32_t out[2] = {0, 0};  // out[0] = local_8, out[1] = local_4
    if (e->getClientCtx(out) && out[0] != 0 && out[1] != 0) return e->f60450(out[0]);
    return 0;
}

// 1.13c D2Client+0xA1620 (556 B): does candidate `esi` pass the target filter `flags` (EBX) for `origin`?
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_a1620(uintptr_t origin, uintptr_t esi, uint32_t flags, const Env* e) {
    if (!esi) return 0;
    int32_t st = (int32_t)rd32(esi, 0), sub = (int32_t)rd32(esi, 0x10);
    switch (st) {
    case 0:
        if (!(flags & 1)) return 0;
        if (flags & 0x1000) { if (sub != 0x11) return 0; } else if (sub == 0x11 || sub == 0) return 0;
        break;
    case 1:
        if (!(flags & 2)) return 0;
        if (flags & 0x1000) { if (sub != 0xC) return 0; } else if (sub == 0xC || sub == 0) return 0;
        if ((flags & 4) && e->ord10239() == 0) return 0;
        if ((flags & 0x4000) && e->ord10064(0) != 0) return 0;
        if ((flags & 0x40000) && e->ord10278() != 0) return 0;
        break;
    case 2: if (!(flags & 0x10)) return 0; break;
    case 3: {
        if (!(flags & 8)) return 0;
        uintptr_t r = e->f1580();
        if (!r || (e->gdwBitMask4 & *(const uint8_t*)(r + 4))) return 0;
        break; }
    case 4: if (!(flags & 0x20)) return 0; break;
    default: return 0;
    }
    uint32_t sf = rd32(esi, 0xC4);
    if (!((int8_t)flags >= 0 || ((sf >> 2) & 1))) return 0;
    if ((flags & 0x400) && !((sf >> 3) & 1)) return 0;
    if ((flags & 0x100) && e->ord10057(e->ord10331(esi)) != 0) return 0;
    if (flags & 0x10000) {
        if (!e->f50ee0(origin)) return 0;
        if (st == 1) { uintptr_t r = e->f1230(); if (!r || (e->gdwBitMask0 & *(const uint8_t*)(r + 0xD))) return 0; }
    }
    if ((flags & 0x8000) && e->f62a0(origin) == 0) return 0;
    if ((flags & 0x20000) && e->ord11138(origin) == 0) return 0;
    if ((flags & 0x80000) && e->ord10494() != 0) return 0;
    if (!(flags & 0x200)) return 1;
    int32_t a = e->ord10331(origin);
    if (a && e->ord10331(esi)) {
        uint32_t p[2] = {e->f1630(origin), e->f1660(origin)};
        uint32_t q[2] = {e->f1630(esi), e->f1660(esi)};
        if (e->ord11076(a, p, q, 4) == 0) return 1;
    }
    return 0;
}

// 1.13c D2Client+0xA1860 (490 B): walk the rooms around `unit`, call scanCb on each in-range candidate
// that passes 0xA1620. `limit` is EBX (distance bound); `flags` (param_5) defaults to 0x583; `ctx` is
// param_6, passed through to 0xA0750 and ord10769 unread. Register-passed args reconstructed from the
// decompile (ponytail: arg slots after param_4 are decompiler-shifted; names follow use).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_a1860(uintptr_t unit, uint32_t radius, uint32_t flags, int32_t limit, uintptr_t ctx, const Env* e) {
    if (!unit || !ctx || !radius) return;
    int32_t room = e->ord10331(unit);
    if (!room) return;
    if (!flags) flags = 0x583;
    uint32_t* list = nullptr; uint32_t count = 0;
    e->ord10383(room, &list, &count);
    for (uint32_t i = 0; i < count; i++) {
        uint32_t rm = list[i];
        if ((flags & 0x2000) && e->ord10057((int32_t)rm) != 0) continue;
        if (!e->f50750(rm, ctx)) continue;
        for (uintptr_t c = rd32(rm, 0x74); c; c = rd32(c, 0xE8)) {
            if (c == unit) continue;
            int32_t w, h; unitSize(e, c, &w, &h);
            int32_t d = e->ord10769((int32_t)ctx, 0, h, w);
            if (d > limit) continue;
            if (!d2_fn_d2client_a1620(unit, c, flags, e)) continue;
            if (radius != 0 && (int32_t)rd32(c, 0) == 1 && e->f1760(0, 0x1B) != 0) continue;
            e->scanCb(c, ctx);
        }
    }
}

// 1.13c D2Client+0xA1A50 (117 B): scan with flags|0xA280 and the 0x509F0 filter; clears *out.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_a1a50(uintptr_t unit, uint32_t radius, uint32_t flags, int32_t limit, uint32_t* out, const Env* e) {
    if (!flags) flags = 0x583;
    d2_fn_d2client_a1860(unit, radius, flags | 0xA280, limit, e->cb509f0, e);
    if (out) *out = 0;
    return 0;
}

// 1.13c D2Client+0xA1AD0 (55 B): scan with flags|0xA783 and the 0x50740 filter.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_a1ad0(uintptr_t unit, uint32_t radius, uint32_t flags, int32_t limit, const Env* e) {
    d2_fn_d2client_a1860(unit, radius, flags | 0xA783, limit, e->cb50740, e);
    return 0;
}

// 1.13c D2Client+0xA1B10 (468 B): apply skill `idx`'s start handler to `unit` (EAX). param_3/4/5 are the
// pair/byte; `swap` is the caller-pushed flag (unaff_retaddr), `ebx`/`ebp` the register-passed flags.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_a1b10(uintptr_t unit, int32_t idx, uint32_t p3, uint32_t p4, uint8_t p5,
                                                         uint32_t swap, uint32_t ebx, uint32_t ebp, const Env* e) {
    if (!unit || idx < 0 || idx >= (int32_t)e->dtI32(0xBA0)) return;
    const uint8_t* rec = e->dtPtr(0xB98) + (uint32_t)idx * 0x23C;
    if (!rec) return;
    int32_t ctx2[2]; uint32_t saved = rd32(unit, 0x28);
    uint32_t keep = e->getClientCtx(ctx2);
    e->fda0b0();
    if (rd32(unit, 0x2C) == 0) e->fatal(0x946);
    uint32_t u2 = e->ord10392(rd32(unit, 0x2C));
    if (swap == 0) {
        if (p3 != 0 && p4 != 0) { e->ord10286(rd32(unit, 0x2C), p3, p4, 0); wr32(unit, 0x28, p5); }
    } else { e->ord11129(); wr32(unit, 0x28, p5); }
    if (p3 != 0 && p4 == 0) wr32(unit, 0x28, p3);
    int16_t hs = rd16(rec, 0xF6);
    if (hs >= 0 && hs < e->handlerCount() && e->callHandler(hs, idx) > 0) {
        if (rd16(rec, 0x100) > 0) e->f38a70(unit, 0, 0, 0);
        if (rd16(rec, 0x10A) > 0) { uintptr_t t = e->f55f70(); if (t) e->f38a70(t, 0, 0, 0); }
        if (rd16(rec, 0x108) > 0) { uintptr_t t = e->f55f70(); if (t) e->f1b300(t, 2, 0, 0, 0); }
    }
    e->ord11129();
    if (ebp != 0 && ebx == 0) e->ord10286(rd32(unit, 0x2C), 0, u2, 0);
    wr32(unit, 0x28, keep);
}

// 1.13c D2Client+0xA2230 (128 B): mark `unit` (flags |= 0x40) and run its stand-down; always 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_a2230(uintptr_t unit, uintptr_t stackArg, const Env* e) {
    if (unit) wr32(unit, 0xC4, rd32(unit, 0xC4) | 0x40);
    if (e->ord10889(unit) == 0) {
        e->fda0b0();
        if (rd32(unit, 0x2C) == 0) e->fatal(0x946);
        e->ord10392(rd32(unit, 0x2C));
    } else if (e->ord10709(unit, stackArg) != -1) {
        e->f515c0();
    }
    return 1;
}

// 1.13c D2Client+0xA2380 (419 B): scatter `count` particles around the template `p` (EDI), random
// jitter of +-spread, one diagonal per step (rng holder `h` = ESI).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_a2380(int32_t count, int32_t spread, uintptr_t h, uint32_t* p, const Env* e) {
    uint64_t s = (uint64_t)rd32(h, 0x20) * 0x6AC690C5ull + rd32(h, 0x24);
    wr32(h, 0x20, (uint32_t)s); wr32(h, 0x24, (uint32_t)(s >> 32));
    uint32_t y0 = p[6], x0 = p[5], dir = (uint32_t)s & 3;
    uint32_t n2 = (uint32_t)spread * 2;
    for (int32_t i = 0; i < count; i++) {
        static const int32_t dx[4] = {-0x10, -0x10, 0x10, 0x10}, dy[4] = {-0x10, 0x10, 0x10, -0x10};
        int32_t lx = dx[dir], ly = dy[dir];
        dir = (dir + 1) & 3;
        p[5] = seedRoll(h, n2) - spread + x0;
        uint32_t y = y0 + seedRoll(h, n2) - spread;
        p[6] = y; p[8] = y + ly; p[7] = p[5] + lx;
        p[0x15] = e->cb50650; p[0x16] = dir << 4;
        e->f630f0(p);
    }
}

// 1.13c D2Client+0xA25C0 (233 B): emit the ring of `n` particles through the two direction tables.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_a25c0(int32_t n, uint32_t* p, const Env* e) {
    int32_t r = n - 1;
    if (r / 5 > 7) e->fatal(0x848);
    p[7] = 0xE; p[8] = 0xFFFFFFF2; e->f630f0(p);
    for (uint32_t a = 0;;) {
        if (r == 0) return;
        for (uint32_t b = 0; b < 0x10; b += 4) {
            if (r < 1) break;
            p[7] = e->rodata(0x6FB84280 + b) * e->rodata(0x6FB84254 + a);
            p[8] = e->rodata(0x6FB84270 + b) * e->rodata(0x6FB84238 + a);
            e->f630f0(p);
            p[7] = e->rodata(0x6FB84280 + b) * e->rodata(0x6FB84238 + a);
            p[8] = e->rodata(0x6FB84270 + b) * e->rodata(0x6FB84254 + a);
            e->f630f0(p);
            r -= 2;
        }
        p[7] = e->rodata(0x6FB84218 + a); p[8] = e->rodata(0x6FB841F8 + a);
        e->f630f0(p);
        a += 4; r -= 1;
        if (a > 0x1B) return;
    }
}

// 1.13c D2Client+0xA27B0 (411 B): `count` random-offset effects around `u`'s position; returns 1 on a ctx.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_a27b0(uintptr_t u, uint32_t p2, int32_t count, int32_t spread, uintptr_t p5, uint32_t p6, const Env* e) {
    int32_t ctx[2];  // ctx[0] = local_10 (x base), ctx[1] = local_4 (y base)
    if (!e->getClientCtx(ctx)) return 0;
    e->ord10614();
    uint32_t n2 = (uint32_t)spread * 2;
    for (int32_t i = count; i > 0; i--) {
        int32_t x = (int32_t)seedRoll(u, n2) - spread + ctx[0];
        int32_t y = (int32_t)seedRoll(u, n2) - spread + ctx[1];
        int32_t w, h; unitSize(e, u, &w, &h);
        if (e->ord10769(h, w, x, y) > 3) e->f69a80(p5, p2, 0, 0, x, y, p5, p6, 0);
    }
    return 1;
}

// 1.13c D2Client+0xA2950 (824 B): cast skill `idx` effect on `unit` (EAX); `swap` is the caller-pushed
// flag (unaff_retaddr). Mirrors 0xA1B10's tail plus the 0xE8 start-sound branch and the pre-pass of
// earlier-stage handlers when the record has gdwBitMasks[0x1c].
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_a2950(uintptr_t unit, int32_t p1, int32_t idx, uint32_t p3, int32_t stage,
                                                         uint32_t swap, const Env* e) {
    if (!unit || idx < 0 || idx >= (int32_t)e->dtI32(0xBA0)) return;
    const uint8_t* rec = e->dtPtr(0xB98) + (uint32_t)idx * 0x23C;
    if (!rec) return;
    int16_t s80 = rd16(rec, 0x80), s54 = rd16(rec, 0x54);
    if (s80 < 0 || s80 >= (int32_t)e->dtI32(0xC4) || s54 < 0 || s54 >= (int32_t)e->dtI32(0xBD4)) return;
    int32_t room = e->ord10871();
    if (room == 0) {
        room = e->ord11013(0, 0, 0, rd32(unit, 0), rd32(unit, 0xC));
        if (!room) return;
        e->ord10785(room, s80);
        int32_t t = e->ord10807();
        e->ord10188(room, s54, t, 0);
    }
    int32_t ctx[2]; uint32_t uVar4 = e->getClientCtx(ctx);
    uintptr_t uVar5 = e->f55f70();
    if (swap == 0) { if (idx != 0 && p3 != 0) e->ord10286(rd32(unit, 0x2C), idx, p3, 0); }
    else e->ord11129();
    uint32_t saved = rd32(unit, 0x28);
    if (idx != 0 && p3 == 0) wr32(unit, 0x28, idx);
    int32_t last = stage - 1;
    if (rec[4] & e->gdwBitMask0x1c) {
        for (int32_t i = 0; i < last; i++) {
            int16_t hs = rd16(rec, 0xF6 + i * 2);
            if (e->callHandler(hs, 0) != -1) { e->ord10188(room, s54, i + 1, 0); e->callHandler(hs, (int32_t)saved); }
        }
        e->ord10188(room, s54, stage, 0);
    }
    int16_t hs = rd16(rec, 0xF6 + last * 2);
    if (hs >= 0 && hs < e->handlerCount()) {
        if (rd16(rec, 0xE8) > 0) {
            if ((e->gdwBitMask4 & rec[4]) == 0) e->f515c0();
            else { int32_t c2[2]; e->getClientCtx(c2); e->f69a80((uintptr_t)c2[0], (uint32_t)rd16(rec, 0xE8), 0, 0, room, c2[0], swap, p1, 0); }
            wr32(unit, 0xC4, rd32(unit, 0xC4) | 0x40);
        }
        int32_t r = e->callHandler(hs, p1);
        if (r == -1) wr32(unit, 0xC4, rd32(unit, 0xC4) | 0x40);
        else if (r != 0) {
            if (rd16(rec, 0x100) > 0) e->f38a70(unit, 0, 0, 0);
            if (rd16(rec, 0x10A) > 0) { uintptr_t t = e->f55f70(); if (t) e->f38a70(t, 0, 0, 0); }
            if (rd16(rec, 0x108) > 0) { uintptr_t t = e->f55f70(); if (t) e->f1b300(t, 2, 0, 0, 0); }
        }
    }
    e->ord11129();
    if (room != 0) e->ord10286(rd32(unit, 0x2C), uVar4, (uint32_t)uVar5, (uint32_t)uVar5);
    if (swap != 0 && p1 == 0) wr32(unit, 0x28, (uint32_t)ctx[0]);
}

// 1.13c D2Client+0xA2F60 (146 B): mark `unit` (flags |= 0x40), resolve its object; returns 0 if none, else 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2client_a2f60(uintptr_t unit, uint32_t p2, uint32_t edi, const Env* e) {
    if (unit) wr32(unit, 0xC4, rd32(unit, 0xC4) | 0x40);
    int32_t o = e->ord10641(unit);
    if (!o) return 0;
    uint32_t a = e->ord10321(o);
    if (e->ord11088(o) == 0x26) {
        int32_t c[2] = {0, 0}; e->getClientCtx(c);
        e->f69a80((uintptr_t)c[0], a, 0, 0, (int32_t)edi, c[0], p2, 1, 0);
        return 1;
    }
    e->f515c0();
    return 1;
}
