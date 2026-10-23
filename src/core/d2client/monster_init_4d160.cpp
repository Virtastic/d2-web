// src/core/d2client/monster_init_4d160.cpp -- D2Client+0x4D160 (monster client-unit init), from decomp 0x4D160.c.
// Ordinals / unrecovered callees are host callbacks; globals are Env fields.
#include <cstdint>
#include <emscripten.h>

struct M4D160Env {
    uint32_t monCount;          // *(sgptDataTables+0)+0xA80
    uint8_t* monBase;           // *(sgptDataTables+0)+0xA78, stride 0x1A8
    uint32_t bitMasks[32];      // gdwBitMasks
    uint32_t dbgLevel;          // DAT_6fbcc390
    uint32_t dat9854;           // DAT_6fbc9854
    uint32_t (*f1260)(int32_t);                 // record lookup (0 = absent)
    uint32_t (*f1380)(uint64_t* seed);          // seed rand
    uint32_t (*f1700)();                        // monster index of current desc
    uint32_t (*f1c10)(uint32_t, uint32_t);
    uint32_t (*f6c20)(uint32_t, uint32_t);
    void (*f4c370)(int32_t* u, uint32_t);
    void (*f4cec0)(int32_t* u);
    void (*f4c150)();
    void (*f20f20)();
    void (*fatal)(uint32_t id);                 // ord10265+ord10024+_exit
    uint32_t (*ord)(uint32_t n, uint32_t a, uint32_t b, uint32_t c, uint32_t d); // D2 ordinals, in call order
};

// 1.13c D2Client+0x4D160  (EAX = u (int*), stack arg = nClass)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_4d160(const M4D160Env* e, int32_t* u, int32_t cls) {
    if (cls < 0 || (uint32_t)cls >= e->monCount) return;
    uint8_t* r = e->monBase + (uint32_t)cls * 0x1a8;
    if (!r || !e->f1260(0)) return;
    if (e->dbgLevel > 2) { e->fatal(0x1f0); return; }
    e->ord(11166, 0, 0, 0, 0);
    for (int i = 0; i < 10; i++) e->ord(10887, 0, 0, 0, 0);
    e->ord(11136, e->dat9854, 0, 0, 0);
    e->ord(10887, 0, 0, 0, 0);
    e->ord(10887, 0, 0, 0, 0);
    e->ord(11003, 0, 0, (uint32_t)(uintptr_t)u, e->dat9854);
    uint32_t iv4 = (uint32_t)(int32_t)(*(int16_t*)(r + 0x32)) << 8;   // pointer-like value, as retail
    e->ord(10488, (uint32_t)u[0xb], iv4, 0, 0);
    if (u[0x18] == 0 && ((e->bitMasks[4] & r[0xd]) || cls == 0x123 || cls == 0x165 || cls == 0x1a1 || cls == 0x1a2))
        u[0x18] = (int32_t)e->ord(10387, 0, 0, 0, 0);
    e->ord(11090, 0, 0, 0, 0);
    if (e->ord(10648, 0, 0, 0, 0) && !(e->bitMasks[0xc] & *(uint8_t*)(uintptr_t)(iv4 + 6))) {
        e->ord(10775, 0, 0, 0, 0);
        e->ord(10671, 0, 0, 0, 0);
        e->ord(11056, (uint32_t)u[0xb], 0x8000, 0, 0);
    }
    u[0x11] = (int32_t)e->f1380((uint64_t*)(u + 8));
    uint32_t f = (uint32_t)u[0x31];
    f = (e->bitMasks[0xc] & *(uint8_t*)(uintptr_t)(iv4 + 4)) ? f | 2 : f & ~2u;
    f = (*(uint8_t*)(uintptr_t)(iv4 + 5) & e->bitMasks[0x18]) ? f & ~0x20u : f | 0x20;
    f |= 8;
    f = (e->bitMasks[4] & *(uint8_t*)(uintptr_t)(iv4 + 5)) ? f | 0xc : (f & ~4u) | 8;
    u[0x31] = (int32_t)f;
    e->f4c370(u, 0);
    e->f4cec0(u);
    e->f4c150();
    u[0x2a] = (int32_t)e->ord(10856, 0, 0, 0, 0);
    e->f20f20();
    e->ord(10237, iv4, 0, 0, 0);
    for (int i = 0; i < 8; i++) {
        if (*(int16_t*)(r + 0x170 + i * 2) >= 0 && (int8_t)r[0x198 + i] > 0) {
            e->ord(10302, 0, 0, 0, 0);
            uint32_t v = e->ord(10630, (uint32_t)(int32_t)(int8_t)r[0x180 + i], 0, 0, 0);
            e->ord(10368, v, 0, 0, 0);
        }
    }
    e->ord(10182, 0, 0, 0, 0);
    int32_t c2 = u[1];
    const uint8_t* r2 = (c2 >= 0 && (uint32_t)c2 < e->monCount) ? e->monBase + (uint32_t)c2 * 0x1a8 : nullptr;
    if (!r2 || !(r2[0xd] & e->bitMasks[0])) {
        uint32_t v = 0;
        int32_t idx = (int32_t)e->f1700();
        if (idx < 0 || (uint32_t)idx >= e->monCount) idx = -1;
        if (e->f1c10(0, 2)) {
            uint64_t s = (uint64_t)(uint32_t)u[8] * 0x6ac690c5u + (uint32_t)u[9];
            *(uint64_t*)(u + 8) = s;
            v = (uint8_t)s & 0x3f;
        }
        if (idx == 0x60) v = 7;
        else if (idx == 0x12d) v = e->f6c20(0, 0) & 0xff;
        e->ord(10706, iv4, v, 0, 0);
    }
}
