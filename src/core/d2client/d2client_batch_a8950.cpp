// src/core/d2client/d2client_batch_a8950.cpp -- twelve UNMAPPED 1.13c D2Client.dll rows (light grid / minimap)
// reimplemented from build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane). Base 0x6FAB0000.
// Globals are parameters; unrecovered callees and Ordinals are host callbacks. Register-passed
// arguments (EAX/ESI/EDI) are named in each comment. `fatal` is the retail Ordinal_10265/10024/_exit path.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t R32(uintptr_t p) { return *(const uint32_t*)p; }
static inline void W32(uintptr_t p, uint32_t v) { *(uint32_t*)p = v; }
typedef void (*FatalFn)(uint32_t msg);

// 1.13c D2Client+0xA8950 (219 B): ECX unused, param_2 = light record. tbl = DAT_6fbb0de8 (int32,
// row stride 0x41), ox/oy = DAT_6fbb4df0/4, flags = DAT_6fbb5208 (stride 8 bytes, 0x30x0x30 grid).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a8950(uintptr_t rec, int32_t* tbl, int32_t ox, int32_t oy, const uint8_t* flags) {
    int32_t r = (int32_t)R32(rec + 0x18);
    int32_t x0 = (int32_t)((R32(rec + 0x10) & ~7u) - (uint32_t)r);
    int32_t y = (int32_t)((R32(rec + 0x14) & ~7u) - (uint32_t)r);
    int32_t n = (r * 2) >> 3;
    if (n < 0) return;
    n++;
    int32_t* row = tbl + (0x20 - (r >> 3)) * 0x41;
    for (int32_t j = 0; j < n; j++, row += 0x40, y += 8) {
        int32_t* p = row;
        int32_t x = x0;
        for (int32_t i = 0; i < n; i++, x += 8) {
            uint8_t b = 1;
            int32_t cx = (x >> 3) - ox, cy = (y >> 3) - oy;
            if (cx >= 0 && cx < 0x30 && cy >= 0 && cy < 0x30) b = flags[(cy * 0x30 + cx) * 8];
            *p++ = (int32_t)((uint32_t)b << 4);
        }
    }
}

// 1.13c D2Client+0xA8A40 (283 B): EAX = x, ECX = y (pixels), then a,r,g,b. cells = DAT_6fbb520c
// (stride 8: light, r, g, b), curve = DAT_6fbb4e00 (int32[256]), tint = DAT_6fb8e830.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a8a40(int32_t x, int32_t y, int32_t a, uint8_t r, uint8_t g, uint8_t b,
                     int32_t ox, int32_t oy, uint8_t* cells, const int32_t* curve, uint32_t tint) {
    int32_t cx = (x >> 3) - ox, cy = (y >> 3) - oy;
    if (cx < 0 || cx >= 0x30 || cy < 0 || cy >= 0x30) return;
    uint8_t* c = cells + (cy * 0x30 + cx) * 8;
    uint32_t old = c[0];
    int32_t v = (int32_t)old + a;
    if (v > 0xfe) v = 0xff;
    if (tint == 0) { c[0] = (uint8_t)v; c[1] = c[2] = c[3] = 0; return; }
    uint32_t k = (uint32_t)curve[v];
    uint32_t nr = ((uint32_t)c[1] * old + (uint32_t)r * (uint32_t)a) * k >> 16;
    uint32_t ng = ((uint32_t)c[2] * old + (uint32_t)g * (uint32_t)a) * k >> 16;
    uint32_t nb = ((uint32_t)c[3] * old + (uint32_t)b * (uint32_t)a) * k >> 16;
    c[0] = (uint8_t)v;
    c[1] = (uint8_t)(nr > 0xff ? 0xff : nr);
    c[2] = (uint8_t)(ng > 0xff ? 0xff : ng);
    c[3] = (uint8_t)(nb > 0xff ? 0xff : nb);
}

// 1.13c D2Client+0xA8B60 (113 B): EAX = radius, ESI = light record (null -> fatal 0x141).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a8b60(int32_t radius, uintptr_t rec, FatalFn fatal, void (*ord10043)(uint32_t, uint32_t)) {
    if (radius <= 0) return;
    if (radius > 0x11) radius = 0x12;
    if (rec == 0) { fatal(0x141); return; }
    W32(rec + 0x18, (uint32_t)radius << 3);
    W32(rec + 0x1c, (uint32_t)radius << 3);
    if (R32(rec + 0xc) == 2) {
        W32(rec + 0x2c, 0);
        if (R32(rec + 0x30) != 0) ord10043(0x14a, 0);
        W32(rec + 0x30, 0);
    }
}

