// 1.13c D2Game+0xB08A0: a unit's alignment byte (caller 0xB1660). ECX = pUnit, plain ret;
// 0xE for a player, MonStats byte +0x4E for a monster with a valid class, 0 otherwise.
#include <cstdint>
static uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
// 1.13c D2Game+0xB08A0
uint32_t g_b08a0(uint32_t unit, uint32_t dt)
{
    if (unit == 0) return 0;
    if (rd32(unit) == 0) return 0xE;
    if (rd32(unit) != 1) return 0;
    const int32_t cls = (int32_t)rd32(unit + 4);
    if (cls < 0 || cls >= (int32_t)rd32(dt + 0xA80)) return 0;
    const uint32_t rec = (uint32_t)cls * 0x1A8 + rd32(dt + 0xA78);
    if (rec == 0) return 0;
    return *(const uint8_t*)(uintptr_t)(rec + 0x4E);
}
