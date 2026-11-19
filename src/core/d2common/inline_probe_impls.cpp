// Engine implementations behind FN probes d2common_{34690,38020,3c000,34ce0,34d10,34cb0,36c20,23be0,22510,306d0}.
// Operate on retail's 32-bit image through the low 32 bits of a wasm address (same as sub_34d40.cpp).
#include <cstdint>
#include "D2Items.h"
#define P(x) ((uint8_t*)(uintptr_t)(x))
// Export of 1.13c D2Common 0x23be0; the body is ITEMS_InitItemSeed in src/core/game/Items/Items.cpp.
void D2Common_23BE0(uint32_t pu) {
    ITEMS_InitItemSeed((D2UnitStrc*)P(pu));
}
// 1.13c D2Common+0x22510
void D2Common_22510(uint32_t pu, uint32_t v) {
    const uint32_t* u = (const uint32_t*)P(pu);
    if (u[0] == 4 && u[5]) *(uint32_t*)(P(u[5]) + 0xC) = v;
}
// D2Common 0x23740
uint32_t D2Common_23740(uint32_t u) {
    const uint8_t* it = nullptr;
    if (u && *(uint32_t*)P(u) == 4) it = P(*(uint32_t*)(P(u) + 0x14));
    return it ? it[0x47] : 0xFF;
}
// D2Common 0x2c990
uint32_t D2Common_2C990(uint32_t a, uint32_t b) { return (a + 1) * a * a * b; }
// 1.13c D2Game+0x1720
uint32_t D2Game_1720(uint32_t u, uint32_t x) { return *(uint32_t*)(P(*(uint32_t*)(P(u) + 0x14)) + 0x1c + 4u * P(x)[0x6d]); }
// 1.13c D2Game+0x20c60
uint32_t D2Game_20C60(uint32_t u) { return *(uint32_t*)(P(*(uint32_t*)(P(*(uint32_t*)(P(u) + 0x14)) + 0x28)) + 0x3c); }
// 1.13c D2Game+0xdf9a0
uint32_t D2Game_DF9A0(uint32_t u) {
    uint32_t t = *(uint32_t*)P(u), room = 0;
    if (t == 2 || t == 4 || t == 5) room = *(uint32_t*)P(*(uint32_t*)(P(u) + 0x2c));
    return room && *(uint32_t*)(P(room) + 0x78) ? 1 : 0;
}
// stand-alone 32-bit layout twin of D2Common 0x4E1D0 (the proven row runs SKILLS_GetSkillIdFromSkill): p ? (int16)**p : 0
uint32_t D2Common_4E1D0(uint32_t p) { return p ? (uint32_t)(int32_t)*(int16_t*)P(*(uint32_t*)P(p)) : 0; }
// 1.13c D2Game+0x0373C entry (CRT DllMain wrapper around DllMain body 0x131D0).
// ponytail: no CRT/_pRawDllMain in wasm; DllMain body is a no-op that succeeds, so return 1.
uint32_t D2Game_373C(uint32_t, uint32_t, uint32_t) { return 1; }
