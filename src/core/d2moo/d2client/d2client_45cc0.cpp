// D2Client 0x45CC0: the gated callback table walk (caller 0x4A4F0).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
// 1.13c D2Client+0x45CC0 ; EAX = n, ECX = tbl (0x14-byte entries {key, a, b, want, callback}), stack key, p (ret 8).
// For each entry with key: Ordinal_10174 (thunk 0xC58C, stack p, a, b) through host; the callback runs when its result is nonzero
// and want != 0, or zero and want == 0; it goes through host with rva = the entry's callback pointer. Returns the last callback's EAX, else 1.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_45cc0(int32_t n, const uint8_t* tbl, uint32_t key, uint32_t p, D2cHost host) {
    uint32_t r = 1;
    for (int32_t i = 0; i < n; ++i) {
        uint32_t e[5]; std::memcpy(e, tbl + i * 0x14, 0x14);
        if (e[0] != key) continue;
        D2cCall c = {}; c.rva = 0xC58C; c.s[0] = p; c.s[1] = e[1]; c.s[2] = e[2];
        const bool hit = host(&c) != 0;
        if (hit == (e[3] != 0)) { c = {}; c.rva = e[4]; r = host(&c); }
    }
    return r;
}