// 1.13c D2Client+0xA8BE0 (56 B): EAX = radius, ECX = record (null -> fatal 0x131).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a8be0(uintptr_t rec, int32_t radius, FatalFn fatal) {
    if (radius <= 0) return;
    if (radius > 0x11) radius = 0x12;
    if (rec == 0) { fatal(0x131); return; }
    W32(rec + 0x1c, (uint32_t)radius << 3);
}

// 1.13c D2Client+0xA8D30 (56 B): EAX = {type, id, flag}; type 6 -> 0. flag!=0: table b9a08 else ba608
// (both 0x200 bytes per type). Returns 0x54E20(id, table + type*0x200, type).
extern "C" EMSCRIPTEN_KEEPALIVE
uintptr_t d2_client_a8d30(uintptr_t key, uintptr_t tab_b9a08, uintptr_t tab_ba608,
                          uintptr_t (*lookup_54e20)(uint32_t id, uintptr_t tab, int32_t type)) {
    int32_t t = (int32_t)R32(key);
    if (t == 6) return 0;
    uintptr_t tab = ((uint8_t)R32(key + 8) != 0 ? tab_b9a08 : tab_ba608) + (uintptr_t)t * 0x200;
    return lookup_54e20(R32(key + 4), tab, t);
}

// 1.13c D2Client+0xA8D70 (485 B): ECX = param_1; walk the list at head (DAT_6fbcbbec, next = +0x28).
// tab_b9a08/tab_ba608 = hash tables (bucket = type*0x200 + (id&0x7f)*4, chain next = +0xE4).
// ord10331 = unit->owner, ord10867/10750 = per-unit hooks, call1400 = FUN_6fab1400 (x4, no args).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a8d70(uint32_t param_1, uintptr_t head, uintptr_t tab_b9a08, uintptr_t tab_ba608,
                     FatalFn fatal, uint32_t (*ord10331)(uintptr_t), void (*ord10867)(uint32_t),
                     void (*ord10750)(uint32_t), uint32_t (*call1400)(void)) {
    for (uintptr_t n = head; n != 0; n = (uintptr_t)R32(n + 0x28)) {
        if (R32(n + 0xc) != 2) continue;
        int32_t t = (int32_t)R32(n);
        if (t == 6) { fatal(0x591); return; }
        uint32_t id = R32(n + 4);
        uintptr_t tab = ((uint8_t)R32(n + 8) != 0 ? tab_b9a08 : tab_ba608) + (uintptr_t)t * 0x200;
        uintptr_t u = (uintptr_t)R32(tab + (id & 0x7f) * 4);
        for (; u != 0; u = (uintptr_t)R32(u + 0xe4))
            if (R32(u + 0xc) == id) break;
        if (u != 0 && (int32_t)R32(u) != t) { fatal(0x5a); return; }
        if (u == 0) { fatal(0x591); return; }
        if (ord10331(u) == param_1) continue;
        int32_t k = (int32_t)R32(u);
        if (k != 2 && (k < 4 || k > 5) && R32(u + 0x2c) != 0) ord10867(R32(u + 0x2c));
        k = (int32_t)R32(u);
        if (k != 2 && (k < 4 || k > 5) && R32(u + 0x2c) != 0) ord10750(R32(u + 0x2c));
        if (call1400() != param_1 && call1400() != param_1 && call1400() != param_1 && call1400() != param_1)
            continue;
        W32(n + 0x2c, 0);
    }
}

