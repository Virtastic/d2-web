// src/core/d2sound/d2sound_batch_z34.cpp -- 1.13c D2Sound.dll batch z34 (Stage A, unspecced/unvectored).
// 32-bit links are uint32_t addresses reached through rd32/wr32; COM calls go through com(); every DAT_ is a seam.
// GUESSED: 0x6C00/0x6D80 scale (FUN_8830 x87, result ST0 dropped by decompiler) modelled as seam scale(lo,hi,x);
//          FUN_baccc modelled as seam ftol(). 0x9230 Ordinal_256 / 0x83C0 ordinals 0x-6F9AFFxx are Win32 imports (seams).
#include <cstdint>
#include <cstdlib>

struct Z34Seams {
    uint32_t (*rd32)(uint32_t a);
    void (*wr32)(uint32_t a, uint32_t v);
    uint32_t (*alloc)(uint32_t n);                       // Fog Ordinal_10042(n,0)
    void (*fatal)(uint32_t strId);          // Ordinal_10265(id) + Ordinal_10024(fmt,..) + _exit(-1)
    uint32_t (*com)(uint32_t obj, uint32_t vtOff, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e); // vcall obj->vt[vtOff]
    void (*regWrite)(const char* app, const char* key, uint32_t def, uint32_t val);  // Fog ord 426
    void (*sleepMs)(uint32_t ms);
    void (*lock)(uint32_t cs); void (*unlock)(uint32_t cs); void (*initCs)(uint32_t cs); void (*delCs)(uint32_t cs);
    void (*setEvent)(uint32_t h); void (*waitInf)(uint32_t h); void (*closeH)(uint32_t h);
    void (*fn6B80)(uint32_t o, uint32_t a, uint32_t b);
    void (*fn6E20)(uint32_t o, uint32_t v);
    void (*fn6FA0)(uint32_t o);
    uint32_t (*fn7740)(uint32_t a, uint32_t b);
    void (*fn7480)(uint32_t v);
    uint32_t (*fn8790)(uint32_t* p, uint32_t* len);      // returns chunk ptr or 0
    float (*scale)(float lo, float hi, float x);         // FUN_8830 (guessed)
    int32_t (*ftol)(float v);                            // FUN_baccc (guessed)
    void (*fn256)();                                     // Ordinal_256
};
struct Z34G {  // DAT_6f9c.. globals
    uint32_t d5994, d6580, d6588, d658c, d6598, d659c, d65a0, d65b8, d65c0, d65cc, d65d0, d65d4;
    uint32_t d64a4, d64ac, d64b8, d6524, d6528, d652c;
    uint32_t tbl6560[8];
    uint32_t cs6530, cs6548;
};

// 1.13c D2sound+0x8640
void z34_8640(const Z34Seams& s, uint32_t head, uint32_t data) {
    uint32_t n = s.alloc(0x11f);
    s.wr32(n, 0); s.wr32(n + 4, 0); s.wr32(n, data);
    uint32_t c = s.rd32(head);
    if (!c) { s.wr32(head, n); return; }
    while (s.rd32(c + 4)) c = s.rd32(c + 4);
    s.wr32(c + 4, n);
}

// 1.13c D2sound+0x8100
void z34_8100(const Z34Seams& s, Z34G& g) {
    if (!g.d65b8) return;
    for (int i = 0; i < 8; i++) {
        uint32_t o = g.tbl6560[i];
        if (!o || !s.rd32(o)) s.fatal(0x2a5);
        if (s.rd32(o + 0x1c)) s.fn6B80(o, 0, 200);
    }
}

// 1.13c D2sound+0x68D0
void z34_68D0(const Z34Seams& s, uint32_t self, uint32_t a, uint32_t b, uint32_t c) {
    if (!self || !s.rd32(self)) s.fatal(0x302);
    if (s.rd32(self + 0x34) & 2) s.com(s.rd32(self + 0xc), 0x4c, a, b, c, 0, 0);
}

// 1.13c D2sound+0x83C0
void z34_83C0(const Z34Seams& s, Z34G& g) {
    if (!g.d658c || g.d65b8) return;
    s.initCs(g.cs6548); s.lock(g.cs6548);
    if (g.d658c && g.d5994 != 0xff) {
        s.lock(g.cs6530);
        g.d5994 = 0xff;
        for (uint32_t n = g.d65cc; n; n = s.rd32(n + 4)) {
            uint32_t o = s.rd32(n);
            if (!o) s.fatal(0x2e4);
            s.fn6E20(o, s.rd32(o + 0xe * 4));
        }
        s.unlock(g.cs6530);
    }
    for (int i = 0; i < 8; i++) g.tbl6560[i] = s.fn7740(i < 4, 0);
    g.d65b8 = 1;
    s.unlock(g.cs6548);
}

// 1.13c D2sound+0x9A70
uint32_t z34_9A70(const Z34Seams& s, Z34G& g, uint32_t v) {
    s.regWrite("Diablo II", "Positional Bias", 0, v);
    return g.d64ac = v;
}

// 1.13c D2sound+0x7510
void z34_7510(const Z34Seams& s, Z34G& g, int32_t p) {
    if (!g.d658c) return;
    if (p < 1) s.fatal(0x387);
    uint32_t step = (uint32_t)((uint64_t)g.d5994 / ((uint64_t)(uint32_t)p / 0x14));
    uint32_t t = g.d5994;
    while (t) {
        s.sleepMs(0x14);
        t = step < t ? t - step : 0;
        s.fn7480(t);
    }
}

