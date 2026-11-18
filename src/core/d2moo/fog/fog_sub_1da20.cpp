// 1.13c Fog+0x1DA20 (ordinal 10095): async-request handle getter, returns field +0x40 after the
// magic check. Process exit on bad handle: NAMED, no FN vector.
#include <cstdint>

struct FogAsyncReq {
    uint8_t  pad[0x40];
    uint32_t value;  // +0x40
    uint32_t pad2;
    uint32_t flags;  // +0x48: magic 0x41737960 | low nibble
};

// 1.13c Fog+0x1DA20
extern "C"
uint32_t fog_sub_1da20(FogAsyncReq* r, void (*fatal)(int line, uint32_t flags)) {
    if (!r) { fatal(0x276, 0); return 0; }
    if ((r->flags & 0xfffffff0u) != 0x41737960u) { fatal(0x277, r->flags); return 0; }
    return r->value;
}
