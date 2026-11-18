// 1.13c BNClient+0xEC00 (no arguments): the logon answer for the logon scheme in use.
// When the connection has dropped, the pending error becomes 0x12 unless one is already set (under the module lock) and
// 0 is returned. Otherwise scheme 0 returns the answer slot of message 0x3A, scheme 1 that of message 0x54, any other 0.
#include <cstdint>

struct BnEc00Globals { const uint32_t* g3f644; uint32_t* g3f1f4; const uint32_t* g3f878; const uint32_t* g3eea8; const uint32_t* g3ee40; };

// 1.13c BNClient+0xEC00
extern "C" uint32_t bnclient_ec00(const BnEc00Globals* g) {
    if (*g->g3f644 != 0) {
        if (*g->g3f1f4 == 0) *g->g3f1f4 = 0x12;
        return 0;
    }
    if (*g->g3f878 == 0) return *g->g3ee40;
    if (*g->g3f878 == 1) return *g->g3eea8;
    return 0;
}
