// 1.13c D2Game+0xA44C0: may this monster pick skill `skill`? Written from the disassembly: the
// decompile drops the stack argument that becomes 0xA3D60's EAX, and the tests use EBX (the unit) for 10494/10871 except the
// class-2 second state, which uses ctx[+8]; the passive test reads gdwBitMasks[2] (a byte test), not [8].
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_a44c0.h"

typedef uintptr_t A;
static inline uint32_t rd32(A p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline int16_t rd16s(A p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint8_t rd8(A p, uint32_t o) { return ((const uint8_t*)p)[o]; }

// 1.13c D2Game+0xA44C0
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_game_A44C0(A ctx, int32_t skill, A unit, uint32_t arg2, const D2GameA44C0Env* e)
{
    A unit8 = (A)rd32(ctx, 8);
    uint32_t boss = rd32(ctx, 0x18);
    if (skill < 0 || skill >= (int32_t)rd32(e->dt, 0xBA0)) return 0;
    A row = (A)((uint32_t)skill * 0x23C + rd32(e->dt, 0xB98));
    if (!row) return 0;
    if (!e->f3D60(arg2, skill, unit)) return 0;
    uint8_t cls = rd8(row, 0x230);
    if (boss) { if (cls != 4 && cls != 0xD) return 0; }
    else if (cls == 4 || cls == 0xD) return 0;
    if (cls == 1 && rd16s(row, 0x80) > 0 && e->o10494(unit, rd16s(row, 0x80))) return 0;
    if (!unit8 && (uint8_t)(cls - 2) <= 11 && ((0xE0Du >> (cls - 2)) & 1)) return 0;   // cls 2,4,5,0xB,0xC,0xD
    if (cls == 2) {
        if (rd16s(row, 0x80) > 0 && e->o10494(unit, rd16s(row, 0x80))) return 0;
        if (rd16s(row, 0x82) > 0 && e->o10494(unit8, rd16s(row, 0x82))) return 0;
    }
    if ((uint8_t)e->masks[2] & rd8(row, 4)) {
        int16_t s = rd16s(row, 0x80);
        if (s > 0 && e->o10494(unit, s)) {
            A r = e->o10871(unit, rd16s(row, 0x80));
            if (r && e->o10680(r, rd16s(row, 0x54), 0) >= 3) return 0;
        }
    }
    return 1;
}
