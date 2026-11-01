// Picks a shrine id of a shrine class from the game's object control (game+0x10F0): the class (0 or > 4 -> 2) indexes
// a count at +8 and an id list at +0x28; up to 8 seeded picks until the shrine record's minimum level (+0xB4) is met.
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G2ED70Env {
    void (*fatal)(uint32_t line);            // D2Common #10265 + Fog #10024 + exit
    uint32_t (*shrineRecord)(uint32_t id);   // D2Common #10403 (thunk 0xB2CA)
};

// 1.13c D2Game+0x2ED70
int32_t OBJECTS_PickShrineId(uint32_t game, uint8_t shrineClass, uint32_t level, const D2G2ED70Env& env)
{
    if (shrineClass == 0 || shrineClass > 4) shrineClass = 2;
    const uint32_t control = rd32(game + 0x10F0);
    const uint32_t cls = shrineClass;
    if (rd32(control + 8 + cls * 4) == 0) { env.fatal(0x2CC); return 0; }
    int32_t tries = 8;
    int32_t id;
    do {
        uint32_t n = rd32(control + 8 + cls * 4);
        const uint32_t seed = rd32(game + 0x10F0);
        if ((int32_t)n < 1) n = 0;
        else {
            const uint64_t s = (uint64_t)rd32(seed) * 0x6AC690C5u + rd32(seed + 4);
            wr32(seed, (uint32_t)s);
            wr32(seed + 4, (uint32_t)(s >> 32));
            n = (n & (n - 1)) == 0 ? (n - 1) & (uint32_t)s : (uint32_t)s % n;
        }
        id = (int32_t)rd32(rd32(control + 0x28 + cls * 4) + n * 4);
        if (id == 0) id = 1;
    } while (level < rd32(env.shrineRecord((uint32_t)id) + 0xB4) && --tries > 0);
    return id;
}
