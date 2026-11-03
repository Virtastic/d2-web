// 1.13c D2Game+0x79730: finish a unit's (player's) arrival in a game. Registers: ECX = game, EDX = unit; the function pops 0xC (ret 0xC) but reads no stack word.
// Order: the unit's two descriptor words (types 2, 4 and 5 read them from [[unit+0x2c]+0x10 / +0xC]; any other type asks 0xA3B8 / 0xA454 for them when +0x2c is set),
// 0x79210 (game, unit, second, first, room), 0x97730 (EBX = unit; game), the player data's flag byte +0xA |= 8, then either 0xDA750 / 0xA4A2(0x11) / 0xDFF10 / 0x78230 / 0xE0750
// straight away (game flag [0x111C3C] set and flag bit 2 clear) or, first, 0x54320 on player data +0xD. Anything but a player with player data is fatal (line 0x9D9).
// Addresses are 32-bit words as in the FN probes.
#include <cstdint>
#include <cstring>
struct D2G79730Seams {
    uint32_t (*descA)(uint32_t p);                                                    // 0xA3B8
    uint32_t (*descB)(uint32_t p);                                                    // 0xA454
    uint32_t (*getRoom)(uint32_t unit);                                               // 0xA478
    void     (*arrive)(uint32_t game, uint32_t unit, uint32_t b, uint32_t a, uint32_t room); // 0x79210
    void     (*s97730)(uint32_t unit, uint32_t game);                                 // 0x97730 (EBX = unit; stack game)
    uint32_t (*playerData)(uint32_t unit);                                            // 0xA4C0 (a unit-data record; +0x9C is the player data)
    uint32_t (*gameFlag)();                                                           // the module word at 0x111C3C
    void     (*s54320)(uint32_t unit, uint32_t game, uint32_t p, uint32_t zero);      // 0x54320 (ESI = unit, EBX = game; stack p, 0)
    void     (*sDA750)(uint32_t unit, uint32_t game);                                 // 0xDA750 (EDI = unit; stack game)
    void     (*setState)(uint32_t unit, uint32_t v);                                  // 0xA4A2
    void     (*sDFF10)(uint32_t unit);                                                // 0xDFF10 (ESI = unit)
    void     (*s78230)(uint32_t unit, uint32_t game);                                 // 0x78230 (EAX = unit, ECX = game, ESI = unit)
    void     (*sE0750)(uint32_t unit, uint32_t game);                                 // 0xE0750 (EBX = unit, EAX = game)
    void     (*fatal)(uint32_t line);                                                 // 0xA2DA, 0xA2C2, 0x22A3C(-1): never returns in retail
};
static inline uint32_t r32_79730(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
// 1.13c D2Game+0x79730
extern "C" void d2game_79730(uint32_t game, uint32_t unit, const D2G79730Seams* s) {
    const uint32_t type = r32_79730(unit, 0), ext = r32_79730(unit, 0x2c);
    const bool direct = type == 2 || (type > 3 && type < 6);
    const uint32_t a = direct ? r32_79730(ext, 0x10) : ext ? s->descA(ext) : 0;
    const uint32_t b = direct ? r32_79730(ext, 0xc) : ext ? s->descB(ext) : 0;
    s->arrive(game, unit, b, a, s->getRoom(unit));
    s->s97730(unit, game);
    uint32_t pd = type == 0 ? r32_79730(s->playerData(unit), 0x9c) : 0;
    uint8_t* fl = (uint8_t*)(uintptr_t)(pd + 10);
    *fl |= 8;
    uint16_t w; std::memcpy(&w, fl, 2);
    if (!(s->gameFlag() && !(w & 4))) {
        pd = type == 0 ? r32_79730(s->playerData(unit), 0x9c) : 0;
        if (!pd) { s->fatal(0x9d9); return; }
        s->s54320(unit, game, pd + 0xd, 0);
    }
    s->sDA750(unit, game);
    s->setState(unit, 0x11);
    s->sDFF10(unit);
    s->s78230(unit, game);
    s->sE0750(unit, game);
}
