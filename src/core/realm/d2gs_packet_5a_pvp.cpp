#include <cstdint>
#include <emscripten.h>

// 0x5a game event, 40 bytes, as the party screen sends it (1.13c D2Game+0x8B400 copies the
// 0x28-byte record the party code filled).  Type 7 is the party / hostility / corpse-loot notice:
// [1]=7 [2]=text colour [3..6]=the other player's unit id [7]=notice kind, the rest zero.  The
// colour follows the kind: loot allowed dark gold, hostile orange, no longer hostile white, invite /
// joined / allied light green, cancelled / left / loot withdrawn yellow.  Type 16 is the "wait to
// declare hostility again" refusal and carries nothing else.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_pvp_status(uint32_t id, uint8_t notice, uint8_t* out) {
    for (int i = 0; i < 40; i++) out[i] = 0;
    out[0] = 0x5A;
    out[1] = 7;
    uint8_t colour = 3;   // blue, the default
    switch (notice) {
    case 1: case 2: colour = 4; break;
    case 3: colour = 8; break;
    case 4: colour = 0; break;
    case 5: case 7: case 8: colour = 2; break;
    case 6: case 9: case 10: case 11: colour = 9; break;
    default: break;
    }
    out[2] = colour;
    for (int i = 0; i < 4; i++) out[3 + i] = (uint8_t)(id >> (8 * i));
    out[7] = notice;
    return 40;
}

extern "C" EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_event_wait_hostile(uint8_t* out) {
    for (int i = 0; i < 40; i++) out[i] = 0;
    out[0] = 0x5A;
    out[1] = 16;
    return 40;
}
