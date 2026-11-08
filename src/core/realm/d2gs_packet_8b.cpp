#include <cstdint>
#include <emscripten.h>

// 0x8b PartyState, 6 bytes: [0x8b][other player's unit id u32][state u8].  1.13c D2Game+0x8A7E0.
// The state is the recipient's own record toward that player: 0 none, 1 in a party together,
// 2 invited by them, 4 has invited them.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_party_state(uint32_t id, uint8_t state, uint8_t* out) {
    out[0] = 0x8B;
    for (int i = 0; i < 4; i++) out[1 + i] = (uint8_t)(id >> (8 * i));
    out[5] = state;
    return 6;
}
