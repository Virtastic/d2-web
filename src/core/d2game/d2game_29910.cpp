// 1.13c D2Game+0x29910: fastcall ECX = dst, EDX = character class. Writes the class name (terminator included) to dst: 0 Amazon, 1 Sorceress, 2 Necromancer, 3 Paladin, 4 Barbarian, 5 Druid, 6 Assassin, anything above 6 (unsigned) "Invalid Class". Exactly the string bytes and the NUL are stored.
#include "d2game_seam4e.h"
extern "C" void d2g_29910(uint32_t dst, uint32_t cls, const D2Seam*)
{
    static const char* const names[8] = { "Amazon", "Sorceress", "Necromancer", "Paladin", "Barbarian", "Druid", "Assassin", "Invalid Class" };
    const char* s = names[cls > 6 ? 7 : cls];
    uint32_t i = 0;
    for (;;) { d2wr8(dst + i, (uint8_t)s[i]); if (!s[i]) break; ++i; }
}
