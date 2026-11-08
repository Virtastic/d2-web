#include <cstdint>
#include <emscripten.h>

// 0x8d PartyId, 7 bytes: [0x8d][unit id u32][party id u16], 0xffff for a player in no party.
// 1.13c D2Game+0x8CFB0 (broadcast) and +0x8C570 (per client) take the party id from the party
// control and write 0xffff for a null unit's id as well.
extern "C" EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_party_id(uint32_t id, uint16_t party, uint8_t* out) {
    out[0] = 0x8D;
    for (int i = 0; i < 4; i++) out[1 + i] = (uint8_t)(id >> (8 * i));
    out[5] = (uint8_t)party;
    out[6] = (uint8_t)(party >> 8);
    return 7;
}
