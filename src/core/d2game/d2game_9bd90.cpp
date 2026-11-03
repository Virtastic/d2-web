// Party control teardown on retail's 32-bit layout (the vendored typed form is PARTY_FreePartyControl in
// d2moo/src/D2Game/UNIT/Party.cpp). EDI = game: every member node of every party, then the party, then the control
// block go back to the game pool (+0x1C) through Fog's free (Party.cpp lines 0x3F, 0x43, 0x47), and +0x1D2C is cleared.
// A null game is fatal (Fog #10265 / #10024, then exit), as in retail.
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G9BD90Env {
    void (*free)(uint32_t pool, uint32_t p, int line);   // Fog #10046 (thunk 0xA2EC), file "..\Source\D2Game\UNIT\Party.cpp"
    void (*fatal)();                                     // the null-game assert
};

void PARTY_FreePartyControl32(uint32_t game, const D2G9BD90Env& env)
{
    if (!game) { env.fatal(); return; }
    const uint32_t pc = rd32(game + 0x1D2C);
    if (!pc) return;
    for (uint32_t party = rd32(pc + 4); party;) {
        const uint32_t next = rd32(party + 8);
        for (uint32_t m = rd32(party + 4); m;) {
            const uint32_t n = rd32(m + 4);
            env.free(rd32(game + 0x1C), m, 0x3F);
            m = n;
        }
        env.free(rd32(game + 0x1C), party, 0x43);
        party = next;
    }
    env.free(rd32(game + 0x1C), pc, 0x47);
    wr32(game + 0x1D2C, 0);
}
