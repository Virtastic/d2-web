// 1.13c D2Game+0x6FEC0 -- MISSILES_RemoveAll: EDI = game. Walks the 128 buckets of unit row 4 (game+0x1920), the chain link at unit+0xE4 read
// before each 0xE20A0 SUNIT_RemoveUnit(game, unit) call. The bucket head is not rewritten; the null-unit assert of the decompile is unreachable.
#include <cstring>
#include "d2game_6fec0.h"
// 1.13c D2Game+0x6FEC0
void d2game_port_6fec0(uint32_t game, const D2Game6FEC0Env* env)
{
    auto rd = [](uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; };
    for (uint32_t i = 0; i < 0x80; i++)
        for (uint32_t u = rd(game + 0x1920 + 4 * i); u;) {
            const uint32_t next = rd(u + 0xe4);
            env->removeUnit(game, u);
            u = next;
        }
}
