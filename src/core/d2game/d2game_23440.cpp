// 1.13c D2Game+0x23440 -- unit death/idle notification (ECX = game, EDX = unit, two ignored stack words, ret 8).
// Skips a null unit, flag +0xC6 bit 0, a player (type 0) in mode 0 or 0x11, a monster (type 1) in mode 0 or 0xC and
// every other type. #10494 = 0xA68E(unit, 0x36) non-zero -> 0x8EA80(game, unit, 7, game[0xA8] + 3, 0, 0, 0). Else a
// type-1 unit whose monster data (+0x14 -> +0x28) holds a link (+0x28) that 0xE03A0(EAX = link[0x30], EDX = link[0x2C],
// ECX = link[0x28]) resolves to a type-0 unit gets 0x97B50(game, found, unit[0xC], 1). Otherwise the action is cleared
// (0xA496(unit, 0)) and the state block (dword +4 = unit, byte +0x15 = 0x64) goes out through 0x75E10(game, block, 1).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game23440Env {
    uint32_t (*ordinalA68E)(uint32_t unit, uint32_t what);
    void (*sub8EA80)(uint32_t game, uint32_t unit, uint32_t seven, uint32_t value, uint32_t z0, uint32_t z1, uint32_t z2);
    uint32_t (*subE03A0)(uint32_t a, uint32_t d, uint32_t c);
    void (*sub97B50)(uint32_t game, uint32_t found, uint32_t id, uint32_t one);
    void (*ordinalA496)(uint32_t unit, uint32_t zero);
    void (*sub75E10)(uint32_t game, const uint32_t* block, uint32_t one);
};
static inline uint32_t g23440_u32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_23440(uint32_t game, uint32_t unit, const D2Game23440Env* e)
{
    if (unit == 0 || (*((const uint8_t*)(uintptr_t)unit + 0xC6) & 1)) return;
    const uint32_t type = g23440_u32(unit, 0), mode = g23440_u32(unit, 0x10);
    if (type == 0) { if (mode == 0 || mode == 0x11) return; }
    else if (type == 1) { if (mode == 0 || mode == 0xC) return; }
    else return;
    if (e->ordinalA68E(unit, 0x36)) {
        e->sub8EA80(game, unit, 7, g23440_u32(game, 0xA8) + 3, 0, 0, 0);
        return;
    }
    if (type == 1) {
        const uint32_t pd = g23440_u32(unit, 0x14);
        const uint32_t rec = pd ? g23440_u32(pd, 0x28) : 0;
        const uint32_t link = rec ? g23440_u32(rec, 0x28) : 0;
        if (link) {
            const uint32_t found = e->subE03A0(g23440_u32(rec, 0x30), g23440_u32(rec, 0x2C), link);
            if (found && g23440_u32(found, 0) == 0) {
                e->sub97B50(game, found, g23440_u32(unit, 0xC), 1);
                return;
            }
        }
    }
    e->ordinalA496(unit, 0);
    uint32_t blk[8] = {}; blk[1] = unit; ((uint8_t*)blk)[0x15] = 0x64;
    e->sub75E10(game, blk, 1);
}
