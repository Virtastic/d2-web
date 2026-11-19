// 1.13c D2Common+0x306D0: refresh the player data's switch-right-skill id and flags (+0x70/+0x78) of a type-0 unit.
// Operates on retail's 32-bit image through the low 32 bits of a wasm address; the FN probe d2common_306d0 drives this body.
#include <cstdint>
#define P(x) ((uint8_t*)(uintptr_t)(x))
void D2Common_306D0(uint32_t pu) {
    const uint8_t* u = P(pu);
    if (!u || *(const uint32_t*)u != 0) return;
    uint8_t* pd = P(*(const uint32_t*)(u + 0x14));
    const uint8_t* rec = P(*(const uint32_t*)(P(*(const uint32_t*)(u + 0xA8)) + 0xC));
    *(int32_t*)(pd + 0x70) = rec ? (int32_t)*(const int16_t*)P(*(const uint32_t*)rec) : 0;
    if (rec) *(uint32_t*)(pd + 0x78) = *(const uint32_t*)(rec + 0x34);
}
