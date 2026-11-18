// D2Client 0x45FF0: the skill-id resolver (BX = wanted id; caller 0x4B8C8).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
// 1.13c D2Client+0x45FF0 ; tbl = the 8-byte entries at 0xF42F8 ({id u16, value u16, ..}), n = [0xF4304], h = [0x113B10],
// skip = the u16 list at 0x119792, nskip = the u16 [0x1197A6]. Ordinal_10778 (thunk 0xCE08, stack h, i) goes through host; its AX is read.
extern "C" EMSCRIPTEN_KEEPALIVE uint16_t d2_client_45ff0(uint16_t bx, const uint8_t* tbl, int32_t n, uint32_t h,
                                                        const uint16_t* skip, uint16_t nskip, D2cHost host) {
    for (int32_t i = 0; i < n; ++i) {
        uint16_t id; std::memcpy(&id, tbl + i * 8, 2);
        if (id == bx) { uint16_t v; std::memcpy(&v, tbl + i * 8 + 2, 2); return v; }
    }
    for (uint32_t i = 0;; ++i) {
        D2cCall c = {}; c.rva = 0xCE08; c.s[0] = h; c.s[1] = i;
        const uint16_t s = (uint16_t)host(&c);
        if (s == 0xFFFF) return 0xFFFF;
        if (s == bx) continue;
        uint32_t k = 0;
        while (k < nskip && skip[k] != s) ++k;
        if (k == nskip) return s;
    }
}
