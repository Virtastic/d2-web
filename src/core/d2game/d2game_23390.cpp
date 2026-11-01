// 1.13c D2Game+0x23390 -- unit mode-change broadcast (ECX = game, EDX = unit, two ignored stack words, ret 8).
// Unit mode 3 clears the unit's pending action (#11103 = 0xA496(unit, 0)), sends a 0x20-byte state block via
// 0x75E10(game, block, 1) (block: dword +4 = unit, byte +0x15 = 0x64, rest zero) and then 0x8EA80. A null unit and
// modes 0 and 0xC go straight to 0x8EA80(ECX = game, EDX = unit, 7, game[0xA8] + 4, 0, 0, 0); other modes do nothing.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game23390Env {
    void (*ordinalA496)(uint32_t unit, uint32_t zero);
    void (*sub75E10)(uint32_t game, const uint32_t* block, uint32_t one);
    void (*sub8EA80)(uint32_t game, uint32_t unit, uint32_t seven, uint32_t value, uint32_t z0, uint32_t z1, uint32_t z2);
};
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_23390(uint32_t game, uint32_t unit, const D2Game23390Env* e)
{
    uint32_t a8; std::memcpy(&a8, (const uint8_t*)(uintptr_t)game + 0xA8, 4);
    if (unit != 0) {
        uint32_t mode; std::memcpy(&mode, (const uint8_t*)(uintptr_t)unit + 0x10, 4);
        if (mode == 3) {
            e->ordinalA496(unit, 0);
            uint32_t blk[8] = {}; blk[1] = unit; ((uint8_t*)blk)[0x15] = 0x64;
            e->sub75E10(game, blk, 1);
        } else if (mode != 0 && mode != 0xC) {
            return;
        }
    }
    e->sub8EA80(game, unit, 7, a8 + 4, 0, 0, 0);
}
