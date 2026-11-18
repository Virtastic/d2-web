// 1.13c D2Client+0x4CEC0: EAX = message id, ECX = unit (ESI inside); plain ret. A small switch over the unit-spawn / sound messages:
//  0x96 stores the unit in DAT_6FBC9650; 0x9D/0x9E/0x13F: D2Common 10671(unit, 5), 11164(u[0xB], 0); 0x14B: with DAT_6FBCBBFC set, D2Common 11129(unit, DAT) and 0xA5FB0(unit, 0);
//  0x158: one LCG step (x*0x6AC690C5 + hi, 64-bit) of the seed at DAT_6FBCBBFC+0x20, then D2Common 10520(h,1), 10706(h, lo&0x3F), 10520(h,0) with h = u[0xB];
//  0x19D / 0x23A: 0xAFF10 spawn (class 0x188 / 0x265) at the unit's (0x1630, 0x1660) position, 0x19D then D2Common 10210(result, unit);
//  0x1F1..0x1F4: 0x4CDD0(unit).
#include <emscripten.h>
#include <cstdint>
struct D2c4cec0Env {
    uint32_t* seed;                                 // *(DAT_6FBCBBFC) as a pointer (null when the global is 0); words 8 / 9 are the 64-bit seed
    uint32_t* g9650;                                // &DAT_6FBC9650
    void (*f4cdd0)(const int32_t* unit);            // ESI = unit
    void (*o10671)(const int32_t* unit, uint32_t a);
    void (*o11164)(uint32_t h, uint32_t a);
    void (*o10520)(uint32_t h, uint32_t a);
    void (*o10706)(uint32_t h, uint32_t a);
    void (*o11129)(const int32_t* unit, const uint32_t* seed);
    void (*fa5fb0)(const int32_t* unit, uint32_t a); // EAX = unit
    int32_t (*f1630)(const int32_t* unit);
    int32_t (*f1660)(const int32_t* unit);
    uint32_t (*faff10)(const int32_t* unit, uint32_t cls, int32_t x, int32_t y, uint32_t a, uint32_t b); // EDX = unit
    void (*o10210)(uint32_t rec, const int32_t* unit);
};
extern "C" void d2_client_4cec0(int32_t msg, int32_t* u, const D2c4cec0Env* e) {
    switch (msg) {
    case 0x96: *e->g9650 = (uint32_t)(uintptr_t)u; return;
    case 0x9d: case 0x9e: case 0x13f:
        e->o10671(u, 5); e->o11164((uint32_t)u[0xb], 0); return;
    case 0x14b:
        if (e->seed) { e->o11129(u, e->seed); e->fa5fb0(u, 0); }
        return;
    case 0x158: {
        uint64_t t = (uint64_t)e->seed[8] * 0x6ac690c5u + e->seed[9];
        e->seed[8] = (uint32_t)t; e->seed[9] = (uint32_t)(t >> 32);
        uint32_t h = (uint32_t)u[0xb];
        e->o10520(h, 1); e->o10706(h, (uint32_t)t & 0x3f); e->o10520(h, 0);
        return;
    }
    case 0x19d: {
        int32_t y = e->f1660(u), x = e->f1630(u);
        uint32_t r = e->faff10(u, 0x188, x, y, 0, 1);
        if (r) e->o10210(r, u);
        return;
    }
    case 0x1f1: case 0x1f2: case 0x1f3: case 0x1f4: e->f4cdd0(u); return;
    case 0x23a: {
        int32_t y = e->f1660(u), x = e->f1630(u);
        e->faff10(u, 0x265, x, y, 0, 1);
        return;
    }
    default: return;
    }
}