// 1.13c D2Client+0xA8F60 (403 B): EDI = light record. The running x pixel is a register the
// decompile drops; it starts at (rec+0x10 & ~7) - r and advances 8 per inner step (as in 0xA9860).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a8f60(uintptr_t rec, int32_t ox, int32_t oy, int32_t mx, int32_t my, FatalFn fatal,
                     uint32_t (*f587b0)(uint32_t),
                     void (*f58a40)(int32_t x, int32_t y, int32_t a, uint8_t r, uint8_t g, uint8_t b)) {
    int32_t r = (int32_t)R32(rec + 0x18);
    if (r <= 0 || r >= 0x100) return;
    int32_t y = (int32_t)((R32(rec + 0x14) & ~7u) - (uint32_t)r);
    int32_t x0 = (int32_t)((R32(rec + 0x10) & ~7u) - (uint32_t)r);
    int32_t n = (r * 2) >> 3;
    if (!((x0 >> 3) <= mx && ox <= (x0 >> 3) + 1 + n && (y >> 3) <= my && oy <= (y >> 3) + 1 + n)) return;
    uintptr_t p = (uintptr_t)R32(rec + 0x30);
    if (p == 0) { fatal(0x47e); return; }
    if (R32(rec + 0x2c) == 0) { fatal(0x47f); return; }
    uint8_t amp = (uint8_t)R32(rec + 0x24);
    if (n < 0) return;
    n++;
    for (int32_t j = 0; j < n; j++, y += 8) {
        int32_t dy = (int32_t)R32(rec + 0x14) - y;
        uint32_t ady = (uint32_t)(dy < 0 ? -dy : dy);
        int32_t x = x0;
        for (int32_t i = 0; i < n; i++, p += 4, x += 8) {
            int32_t cell = (int32_t)R32(p);
            if (cell < 0x10) {
                uint32_t d = f587b0(ady);
                int32_t q = (int32_t)(((int64_t)((uint64_t)amp << 16)) / (int64_t)r);
                int32_t a = (((int32_t)(((uint32_t)(int32_t)R32(rec + 0x18) - d) * (uint32_t)q)) >> 16) *
                            (8 - (cell >> 1)) >> 3;
                if (a > 0)
                    f58a40(x, y, a, (uint8_t)R32(rec + 0x25), (uint8_t)R32(rec + 0x26), (uint8_t)R32(rec + 0x27));
            }
        }
    }
}

// 1.13c D2Client+0xA9860 (382 B): EAX = flag, ESI = light record, localunit = DAT_6fbcbbfc.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a9860(int32_t flag, uintptr_t rec, int32_t ox, int32_t oy, int32_t mx, int32_t my,
                     uintptr_t localunit, uint32_t (*f58760)(uint32_t),
                     void (*f58a40)(int32_t x, int32_t y, int32_t a, uint8_t r, uint8_t g, uint8_t b)) {
    int32_t r = (int32_t)R32(rec + 0x18);
    if (r <= 0 || r >= 0x100) return;
    if (flag == 0) {
        int32_t k = (int32_t)R32(rec);
        if (k == 0) {
            int32_t id = localunit == 0 ? -1 : (int32_t)R32(localunit + 0xc);
            if ((int32_t)R32(rec + 4) != id && r > 0xf) r = 0x10;
        } else if (k == 1) r = 8;
        else if (k == 3) return;
    }
    int32_t y = (int32_t)((R32(rec + 0x14) & ~7u) - (uint32_t)r);
    int32_t x0 = (int32_t)((R32(rec + 0x10) & ~7u) - (uint32_t)r);
    int32_t n = (r * 2) >> 3;
    if (!((x0 >> 3) <= mx && ox <= (x0 >> 3) + 1 + n && (y >> 3) <= my && oy <= (y >> 3) + 1 + n)) return;
    uint8_t amp = (uint8_t)R32(rec + 0x24);
    if (n < 0) return;
    n++;
    for (int32_t j = 0; j < n; j++, y += 8) {
        int32_t dy = (int32_t)R32(rec + 0x14) - y;
        uint32_t ady = (uint32_t)(dy < 0 ? -dy : dy);
        int32_t x = x0;
        for (int32_t i = 0; i < n; i++, x += 8) {
            uint32_t d = f58760(ady);
            int32_t q = (int32_t)(((int64_t)((uint64_t)amp << 16)) / (int64_t)r);
            int32_t a = ((int32_t)(((uint32_t)r - d) * (uint32_t)q)) >> 16;
            if (a > 0)
                f58a40(x, y, a, (uint8_t)R32(rec + 0x25), (uint8_t)R32(rec + 0x26), (uint8_t)R32(rec + 0x27));
        }
    }
}

