// D2Client+0xA2C90: per-skill-record client effect dispatch (stdcall, 4 stack args); env-seam style.
// Stack args: unit, idx (skills table index), p3, p4. Every callee is a seam so the register inputs retail passes are explicit.
#include <cstdint>
#include <emscripten.h>
struct D2cA2C90Env {
    const uint8_t* (*rec)(int32_t idx);                  // sgptDataTables+0xb98 table, 0x23c stride; null if idx>=count(+0xba0)
    int32_t handlerCount;                                // DAT_6fb8ec50
    int32_t bitMask4;                                    // gdwBitMasks[4]
    int32_t (*hasHandler)(int32_t i);                    // DAT_6fb8ea48[i] != 0
    int32_t (*callHandler)(int32_t i, int32_t unit, int32_t idx, int32_t p3);  // ECX=unit, EDX=idx, stack p3; returns EAX
    void (*f51340)(int32_t unit, int32_t* pY, int32_t* pX);   // EBX=unit, EDI=&X (local+0x20), stack arg=&Y (local+0x1c)
    int32_t (*f1630)(int32_t unit);                      // ECX=unit
    int32_t (*f1660)(int32_t unit);                      // ECX=unit
    int32_t (*f526b0)(int32_t unit, int32_t esi, int32_t e8, int32_t idx, int32_t p3, int32_t a, int32_t b, int32_t l10);  // EAX=unit, ESI=X+a, stack e8..l10
    int32_t (*f69a80)(int32_t unit, int32_t e8, int32_t a, int32_t b, int32_t l14, int32_t l10, int32_t idx, int32_t p3, int32_t z); // EAX=unit
    int32_t (*ord10327)(int32_t unit, int32_t z);        // Ordinal_10327
    void (*f58630)(int32_t esi);                         // ESI = ord10327 result
    void (*f38a70)(int32_t ebx, int32_t a, int32_t b, int32_t c, int32_t d);  // EBX=short(rec+0x100 / +0x10a)
    int32_t (*f55f70)();                                 // +0xA5F70
    void (*f1b300)(int32_t eax, int32_t u, int32_t two, int32_t z0, int32_t z1, int32_t z2, int32_t z3, int32_t z4); // EAX=short(rec+0x108), 7 stack words
    int32_t (*ord10786)(int32_t unit, int32_t rec190, int32_t idx, int32_t p3);  // Ordinal_10786
    int32_t gCbbfc;                                      // DAT_6fbcbbfc (value)
    int32_t* g985c;                                      // DAT_6fbc985c
    int32_t g97f8;                                       // DAT_6fbc97f8
};
static int16_t s16(const uint8_t* p, int o) { return (int16_t)(p[o] | (p[o + 1] << 8)); }
static int32_t s32(const uint8_t* p, int o) { return (int32_t)(p[o] | (p[o+1] << 8) | (p[o+2] << 16) | ((uint32_t)p[o+3] << 24)); }
extern "C" {
// 1.13c D2Client+0xA2C90
EMSCRIPTEN_KEEPALIVE int32_t d2c_a2c90(const D2cA2C90Env* e, int32_t* u, int32_t idx, int32_t p3, int32_t p4) {
    const int32_t unit = (int32_t)(intptr_t)u;
    const uint8_t* r = (idx < 0) ? nullptr : e->rec(idx);
    if (!r || s16(r, 0xf4) < 0 || e->handlerCount <= s16(r, 0xf4)) return 0;
    int32_t flag = 0;
    if (s16(r, 0xe8) > 0) {
        int32_t X = 0, Y = 0, l14 = 0, l10 = 0, a = 0, b = 0;
        if (p4) {
            int32_t y = 0, x = 0;
            e->f51340(unit, &y, &x);
            Y = y; X = x; a = X; b = Y;           // ebx = X, edi = Y
            if (X && Y) {
                l14 = X; l10 = Y;
                a = X - e->f1630(unit);
                b = Y - e->f1660(unit);
                l14 = X + a;
                l10 = Y + b;
            }
        }
        int32_t rc;
        if ((e->bitMask4 & r[4] & 0xff) == 0) rc = e->f526b0(unit, l14, s16(r, 0xe8), idx, p3, a, b, l10);
        else rc = e->f69a80(unit, s16(r, 0xe8), a, b, l14, l10, idx, p3, 0);
        if (rc) flag = 1;
        if (u) u[0x31] |= 0x40;
    }
    int32_t h = s16(r, 0xf4);
    if (p4 && s16(r, 0x16c) > 1) h = s16(r, 0x16c);
    if (h < 0 || e->handlerCount <= h || !e->hasHandler(h)) {
        if (u) u[0x31] |= 0x40;
        flag = 1;
    } else if (e->callHandler(h, unit, idx, p3)) flag = 1;
    int32_t t10327 = e->ord10327(unit, 0);
    if (t10327) e->f58630(t10327);
    if (!flag) return 0;
    if (s16(r, 0x100) > 0) e->f38a70(s16(r, 0x100), unit, 0, 0, 0);
    if (s16(r, 0x10a) > 0) { int32_t t = e->f55f70(); if (t) e->f38a70(s16(r, 0x10a), t, 0, 0, 0); }
    if (s16(r, 0x108) > 0) { int32_t t = e->f55f70(); if (t) e->f1b300(s16(r, 0x108), t, 2, 0, 0, 0, 0, 0); }
    int32_t n = e->ord10786(unit, s32(r, 400), idx, p3);
    if (n > 0 && e->gCbbfc == unit) {
        if (n > 0xc) n = 0xc;
        if (r[0x10] == 0x12) {
            if ((u[0xe] & 0xffffff00u) == 0) { *e->g985c = e->g97f8 + n; return 1; }
        } else *e->g985c = e->g97f8 + n;
    }
    return 1;
}
}
