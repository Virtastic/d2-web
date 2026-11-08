#include <cstdint>
#include <emscripten.h>

// 0x8c Relation, 11 bytes: [0x8c][from u32][to u32][flags u16].  1.13c D2Game+0x8C630 (the
// per-client form) and +0x8D010 (the broadcast form) lay out the same bytes: the flags word is what
// player `from` holds toward player `to` (1 loot, 2 not heard, 4 squelched, 8 hostile).
extern "C" EMSCRIPTEN_KEEPALIVE int d2_realm_d2gs_relation(uint32_t from, uint32_t to, uint16_t flags, uint8_t* out) {
    out[0] = 0x8C;
    for (int i = 0; i < 4; i++) out[1 + i] = (uint8_t)(from >> (8 * i));
    for (int i = 0; i < 4; i++) out[5 + i] = (uint8_t)(to >> (8 * i));
    out[9] = (uint8_t)flags;
    out[10] = (uint8_t)(flags >> 8);
    return 11;
}
