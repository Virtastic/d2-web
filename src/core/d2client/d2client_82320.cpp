// 1.13c D2Client+0x82320 (226 bytes): the item-update packet handler. EAX = packet
// (+2 byte length, +4 item id, +8 the bit stream); plain `ret`. Retail 32-bit layout; callees go through the D2cHost seam
// in retail's call order with retail's registers and stack words.
//   The item is looked up in the client's item hash 0x10AE08 (128 buckets by id & 0x7F, chained at +0xE4, id at +0xC); a
//   match that is not an item (type != 4) is fatal 0x5A. #11145 (0xCA8A) decodes the stream into it (or a new unit when
//   none matched): (item, pkt+8, len-8, 0, 0, 0x60, &failed). On failure 0xA6EF0 drops the hash entry (ECX = the table,
//   EDX = bucket, stack id, 4). Otherwise the player (0x11BBFC; fatal 0x7B3 when null) inventory (+0x60; fatal 0x7B6) is
//   refreshed: #10838 (0xCA00)(item, 8) into #10197 (0xCA3C); when that is 0, #10466 (0xC7D8)(inv, 0), #11017 (0xC202)(inv)
//   and 0x162D0; then #11017 (0xC202)(inv), and when it returns a unit 0x81AA0 with ESI = that unit.
// Fatal: 0xBF92 (#10265, the message) and 0xBF80 (#10024) then 0x3664 (exit); the port stops there.
#include <cstdint>
#include <cstring>
#include "client_host.h"

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint32_t call(D2cHost h, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t esi,
              uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0) {
    D2cCall c{ rva, eax, ecx, edx, 0, esi, { s0, s1, s2, s3 }, { 0, 0, 0, 0 }, s4, 0 };
    return h(&c);
}
void fatal(D2cHost h, uint32_t code) {
    const uint32_t m = call(h, 0xBF92, 0, 0, 0, 0, code);
    call(h, 0xBF80, 0, 0, 0, 0, 0x6FB8359Fu, m);
    call(h, 0x3664, 0, 0, 0, 0, 0xFFFFFFFFu);
}
}  // namespace

struct D2c82320Globals {
    uint32_t hashTab;   // the bytes of DAT 0x10AE08 (128 bucket heads)
    uint32_t hashVa;    // its retail address (0x6FBBAE08), handed to 0xA6EF0 in ECX
    uint32_t player;    // DAT 0x11BBFC
    uint32_t okSlot;    // the caller-stack dword #11145 writes its failure flag through (retail's [esp+8])
};

// 1.13c D2Client+0x82320
extern "C" void d2_client_82320(uint32_t pkt, const D2c82320Globals* g, D2cHost h) {
    const uint32_t id = rd32(pkt + 4);
    const uint32_t len = (uint32_t)*(const uint8_t*)(uintptr_t)(pkt + 2) - 8;
    uint32_t item = 0;
    for (uint32_t n = rd32(g->hashTab + (id & 0x7F) * 4); n; n = rd32(n + 0xE4)) {
        if (rd32(n + 0xC) == id) {
            if (rd32(n) != 4) { fatal(h, 0x5A); return; }
            item = n;
            break;
        }
    }
    call(h, 0xCA8A, pkt + 8, len, g->okSlot, pkt, item, pkt + 8, len, 0, 0);   // stack words 5, 6: 0x60, &failed
    if (rd32(g->okSlot)) {
        call(h, 0xA6EF0, 0, g->hashVa, id & 0x7F, pkt, id, 4);
        return;
    }
    if (!g->player) { fatal(h, 0x7B3); return; }
    const uint32_t inv = rd32(g->player + 0x60);
    if (!inv) { fatal(h, 0x7B6); return; }
    const uint32_t r = call(h, 0xCA00, 0, 0, 0, inv, item, 8);
    if (!call(h, 0xCA3C, r, 0, 0, inv, r)) {
        call(h, 0xC7D8, 0, 0, 0, inv, inv, 0);
        const uint32_t e = call(h, 0xC202, 0, 0, 0, inv, inv);
        call(h, 0x162D0, e, 0, 0, inv);
    }
    const uint32_t u = call(h, 0xC202, 0, 0, 0, inv, inv);
    if (u) call(h, 0x81AA0, u, 0, 0, u);
}