// 1.13c D2sound+0x7FB0
void z34_7FB0(const Z34Seams& s, Z34G& g) {
    if (g.d6580) {
        g.d65c0 = 0;
        if (s.rd32(g.d6580 + 0x48)) s.fn6B80(g.d6580, 0, 200);
    }
}

// 1.13c D2sound+0x8980
// buf: RIFF header (12 bytes) then chunks; *len in/out. fmt (out) is 9 u16 = WAVEFORMATEX-ish. Returns data chunk ptr or 0.
const uint8_t* z34_8980(const Z34Seams& s, const uint8_t* buf, uint32_t* len, uint16_t* fmt) {
    auto r32 = [](const uint8_t* p) { return (uint32_t)p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; };
    auto r16 = [](const uint8_t* p) { return (uint16_t)(p[0] | p[1] << 8); };
    if (*len < 0x20) return nullptr;
    *len -= 0xc;
    if (r32(buf) != 0x46464952 || r32(buf + 8) != 0x45564157) return nullptr;
    uint32_t cl = 0;
    const uint8_t* c = (const uint8_t*)(uintptr_t)s.fn8790((uint32_t*)len, &cl);  // GUESSED: seam returns chunk body, advances *len
    if (!c) return nullptr;
    if (cl > 0xf && cl <= *len) {
        *len -= cl;
        fmt[0] = r16(c); fmt[1] = r16(c + 2);
        fmt[2] = (uint16_t)r32(c + 4); fmt[3] = (uint16_t)(r32(c + 4) >> 16);
        fmt[4] = (uint16_t)r32(c + 8); fmt[5] = (uint16_t)(r32(c + 8) >> 16);
        fmt[6] = (uint16_t)r32(c + 12); fmt[7] = r16(c + 0xe);
        fmt[8] = 0;
        c = (const uint8_t*)(uintptr_t)s.fn8790((uint32_t*)len, &cl);
        if (c && cl <= *len) { *len = cl; return c; }
    }
    return nullptr;
}

// 1.13c D2sound+0x6C00
void z34_6C00(const Z34Seams& s, Z34G& g, uint32_t self, float v) {
    if (!self) s.fatal(0);
    if (g.d659c) v = 0.0f;
    uint32_t b = s.rd32(self + 0x10);
    if (!b) return;
    if (g.d6598 == 1) { s.com(b, 0x10, 0 /*&DAT_6f9c01e8*/, 1, 0, 0, 0); return; }  // GUESSED arg layout
    float sc = s.scale(0.0f, 1.0f, v);
    int32_t r = s.ftol(sc);
    (void)r;
    s.com(b, 0x10, 0 /*&DAT_6f9c0208*/, 5, 0, 0, 0);
    s.com(b, 0x10, 0, 4, 0, 0, 0);  // second call with (&DAT_6f9c0208,4,0,0,&r,4): GUESSED
}

// 1.13c D2sound+0x6D80
void z34_6D80(const Z34Seams& s, uint32_t* self, int32_t p) {
    if (!self) s.fatal(0x2ef);
    uint32_t obj = *self;
    if (p > 0x80) { int32_t r = s.ftol(s.scale(0.0f, 127.0f, (float)(0xff - p))); s.com(obj, 0x40, (uint32_t)r, 0, 0, 0, 0); return; }
    if (p < 0x80) { int32_t r = s.ftol(s.scale(0.0f, 127.0f, (float)p)); s.com(obj, 0x40, (uint32_t)r, 0, 0, 0, 0); return; }
    s.com(obj, 0x40, 0, 0, 0, 0, 0);
}

// 1.13c D2sound+0x9AD0
uint32_t z34_9AD0(const Z34Seams& s, Z34G& g, uint32_t v) {
    s.regWrite("Diablo II", "Master Volume", 0, v);
    return g.d64a4 = v;
}

// 1.13c D2sound+0x9230
void z34_9230(const Z34Seams& s, Z34G& g) {
    g.d6588 = 0;
    if (g.d658c && g.d5994) {
        s.lock(g.cs6530);
        g.d5994 = 0;
        for (uint32_t n = g.d65cc; n; n = s.rd32(n + 4)) {
            uint32_t o = s.rd32(n);
            if (!o) s.fatal(0x2e4);
            s.fn6E20(o, s.rd32(o + 0xe * 4));
        }
        s.unlock(g.cs6530);
    }
    if (g.d6580) { s.fn6FA0(g.d6580); g.d6580 = 0; }
    s.fn256();
    if (g.d65d4 && g.d65d0) {
        s.setEvent(g.d65d0); s.waitInf(g.d65d4); s.closeH(g.d65d0); s.closeH(g.d65d4);
        g.d65d0 = g.d65d4 = 0;
    }
    uint32_t* rel[] = {&g.d64b8, &g.d6524, &g.d6528, &g.d652c, &g.d658c};
    for (uint32_t* r : rel) if (*r) { s.com(*r, 8, 0, 0, 0, 0, 0); *r = 0; }  // IUnknown::Release
    if (g.d65a0) { s.delCs(g.cs6530); g.d65a0 = 0; }
}
