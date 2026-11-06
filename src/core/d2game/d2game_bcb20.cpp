// 1.13c D2Game+0xBCB20 -- ECX = pGame, EAX = pUnit (may be null), EDI = value. With game byte +0x6D zero, looks up the unit's record (0x8F5C0: EAX = a stack
// out-word, ECX = game, ESI = unit id or -1) and caps the value at {0xC,0x14,0x1C,0x24,0x2D}[record byte +0x22] (signed, records with a byte >= 5 are not capped).
#include <cstdint>
#include <cstring>
struct GBCB20Env { uint32_t (*record)(uint32_t game, uint32_t unitId); };   // 0x8F5C0
// 1.13c D2Game+0xBCB20
uint32_t g_bcb20(uint32_t game, uint32_t unit, uint32_t value, const GBCB20Env& env)
{
    if (*(const uint8_t*)(uintptr_t)(game + 0x6D) != 0) return value;
    uint32_t id = 0xFFFFFFFFu;
    if (unit) memcpy(&id, (const void*)(uintptr_t)(unit + 4), 4);
    const uint32_t rec = env.record(game, id);
    if (!rec) return value;
    const uint32_t k = *(const uint8_t*)(uintptr_t)(rec + 0x22);
    if (k >= 5) return value;
    static const int32_t cap[5] = { 0xC, 0x14, 0x1C, 0x24, 0x2D };
    return cap[k] < (int32_t)value ? (uint32_t)cap[k] : value;
}
