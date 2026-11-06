// D2Game+0x33290 -- remove every unit of one 128-bucket list of a game (the list at game+0x1520), 1.13c.
// edi = game, no stack args. Per bucket head, walk the +0xE4 next link: next is read BEFORE 0xE20A0 (ecx = game, edx = unit) removes the unit.
// A null unit in the inner loop is dead code (the assert/_exit branch); retail tests the head twice. Seam port for the FN probe: the vendored
// OBJECTS_RemoveAll (Objects.cpp, same marker) reaches the same walk through the engine's unit list.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2Game+0x33290
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_33290_probe(uint32_t game, void (*removeUnit)(uint32_t game, uint32_t unit))
{
    for (uint32_t i = 0; i < 0x80; ++i) {
        uint32_t u; std::memcpy(&u, (const void*)(uintptr_t)(game + 0x1520 + 4 * i), 4);
        while (u != 0) {
            uint32_t next; std::memcpy(&next, (const void*)(uintptr_t)(u + 0xe4), 4);
            removeUnit(game, u);
            u = next;
        }
    }
}
