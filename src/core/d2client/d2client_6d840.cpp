// 1.13c D2Client+0x6D840: EAX = key, EDI = out (6 bytes), stack 0 = skip. Key 0: nothing. Else s = 0xC262(key, 3);
// skip == 0 and 0xC724(s) != 0 gives rec = 0xC1BA(s ? s[1] : -1), out := rec[0x116..0x11B] (rec null: out untouched); every other case out := 0.
// Returns the EAX retail leaves: rec on the copy and null-rec paths, 0 on the others (key 0, or the zeroing xor).
#include <cstring>
#include "cl_env.h"
extern "C" uint32_t d2c_6d840(uint32_t key, uint32_t out, uint32_t skip, const cl::Env& e) {
    if (!key) return 0;
    uint32_t s = e.H(0xc262, key, 3);
    uint8_t* o = (uint8_t*)(uintptr_t)out;
    if (skip == 0 && e.H(0xc724, s)) {
        uint32_t rec = e.H(0xc1ba, s ? e.R(s + 4) : 0xffffffffu);
        if (rec) memcpy(o, (const void*)(uintptr_t)(rec + 0x116), 6);
        return rec;
    }
    memset(o, 0, 6);
    return 0;
}
