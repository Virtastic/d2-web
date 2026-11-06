// 1.13c D2Game+0xBB4F0 -- EAX = pGame (second word of the 0x9E170 call), ESI = pUnit. Calls 0x9E170 (EAX = unit, EDI = 0x67, stack game, 0), reads the
// player-data word +0x9C (0 when the unit is null or not type 0), then sends the 6-byte packet 5D 17 01 00 00 00 through 0x8A3E0 (EAX = that word).
#include <cstdint>
#include <cstring>
struct GBB4F0Env {
    void (*sub9E170)(uint32_t eax, uint32_t edi, uint32_t game, uint32_t zero);   // EAX = unit, EDI = 0x67, stack words (game, 0), ret 8
    uint32_t (*pdata)(uint32_t unit);                                              // 0xA4C0 (stdcall 1)
    void (*send)(uint32_t eax, const uint8_t* bytes, uint32_t len);                // 0x8A3E0 (EAX, stack bytes + length, ret 8)
};
// 1.13c D2Game+0xBB4F0
void g_bb4f0(uint32_t game, uint32_t unit, const GBB4F0Env& env)
{
    env.sub9E170(unit, 0x67, game, 0);
    uint32_t w = 0;
    if (unit != 0 && *(const uint32_t*)(uintptr_t)unit == 0) { uint32_t pd = env.pdata(unit); memcpy(&w, (const void*)(uintptr_t)(pd + 0x9C), 4); }
    const uint8_t msg[6] = { 0x5D, 0x17, 0x01, 0x00, 0x00, 0x00 };
    env.send(w, msg, 6);
}
