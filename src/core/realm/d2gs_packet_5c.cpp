#include <cstdint>
#include <emscripten.h>

// 0x5c PlayerLeft, 5 bytes: [0x5c][unit id u32].  1.13c D2Game+0x8B360 sends the opcode and the id
// the caller loaded (the server's player-left notice, composed once per remaining client).
extern "C" EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_player_left(uint32_t id, uint8_t* out) {
    out[0] = 0x5C;
    for (int i = 0; i < 4; i++) out[1 + i] = (uint8_t)(id >> (8 * i));
    return 5;
}
