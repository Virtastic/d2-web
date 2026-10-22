// 1.13c Fog+0x1E240 (ordinal 10094): async-request result getter. Stateful (calls the 0x1E170 blocking
// wait, process exit on corruption): NAMED, no FN vector.
#include <cstdint>

struct FogAsyncResult {
    uint32_t flags;                 // +0x48: magic | done bit
    uint32_t result;                // +0x3c
    void (*wait)(FogAsyncResult*);  // sub_1E170
};

// 1.13c Fog+0x1E240
extern "C"
uint32_t fog_sub_1e240(FogAsyncResult* r, void (*fatal)(int line, uint32_t flags)) {
    if (!r) { fatal(0x264, 0); return 0; }
    if ((r->flags & 0xfffffff0u) != 0x41737960u) { fatal(0x265, r->flags); return 0; }
    if (!(r->flags & 1)) r->wait(r);
    if (!(r->flags & 1)) { fatal(0x26b, r->flags); return 0; }
    return r->result;
}
