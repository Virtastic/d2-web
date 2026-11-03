// src/core/d2client/monster_init_4d160.cpp -- D2Client+0x4D160 (monster client-unit init), from decomp 0x4D160.c.
// Ordinals / unrecovered callees are host callbacks; globals are Env fields.
#include <cstdint>
#include <emscripten.h>

struct M4D160Env {
    uint32_t monCount;          // *(sgptDataTables+0)+0xA80
    uint8_t* monBase;           // *(sgptDataTables+0)+0xA78, stride 0x1A8
    uint32_t bitMasks[32];      // gdwBitMasks
    uint32_t dbgLevel;          // byte at DAT_6fbcc390 (difficulty)
    uint32_t dat9854;           // DAT_6fbc9854
    uint8_t* (*f1260)(int32_t cls);             // EAX = cls; record or 0
    uint32_t (*f1380)(uint64_t* seed);          // seed rand, ECX = seed
    uint32_t (*f1700)(int32_t* u);              // EAX = u
    uint32_t (*f1c10)(uint32_t eax, uint32_t edx);
    uint32_t (*f6c20)(uint32_t, uint32_t);
    void (*f4c370)(int32_t* u, uint32_t edx);   // EAX = u, EDX = 0
    void (*f4cec0)(int32_t* u, int32_t cls);    // EAX = cls, ECX = u
    void (*f4c150)(int32_t* u);                 // EAX = u
    void (*f20f20)();
    void (*fatal)(uint32_t id);                 // ord10265+ord10024+_exit
    // D2 ordinals, stdcall, args in push order (first argument first); a..f unused slots are 0.
    uint32_t (*ord)(uint32_t n, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f);
    uint32_t (*rd32)(uint32_t addr);            // guest dword read
    uint32_t (*fc838)(int32_t* u, uint32_t edx, uint32_t stackArg);   // ECX=u, EDX=skill, one stack arg
};

// 1.13c D2Client+0x4D160  (EAX = u, stack args a1..a5 = 0x14 bytes; cls = a4)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_4d160(const M4D160Env* e, int32_t* u, int32_t a1, int32_t a2, int32_t a3, int32_t cls, int32_t a5) {
    if (cls < 0 || (uint32_t)cls >= e->monCount) return;
    uint8_t* ms = e->monBase + (uint32_t)cls * 0x1a8;
    if (!ms) return;
    uint8_t* m2 = e->f1260(cls);
    if (!m2) return;
    uint32_t d = e->dbgLevel & 0xff;
    if (d >= 3) { e->fatal(0x1f0); return; }
    const uint32_t U = (uint32_t)(uintptr_t)u;
    auto stat = [&](uint32_t id, uint32_t v) { e->ord(10887, U, id, v, 0, 0, 0); };
    e->ord(11166, U, 0, 0, 0, 0, 0);
    stat(0xc, (uint32_t)(int32_t)*(int16_t*)(ms + 0xaa + d * 2));
    stat(0x44, 100); stat(0x43, 0x4b); stat(0x45, 100);
    static const uint8_t ids[6] = {0x24, 0x25, 0x27, 0x29, 0x2b, 0x2d};
    for (int i = 0; i < 6; i++) stat(ids[i], (uint32_t)(int32_t)*(int16_t*)(ms + 0x144 + i * 6 + d * 2));
    e->ord(11136, U, e->dat9854, d, 0, 0, 0);
    stat(7, 0x6400); stat(6, 0x6400);
    e->ord(11003, 0, (uint32_t)a1, (uint32_t)a2, (uint32_t)a3, U, 1);
    uint32_t w0b = (uint32_t)u[0xb];
    e->ord(10488, w0b, (uint32_t)((int32_t)*(int16_t*)(ms + 0x32) << 8), 0, 0x208, 0, 0);
    if (u[0x18] == 0 && ((e->bitMasks[1] & ms[0xd]) || cls == 0x123 || cls == 0x165 || cls == 0x1a1 || cls == 0x1a2))
        u[0x18] = (int32_t)e->ord(10387, 0, U, 0, 0, 0, 0);
    e->ord(11090, U, (uint32_t)a5, 0, 0, 0, 0);
    if (e->ord(10648, U, 0, 0, 0, 0, 0) && !(e->bitMasks[3] & m2[6])) {
        e->ord(10775, U, 1, 0, 0, 0, 0);
        e->ord(10671, U, 5, 0, 0, 0, 0);
        e->ord(11056, w0b, 0x8000, 0, 0, 0, 0);
    }
    u[0x11] = (int32_t)e->f1380((uint64_t*)(u + 8));
    uint32_t f = (uint32_t)u[0x31];
    f = (e->bitMasks[3] & m2[4]) ? f | 2 : f & ~2u;
    f = (m2[5] & e->bitMasks[6]) ? f & ~0x20u : f | 0x20;
    f |= 8;
    f = (e->bitMasks[1] & m2[5]) ? f | 4 : f & ~4u;
    u[0x31] = (int32_t)f;
    e->f4c370(u, 0);
    e->f4cec0(u, cls);
    e->f4c150(u);
    u[0x2a] = (int32_t)e->ord(10856, 0, 0, 0, 0, 0, 0);
    e->f20f20();
    uint32_t rec = e->ord(10237, d, 0, 0, 0, 0, 0);
    uint32_t x = e->rd32(rec + 0x10);
    for (int i = 0; i < 8; i++) {
        int16_t sk = *(int16_t*)(ms + 0x170 + i * 2);
        int8_t lv = (int8_t)ms[0x198 + i];
        if (sk >= 0 && lv > 0) {
            e->ord(10302, U, (uint32_t)(int32_t)sk, (uint32_t)((int32_t)lv + (int32_t)x), 1, 0, 0x23f);
            uint32_t v = e->fc838(u, (uint32_t)(int32_t)sk, (uint32_t)(int32_t)(int8_t)ms[0x180 + i]);
            e->ord(10368, v, 0, 0, 0, 0, 0);
        }
    }
    e->ord(10182, U, x, 0, 0, 0, 0);
    int32_t c2 = u[1];
    const uint8_t* r2 = (c2 >= 0 && (uint32_t)c2 < e->monCount) ? e->monBase + (uint32_t)c2 * 0x1a8 : nullptr;
    if (!r2 || !(r2[0xd] & e->bitMasks[0])) {
        uint32_t v = 0;
        int32_t idx = (int32_t)e->f1700(u);
        if (idx < 0 || (uint32_t)idx >= e->monCount) idx = -1;
        if (e->f1c10((uint32_t)c2, 2)) {
            uint64_t s = (uint64_t)(uint32_t)u[8] * 0x6ac690c5u + (uint32_t)u[9];
            *(uint64_t*)(u + 8) = s;
            v = (uint8_t)s & 0x3f;
        }
        if (idx == 0x60) v = 7;
        else if (idx == 0x12d) v = e->f6c20(0, 0) & 0xff;
        if (cls == 0x15f) v = 0x18;
        else if (cls == 0x160) v = 0;
        else if (cls == 0x161) v = 0x28;
        else if (cls == 0x165 || cls == 0x158) v = 0;
        e->ord(10706, w0b, v, 0, 0, 0, 0);
    }
}
