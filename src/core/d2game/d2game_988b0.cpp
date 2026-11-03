// 1.13c D2Game+0x988B0: EDI = skill id, EBX = unit (a type-1 unit), stack (ret 0xC) = A1 (unit with the
// seed at +0x20), A2 (skill level, the fourth word of #10786), A3 (when nonzero the seed step must stay below the formula value).
// The Skills row (stride 0x23C) and its count come from sgptDataTables (rows at +0xB98, count at +0xBA0); the callees are seams.
#include <cstdint>
#include <cstring>
struct D2G988B0Seams {
    uint32_t (*o11104)(uint32_t unit);                                   // thunk 0xA7F6
    uint32_t (*o10830)(uint32_t unit);                                   // thunk 0xA514
    uint32_t (*f9F120)(uint32_t unit);                                   // 0xBF120: EAX = unit, stack 0xB
    int32_t  (*o10786)(uint32_t a, uint32_t formula, uint32_t skill, uint32_t c);   // thunk 0xAC9A
    int32_t  (*f1180)(uint32_t seed);                                    // 0x1180: EAX = 100, ECX = A1 + 0x20
};
static uint32_t rd32(const void* p, uint32_t o) { uint32_t v; memcpy(&v, (const uint8_t*)p + o, 4); return v; }
extern "C" uint32_t d2game_988b0(int32_t skill, uint32_t unit, uint32_t a1, uint32_t a2, uint32_t a3, const uint8_t* tables, const D2G988B0Seams* s)
{
    if (skill < 0 || skill >= (int32_t)rd32(tables, 0xba0)) return 0;
    const uint32_t rows = rd32(tables, 0xb98);
    const uint32_t row = (uint32_t)skill * 0x23c + rows;
    if (row == 0) return 0;
    if (s->o11104(unit) != 0) return 0;
    if (unit == 0) return 0;
    if (rd32((const void*)(uintptr_t)unit, 0) != 1) return 0;
    if (s->o10830(unit) != 0) return 0;
    if (s->f9F120(unit) == 0) return 0;
    const int32_t formula = s->o10786(a1, rd32((const void*)(uintptr_t)row, 0x138), (uint32_t)skill, a2);
    if (a3 != 0 && s->f1180(a1 + 0x20) >= formula) return 0;
    return 1;
}
