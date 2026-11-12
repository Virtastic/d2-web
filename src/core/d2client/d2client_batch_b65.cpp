// D2Client batch b65-cli: env-seam style; globals and callees enter through one struct.
// Hidden register inputs (EAX/ESI/ECX) are explicit leading params.
// Not implemented: 6E2C0 (jump table), 6F0B0/6F4A0 (direct Imm* Win32), 78820 (unrecovered stack outputs).
#include <cstdint>
#include <initializer_list>
#include <emscripten.h>
#include "d2client_callee.h"
template <class T> static inline T& at(const void* b, int32_t o) { return *(T*)((const uint8_t*)b + o); }
struct D2cB65Env {
    void (*fatal)(int32_t code);                         // Ordinal_10265(code); Ordinal_10024(); _exit(-1)
    const uint32_t* t_6fbc3fb8; const uint32_t* t_6fbc4018;
    uint32_t (*f1510)(const void* p, int32_t n);         // FUN_6fab1510
    uint32_t (*ord10491)(uint32_t a, uint32_t b, uint32_t c);
    int32_t (*f1d540)(const char* s);                    // D2Client+0x6D540 (ecx hidden); returns the dword it stores through EAX (a zeroed stack local)
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
        int32_t got = e->f1d540((const char*)(ent + 5));
        at<int32_t>(esi, 0x28) = got;
        if (at<int32_t>(esi, 0x1c) == 0) { e->fatal(0x425); return 0; }
        if (at<int32_t>(esi, 0x18) == 0) { e->fatal(0x426); return 0; }
        if (got == 0) { e->fatal(0x427); return 0; }
        if (at<uint32_t>(esi, 0x20) == 0) at<uint32_t>(esi, 0x20) = 0x20787878;
        if (at<uint32_t>(esi, 0x24) == 0) at<uint32_t>(esi, 0x24) = 0x20787878;
        return 1;
    }
    return 0;
}

// 1.13c D2Client+0x6E670  (eax=unit; stdcall p1 = the 6D700 key)
// Seams: the callees of the routine and the five globals it reads.
// The unit+0x54 == 0 path logs, then _exit, which falls into the next function in retail: returns 0 here, never captured.
struct D2cB6E670Env {
    int32_t (*d07e)(); int32_t (*c232)(const uint8_t*, int32_t*, int32_t*); int32_t (*c1ae)(const uint8_t*, int32_t); void (*d084)(int32_t);
    int32_t (*c1d2)(const uint8_t*); int32_t (*c1f6)(const uint8_t*); int32_t (*a3d0)(const uint8_t*, int32_t, int32_t);
    int32_t (*cbda)(const uint8_t*, int32_t); const uint8_t* (*a60d0)(const uint8_t*);
    int32_t (*c2aa)(const uint8_t*, int32_t, int32_t*, int32_t, int32_t); int32_t (*f1af0)(); int32_t (*f6a2f0)(int32_t, int32_t, const uint8_t*);
    int32_t g414, gbc48, g418, g960, g995c;
};
static uint8_t lookup6d700(const uint8_t* t, int32_t key) {   // the 6D700 table lookup (ecx=table, edi=key)
    if (!t || key >= 0x10) return 0;
    const uint8_t* p = t + 0x1c;
    for (int i = 0; i < t[0]; i++, p += 9) if (*p == (uint32_t)key) return p[2];
    return 0;
}
EMSCRIPTEN_KEEPALIVE uint8_t d2c_6e670(const D2cB6E670Env* e, const uint8_t* unit, int32_t key) {
    int32_t A = 0, B = 0;
    if (e->d07e()) {
        int32_t r232 = e->c232(unit, &A, &B);
        e->d084(e->c1ae(unit, r232));
        int32_t q = (e->gbc48 + ((e->gbc48 >> 31) & 3)) >> 2;
        if (e->g414 == 1) A -= q; else if (e->g414 == 2) A += q;
    } else {
        A = e->c1d2(unit) - (e->g960 - e->g418);
        B = e->c1f6(unit) - (e->g995c - 8);
    }
    if (!e->a3d0(unit, A, B)) return 0;
    const uint8_t* hu = unit;
    if (unit && ((at<uint32_t>(unit, 0xc8) >> 3) & 1) && e->cbda(unit, 0x25)) { const uint8_t* r = e->a60d0(unit); if (r) hu = r; }
    int32_t loc2 = 0;
    int32_t k = e->c2aa(unit, at<int32_t>(hu, 0x60), &loc2, -1, 1);
    const uint8_t* sub = at<const uint8_t*>(unit, 0x54);
    if (!sub) return 0;
    int32_t v = e->f1af0();
    int32_t* n = at<int32_t*>(sub, 0x28);
    for (; n; n = (int32_t*)(intptr_t)n[4]) if (n[0] == v && n[1] == k) break;
    if (!n && !e->f6a2f0(-1, k, unit)) return 0;
    int32_t v2 = e->f1af0();
    sub = at<const uint8_t*>(unit, 0x54);
    if (!sub) return 0;
    for (n = at<int32_t*>(sub, 0x28); n; n = (int32_t*)(intptr_t)n[4])
        if (n[0] == v2) {
            const uint8_t* p = at<const uint8_t*>((const void*)(intptr_t)n[2], 0x14);
            return p ? lookup6d700(p, key) : 0;
        }
    return 0;
}

// D2Client 76050 moved to d2client_76050.cpp.
// 1.13c D2Client+0x78240: the buy/sell button cell, drawn through D2Gfx#10041 (0xD096 thunk). The draw block is 18 dwords:
// [0] = 10 + (flag != 0), [13] (+0x34) = the lazily loaded cell (0x2B420 loader, EAX = the path string), [16] = 0.
// g925f0 = [0xE25F0] (set 1/2/3 by the column x -149/-220/-305), g925f4 = the table at 0xE25F4 (12-byte stride per class,
// 4 per [0xE24F8]), cls = byte [0x11991C], b8bc48/b8bc4c = [0xDBC48]/[0xDBC4C], b9a0/b9a4 = [0x11B9A0]/[0x11B9A4],
// b9c4 = the cell global [0x11B9C4], bfb4 = [0x11BFB4].
EMSCRIPTEN_KEEPALIVE void d2c_78240(const D2cSvc& v, uint32_t* g925f0, const uint8_t* g925f4, uint32_t g924f8, uint8_t cls,
                                    int32_t b8bc48, int32_t b8bc4c, int32_t b9a0, int32_t b9a4, uint32_t* b9c4, int32_t bfb4) {
    int32_t x = at<int32_t>(g925f4, (int32_t)(g924f8 * 4 + cls * 12));
    if (x == -0x95) *g925f0 = 1; else if (x == -0xdc) *g925f0 = 2; else if (x == -0x131) *g925f0 = 3;
    x += b8bc48 - b9a0;
    uint32_t blk[18] = {};
    char path[0x100] = "DATA\\GLOBAL\\ui\\panel\\buysellbtn";
    if (!*b9c4) *b9c4 = d2c_call(v, 0x2b420, (uint32_t)(uintptr_t)path, 0, 0, 0, 0, 0);
    blk[13] = *b9c4;
    blk[0] = (bfb4 != 0) + 10;
    d2c_call(v, 0xd096, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)blk, (uint32_t)x, (uint32_t)(b9a4 + b8bc4c - 0x3f), 0xffffffffu, 5, 0);
}

}
