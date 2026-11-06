// 1.13c D2Client+0x596A0 (145 B, ESI = limit, plain ret): if the exit state DAT 0x11C2CC is not 4, 5 or 6, read stat 0xE of the
// player unit DAT 0x10BBFC (Ordinal 10973 through thunk 0xC1C6: stack {unit, 0xE, 0}, ret 12), store min(limit, value) in DAT
// 0x1177EC (limit wins when limit < value, unsigned) and send it through 0x149A0 (AL = 'O', ECX = 8, EDX = value >> 16, one stack
// word = the full value, ret 4). In states 4..6 send {'O', 7, 0, 0, 0, 0, 0} through 0x143E0 (EBX = 7) and set the state to 3.
#include <emscripten.h>
#include <cstdint>
struct D2c596a0Env {
    uint32_t (*c1c6)(uint32_t unit, uint32_t stat, uint32_t layer);
    void (*c149a0)(uint32_t al, uint32_t ecx, uint32_t edx, uint32_t value);
    void (*c143e0)(const uint8_t* buf, uint32_t ebx);
};
extern "C" void d2c_port_596a0(uint32_t esi, const uint32_t* g10bbfc, uint32_t* g1177ec, uint32_t* g11c2cc, const D2c596a0Env* e) {
    uint32_t st = *g11c2cc;
    if (st != 4 && st != 5 && st != 6) {
        uint32_t v = e->c1c6(*g10bbfc, 0xE, 0);
        if (esi < v) {
            *g1177ec = esi;
            e->c149a0(0x4F, 8, esi >> 16, esi);
        } else {
            *g1177ec = v;
            e->c149a0(0x4F, 8, v >> 16, v);
        }
        return;
    }
    const uint8_t p[8] = {0x4F, 7, 0, 0, 0, 0, 0, 0};
    e->c143e0(p, 7);
    *g11c2cc = 3;
}
