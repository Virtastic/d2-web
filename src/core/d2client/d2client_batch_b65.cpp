// D2Client batch b65-cli: env-seam style; globals and callees enter through one struct.
// Hidden register inputs (EAX/ESI/ECX) are explicit leading params.
// Not implemented: 6E2C0 (jump table), 6F0B0/6F4A0 (direct Imm* Win32), 78820 (unrecovered stack outputs).
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>
template <class T> static inline T& at(const void* b, int32_t o) { return *(T*)((const uint8_t*)b + o); }
struct D2cB65Env {
    void (*fatal)(int32_t code);                         // Ordinal_10265(code); Ordinal_10024(); _exit(-1)
    const uint32_t* t_6fbc3fb8; const uint32_t* t_6fbc4018;
    uint32_t (*f1510)(const void* p, int32_t n);         // FUN_6fab1510
    uint32_t (*ord10491)(uint32_t a, uint32_t b, uint32_t c);
    void (*f1d540)(const char* s);                       // D2Client+0x6D540 (ecx hidden)
    // 6E670
    int32_t (*ord10013)(); void (*ord10651)(); void (*ord11142)(); int32_t (*ord11168)(); uint32_t (*ord11080)();
    void (*ord10023)(uint32_t); int32_t* g_6fbcc414; int32_t* g_6fb8bc48;
    bool (*f1a3d0)(int32_t); int32_t (*ord10866)(); void (*f560d0)(); int32_t (*ord10431)(); int32_t (*f1af0)();
    int32_t* (*f1a2f0)(int32_t, const void* unit); uint8_t (*f1d700)(const uint8_t*);
    // 72610 / 73630 / 73760
    int32_t (*ord10871)(const void*, ...); int32_t (*ord11013)(int32_t, int32_t, int32_t, int32_t, int32_t);
    void (*ord10785)(int32_t, int32_t); void (*ord10807)(const void*, int32_t, int32_t); void (*ord10680)(int32_t);
    void (*ord10188)(int32_t); void (*f559e0)(int8_t); void (*ord10819)(const void*, const char*, int32_t);
    const uint8_t* const* sgptDataTables; const uint8_t* gdwBitMasks;
    int32_t (*ord10494)(const void*); int32_t (*ord11132)(int32_t); void (*ord11111)(const void*, int32_t);
    void (*ord11108)(int32_t); void (*f38a70)(const void*, int32_t, int32_t, int32_t); void (*ord10945)(const void*);
    void (*f22130)(const void*); void (*ord10799)(int32_t); void (*f1380)(const void* qword);
    void (*f1b300)(const void*, int32_t, int32_t, int32_t, int32_t); void (*f227f0)(int32_t); void (*f234d0)(const void*);
    void (*f22c80)(int32_t); int32_t (*f55440)(); int32_t (*ord10341)(); void (*ord10343)(int32_t);
    // 76050 / 78240
    char** g_6fbcb9c4; int32_t* g_6fbcbff4; int32_t* g_6fbcbfb4; int32_t* g_6fbcb9a0; int32_t* g_6fbcb9a4; int32_t* g_6fb8bc4c;
    char* (*fadb420)(int32_t); void (*ord10041)(const int32_t* rec, int32_t x, int32_t y, int32_t a, int32_t b, int32_t c);
    int32_t* g_6fb925f0; const int32_t* g_6fb925f4; const int32_t* g_6fb924f8; const uint8_t* g_6fbc991c;
};
extern "C" {
// 1.13c D2Client+0x6DD30  (eax=in_EAX, esi=self, ecx=param_1, edx=param_2)
EMSCRIPTEN_KEEPALIVE uint32_t d2c_6dd30(const D2cB65Env* e, int32_t eax, uint8_t* esi, const uint8_t* p1, int32_t p2,
                                        int32_t p3, const int16_t* p4, const uint8_t* p5) {
    if (!eax) { e->fatal(0x3e2); return 0; }
    if (!p1) { e->fatal(0x3e3); return 0; }
    uint8_t b = esi[5];
    at<int32_t>(esi, 0xc) = *p4; at<int32_t>(esi, 0x10) = p3; at<int32_t>(esi, 8) = 1;
    at<int32_t>(esi, 0x28) = eax; at<uint32_t>(esi, 0x1c) = e->t_6fbc3fb8[b];
    at<uint32_t>(esi, 0x20) = 0x2074696c; at<int32_t>(esi, 0x18) = at<int32_t>(p5, 0x10);
    const uint8_t* q = at<const uint8_t*>(p1, 0x14);
    if (!q) { e->fatal(0x3f0); return 0; }
    const uint8_t* ent = q + 0x1c;
    for (uint32_t i = 0; i < *q; i++, ent += 9) {
        if (*ent > 0xf) { e->fatal(0x3fb); return 0; }
        if (*ent != b) continue;
        at<uint32_t>(esi, 0x24) = e->t_6fbc4018[p3];
        uint8_t c = ((const uint8_t*)p4)[p2 * 0x10 + b + 4];
        if ((p3 != 0 && p3 != 0xc) || e->f1510(p4, 0x10) != 0)
            at<uint32_t>(esi, 0x20) = e->ord10491(at<uint32_t>(esi, 0xc), b, c);
        e->f1d540((const char*)(ent + 5));
        at<int32_t>(esi, 0x28) = 0;
        e->fatal(at<int32_t>(esi, 0x1c) == 0 ? 0x425 : at<int32_t>(esi, 0x18) == 0 ? 0x426 : 0x427);
        return 0;
    }
    return 0;
}

// 1.13c D2Client+0x6E670  (eax=unit)
EMSCRIPTEN_KEEPALIVE uint8_t d2c_6e670(const D2cB65Env* e, const uint8_t* unit) {
    int32_t y;
    if (e->ord10013() == 0) { e->ord10651(); e->ord11142(); y = 0; /* uninitialised in original */ }
    else {
        y = e->ord11168(); e->ord10023(e->ord11080());
        int32_t w = *e->g_6fb8bc48, q = (w + ((w >> 31) & 3)) >> 2;
        if (*e->g_6fbcc414 == 1) y -= q; else if (*e->g_6fbcc414 == 2) y += q;
    }
    if (!e->f1a3d0(y)) return 0;
    if (unit && ((at<uint32_t>(unit, 0xc8) >> 3) & 1) && e->ord10866()) e->f560d0();
    int32_t k = e->ord10431();
    const uint8_t* sub = at<const uint8_t*>(unit, 0x54);
    if (!sub) { e->fatal(0x3ac); return 0; }
    int32_t v = e->f1af0();
    int32_t* n = at<int32_t*>(sub, 0x28);
    for (; n; n = (int32_t*)(intptr_t)n[4]) if (n[0] == v && n[1] == k) break;
    if (!n && !e->f1a2f0(k, unit)) return 0;
    int32_t v2 = e->f1af0();
    sub = at<const uint8_t*>(unit, 0x54);
    if (!sub) return 0;
    for (n = at<int32_t*>(sub, 0x28); n; n = (int32_t*)(intptr_t)n[4])
        if (n[0] == v2) {
            const uint8_t* p = at<const uint8_t*>((const void*)(intptr_t)n[2], 0x14);
            return p ? e->f1d700(p) : 0;
        }
    return 0;
}

// 1.13c D2Client+0x72610  (ecx=p1, edx=p2 unit, eax=skill id)
EMSCRIPTEN_KEEPALIVE int32_t d2c_72610(const D2cB65Env* e, int32_t eax, int32_t p1, const int32_t* p2, int32_t p3, int8_t p4) {
    if (!p2) return 0;
    if (!p3) {
        p3 = e->ord10871(p2, p1);
        if (!p3) { p3 = e->ord11013(0, 0, 0, p2[0], p2[3]); e->ord10785(p3, p1); e->ord10807(p2, p3, 1); }
        else e->ord10680(p3);
    }
    e->ord10188(p3);
    if (eax == 0xac) e->f559e0(p4);
    else if (eax < 0) return p3;
    const uint8_t* dt = *e->sgptDataTables;
    if (eax < at<int32_t>(dt, 0xbd4)) {
        const uint8_t* r = at<const uint8_t*>(dt, 0xbcc) + eax * 0x144;
        if (r && (e->gdwBitMasks[16] & r[5])) e->ord10819(p2, "..\\Source\\D2Client\\UNIT\\PlrSkills.cpp", 0x5d2);
    }
    return p3;
}

// 1.13c D2Client+0x73630  (eax=record index)
EMSCRIPTEN_KEEPALIVE void d2c_73630(const D2cB65Env* e, int32_t eax, const uint8_t* u) {
    const uint8_t* dt = *e->sgptDataTables;
    if (!u || eax < 0 || eax >= at<int32_t>(dt, 0xc4)) return;
    const uint8_t* r = at<const uint8_t*>(dt, 0xbc) + eax * 0x3c;
    if (!r) return;
    if (e->ord10494(u)) {
        int32_t i = e->ord10871(u);
        if (i && e->ord11132(i) == eax) { e->ord11111(u, i); e->ord11108(i); }
        if (at<int16_t>(r, 0x28) >= 0) e->f38a70(u, 0, 0, 0);
    }
    e->ord10945(u);
    if (r[0x21]) e->f22130(u);
    int16_t s = at<int16_t>(r, 0xc);
    if (s >= 0) {
        if (s != -1) { e->ord10799(s); e->f1380(u + 0x20); }
        e->f1b300(u, 4, 0, -1, -1);
    }
    if (at<int16_t>(r, 2) >= 0) e->f227f0(at<int16_t>(r, 2));
    if (at<int16_t>(r, 4) >= 0) e->f227f0(at<int16_t>(r, 4));
    if (e->gdwBitMasks[12] & r[0x10]) e->f234d0(e->gdwBitMasks);
    e->ord10819(u, "..\\Source\\D2Client\\UNIT\\PlrSkills.cpp", 0x59c);
}

// 1.13c D2Client+0x73760  (eax=unit)
EMSCRIPTEN_KEEPALIVE void d2c_73760(const D2cB65Env* e, uint8_t* u, int32_t idx) {
    const uint8_t* dt = *e->sgptDataTables;
    if (!u || idx < 0 || idx >= at<int32_t>(dt, 0xc4)) return;
    const uint8_t* r = at<const uint8_t*>(dt, 0xbc) + idx * 0x3c;
    if (!r) return;
    int32_t t = at<int32_t>(u, 0), m = at<int32_t>(u, 0x10);
    if ((e->gdwBitMasks[0x1c] & r[0x14]) && ((t == 1 && m == 0xc) || (t == 0 && m == 0x11))) { e->ord10945(u); return; }
    if (!e->ord10494(u)) { if (at<int16_t>(r, 0x26) >= 0) e->f38a70(u, 0, 0, 0); }
    else { int32_t i = e->ord10871(u); if (!(e->gdwBitMasks[0x10] & r[0x14])) e->ord10343(i); }
    e->ord10945(u);
    if (r[0x21]) e->f22130(u);
    e->f22c80(idx);
    if (e->f55440() && e->ord10341()) {
        at<uint32_t>(u, 0xc8) |= 0x40000;
        at<uint32_t>(u, 0xc4) = (at<uint32_t>(u, 0xc4) & 0xfffffffdu) | 0x20;
    }
    if (e->gdwBitMasks[12] & r[0x10]) e->f234d0((const void*)(uintptr_t)r[0x10]);
}

static void buysell(const D2cB65Env* e, const int32_t* flag, int32_t x, int32_t y) {
    int32_t rec[18] = {};
    if (!*e->g_6fbcb9c4) *e->g_6fbcb9c4 = e->fadb420(0);  // wsprintfA path string built first in original
    rec[0] = (*flag != 0) + 10;
    { char* s = *e->g_6fbcb9c4; __builtin_memcpy(rec + 11, &s, sizeof s); }  // ponytail: 32-bit slot in original
    e->ord10041(rec, x, y, -1, 5, 0);
}
// 1.13c D2Client+0x76050
EMSCRIPTEN_KEEPALIVE void d2c_76050(const D2cB65Env* e) {
    buysell(e, e->g_6fbcbff4, *e->g_6fbcb9a0 + 0x116, 0x1a5 - *e->g_6fbcb9a4);
}
// 1.13c D2Client+0x78240
EMSCRIPTEN_KEEPALIVE void d2c_78240(const D2cB65Env* e) {
    int32_t v = e->g_6fb925f4[*e->g_6fb924f8 + (uint32_t)*e->g_6fbc991c * 3];
    if (v == -0x95) *e->g_6fb925f0 = 1; else if (v == -0xdc) *e->g_6fb925f0 = 2; else if (v == -0x131) *e->g_6fb925f0 = 3;
    buysell(e, e->g_6fbcbfb4, v + (*e->g_6fb8bc48 - *e->g_6fbcb9a0), *e->g_6fbcb9a4 - 0x3f + *e->g_6fb8bc4c);
}
}
