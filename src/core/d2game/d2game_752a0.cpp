// 1.13c D2Game+0x752A0: the monster mode dispatcher. ECX = pGame, EDX = pUnit, two unread
// stack words (ret 8). The mode-table lookup 0x749A0 (EAX = mode, ECX = pUnit) is retail's own code and is inlined here;
// the table words are the slot-[2] handlers of D2Game's 16-entry table 0xFA498 and its class overrides (read from the DLL).
#include <cstdint>
struct G752A0Env {
    uint32_t (*handler)(uint32_t rva, uint32_t game, uint32_t unit);      // slot [2] of the entry (ECX = game, EDX = unit)
    uint32_t (*fallback)(uint32_t game, uint32_t unit, uint32_t mode);   // 0x750F0 (stack = game; ESI = unit, EDI = mode)
};
static uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
// 1.13c D2Game+0x749A0: the entry's slot-[2] handler rva (0 = none; a null entry also 0)
static uint32_t g_749a0_slot2(int32_t mode, uint32_t unit, uint32_t dt)
{
    static const uint32_t kPlain[16] = { 0x76040, 0, 0, 0x76BF0, 0x76BF0, 0x76BF0, 0x76BF0, 0x76BF0, 0x76BF0, 0x76BF0,
                                         0x74890, 0, 0, 0x765E0, 0x76BF0, 0 };
    if (unit == 0 || rd32(unit) != 1) return 0;
    const int32_t cls = (int32_t)rd32(unit + 4);
    if (cls < 0 || cls >= (int32_t)rd32(dt + 0xA80)) return 0;
    const uint32_t rec = rd32(dt + 0xA78) + (uint32_t)cls * 0x1A8;
    if (rec == 0 || mode < 0 || mode > 15) return 0;
    if (*(const uint8_t*)(uintptr_t)(rec + 0x1A5) != 0) {
        switch (cls) {
        case 0x21F: case 0x220: case 0x23A: case 0x2C5: if (mode == 10) return 0x76BF0; break;   // 0xFA508
        case 0xF3: case 0x14D: case 0x2C1: if (mode == 11 || mode == 10) return 0x76BF0; break;  // 0xFA598 / 0xFA5A8
        case 0x193: if (mode == 4 || mode == 5) return 0x76B20; if (mode == 8 || mode == 9) return 0x76A50; break;
        case 0x1A1: case 0x1A2: if (mode == 11) return 0x76BF0; break;                            // 0xFA5D8
        }
    }
    return kPlain[mode];
}
// 1.13c D2Game+0x752A0
uint32_t g_752a0(uint32_t game, uint32_t unit, uint32_t dt, const G752A0Env& env)
{
    const uint32_t mode = unit ? rd32(unit + 0x10) : 0;
    const uint32_t fn = g_749a0_slot2((int32_t)mode, unit, dt);
    if (fn) return env.handler(fn, game, unit);
    return env.fallback(game, unit, mode);
}