// 1.13c D2Client+0xA9A20 (297 B): ECX = this (only its top byte survives), EAX = flag. Outputs
// alpha + rgb. c9610/c9614 = DAT_6fbc9610 / bytes at 6fbc9614..16, c2e4 = DAT_6fbcc2e4,
// a2130 = DAT_6fba2130, c3b8 = DAT_6fbcc3b8.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a9a20(uint32_t thiz, int32_t flag, uint32_t* oa, uint8_t* orr, uint8_t* og, uint8_t* ob,
                     uint32_t c9610, const uint8_t* c9614, uint32_t c2e4, int32_t a2130, uint32_t c3b8,
                     int32_t (*ord10826)(void), uint32_t (*f24d0)(uint8_t* rgb),
                     void (*ord10636)(void), void (*ord10189)(uint32_t, uint8_t*, uint8_t*, uint8_t*),
                     uint32_t (*ord10731)(uint32_t)) {
    uint8_t c[3] = {0, 0, 0};
    uint32_t hi = thiz & 0xff000000u, a = 0;
    bool done = false;
    if (flag != 0) {
        int32_t m = ord10826();
        if (m == 8) a = f24d0(c);
        else if (m < 0x6b || m > 0x6c) { c[0] = c9614[0]; c[1] = c9614[1]; c[2] = c9614[2]; a = c9610; }
        else if (c2e4 == 0 || a2130 != -1) a = 0;
        else { c[0] = 0xff; c[1] = 0x40; c[2] = 0x30; a = 0xa0; }
        done = c[0] || c[1] || c[2];
    }
    if (!done) {
        ord10636();
        a = hi >> 24;
        if (!c[0] && !c[1] && !c[2]) { ord10189(c3b8, &c[0], &c[1], &c[2]); a = ord10731(c3b8); }
    }
    *oa = a; *orr = c[0]; *og = c[1]; *ob = c[2];
}

// 1.13c D2Client+0xA9C90 (410 B): EBX = unit-id array, count (iStack_30, set by the caller),
// localunit = DAT_6fbcbbfc. s[] mirrors the stack locals (30,2c,28,24,20); 0x10561 fills &s[1].
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_a9c90(int32_t param_1, const uint32_t* ids, int32_t count, uintptr_t localunit,
                     int32_t ox, int32_t oy,
                     uint32_t (*ord10331)(uintptr_t), void (*ord10383)(uint32_t, int32_t*, int32_t*),
                     void (*ord10561)(uint32_t, int32_t*),
                     void (*f59a20)(uint32_t*, uint8_t*, uint8_t*, uint8_t*),
                     void (*f58560)(int32_t, uint32_t, int32_t)) {
    int32_t s[5] = {count, 0, 0, 0, 0};
    uint32_t u = localunit ? ord10331(localunit) : 0;
    if (u == 0 || param_1 != 0) { f58560(0, 0, 0); return; }
    ord10383(u, &s[2], &s[3]);
    uint8_t rr, gg, bb = 0; uint32_t al;
    f59a20(&al, &rr, &gg, &bb);
    f58560(0, (uint32_t)bb << 24, 0);
    for (int32_t i = 0; i < s[0]; i++) {
        if (ids[i] == u) continue;
        f59a20(&al, &rr, &gg, &bb);
        uint32_t c = bb;
        ord10561(ids[i], &s[1]);
        int32_t x = s[1] - ox;
        if (x < 0x31 && s[3] + x >= 0) {
            int32_t y = s[2] - oy;
            if (y < 0x31 && s[4] + y >= 0) {
                f58560(y < 0 ? 0 : y, c << 24, x < 0 ? 0 : x);
            }
        }
    }
}

// 1.13c D2Client+0xAA190 (141 B): release the three handles at DAT_6fbacdc1/c5/c9.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_aa190(uint32_t* h, void (*ord10065)(uint32_t), void (*ord10043)(uint32_t, uint32_t)) {
    for (int i = 0; i < 3; i++) {
        uint32_t v = h[i];
        if (v == 0) continue;
        ord10065(v);
        ord10043(0x7a, 0);
        h[i] = 0;
    }
}

// 1.13c D2Client+0xAA380 (81 B): once, with a local unit whose Ordinal_10331 is non-null, send the
// 3-byte-payload packet {0x49, cd[0..2], cd[3], 0:4} via 0x143E0 and set done (DAT_6fbacdd5).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_aa380(uint8_t* done, uintptr_t localunit, uint32_t cdcd,
                     uint32_t (*ord10331)(uintptr_t), void (*f143e0)(const uint8_t*)) {
    if (*done != 0 || localunit == 0) return;
    if (ord10331(localunit) == 0) return;
    uint8_t pkt[9] = {0x49, (uint8_t)cdcd, (uint8_t)(cdcd >> 8), (uint8_t)(cdcd >> 16),
                      (uint8_t)(cdcd >> 24), 0, 0, 0, 0};
    f143e0(pkt);
    *done = 1;
}
