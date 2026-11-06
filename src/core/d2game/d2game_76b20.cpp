// 1.13c D2Game+0x76B20, injectable callees. Fastcall: ECX = game, EDX = unit, plain ret. Builds the 8-dword request block
// {8, unit, owner-or-0, 0, 0, mode, 0, 0} (mode byte at +0x15: 0x65 when the unit's monstats2 row has a flag in the 0xF8BE0 mask, else 0x64)
// for 0x75E10, then runs the 0xFBC1C hook when the unit is not left in place.
#include <cstdint>
#include <cstring>
struct G76B20Env {
    void     (*ord11103)(uint32_t unit, uint32_t zero);            // 0xA496
    uint32_t (*monstats2)(uint32_t classId);                       // 0x1320 (EAX = class id)
    void     (*f7a0)(uint32_t game, uint32_t unit);                // 0xE07A0 (EAX = unit, ECX = game)
    uint32_t (*owner)(uint32_t v);                                 // 0xA598
    void     (*f75e10)(uint32_t game, uint32_t* blk, uint32_t one);// 0x75E10 (ret 0xC)
    int32_t  (*ord10494)(uint32_t unit, uint32_t one);             // 0xA68E
    int32_t  (*ffa90)(uint32_t unit);                              // 0xFFA90 (EAX = unit)
    const uint32_t* maskp;                                         // **(0xF8BE0)
    void     (*cb)(uint32_t game, uint32_t unit, uint32_t a, uint32_t b); // PTR 0xFBC1C, null = none
};
// 1.13c D2Game+0x76B20
void g_76b20(uint32_t game, uint32_t unit, const G76B20Env& e)
{
    e.ord11103(unit, 0);
    uint32_t blk[8] = {};
    blk[0] = 8; blk[1] = unit;
    uint32_t mode = 0x64;
    uint32_t classId; memcpy(&classId, (const void*)(uintptr_t)(unit + 4), 4);
    uint32_t rec = e.monstats2(classId);
    if (rec && (*(const uint8_t*)(uintptr_t)(rec + 0x105) & *e.maskp)) mode = 0x65;
    ((uint8_t*)blk)[0x15] = (uint8_t)mode;
    e.f7a0(game, unit);
    uint32_t u2c; memcpy(&u2c, (const void*)(uintptr_t)(unit + 0x2c), 4);
    uint32_t o = e.owner(u2c);
    blk[2] = o == unit ? 0 : o;
    e.f75e10(game, blk, 1);
    if ((e.ord10494(unit, 1) == 0 || e.ffa90(unit) != 0) && e.cb) e.cb(game, unit, 0, 0);
}
