// 1.13c D2Client+0xA8030: walk 128 unit-list buckets (param_1[0..0x7f]); per unit call 0x2B1E0, then find the same-id unit in the
// global hash (D2Client+0x109A08, [type*0x80 + (id&0x7f)], chain next = word 0x39) and dispatch by type.
#include "client_host.h"
#include <emscripten.h>
static uint32_t cl(D2cHost h, D2cCall c) { return h(&c); }
// rd = read u32 at guest address; hash = the 0x80*N-entry head table; tbl = byte->handler RVA table (D2Client+0xF54E8, 19 entries).
// Returns 0 normally, 1 for the fatal _exit(-1) paths (msg 0x5A type mismatch / 0x546 bad item state; Ordinal_10265/10024 text unverified).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a8030(D2cHost h, uint32_t (*rd)(uint32_t), uint32_t buckets, const uint32_t* hash, const uint32_t* tbl) {
    for (uint32_t i = 0; i < 0x80; i++) {
        uint32_t u = rd(buckets + i * 4);
        while (u) {
            uint32_t next = rd(u + 0xE4), id = rd(u + 12), type = rd(u);
            cl(h, { 0x2B1E0, u, 0, 0, 0, 0, { 0, 0, 0, 0 } });
            for (uint32_t p = hash[type * 0x80 + (id & 0x7f)]; p; p = rd(p + 0xE4)) {
                if (rd(p + 12) != id) continue;
                if (rd(p) != type) return 1;
                if (type == 1) { cl(h, { 0x17F70, 0, 0, 0, 0, 0, { 0, 0, 0, 0 } }); break; }
                if (type != 2) break;
                uint32_t d = cl(h, { 0x10688, rd(p + 4), 0, 0, 0, 0, { 0, 0, 0, 0 } }); // Ordinal_10688 (unverified id)
                uint32_t s = rd(d + 0x1B4) & 0xFF;
                if (s >= 0x13) return 1;
                if (s) cl(h, { tbl[s], 0, 0, 0, 0, 0, { 0, 0, 0, 0 } });
                break;
            }
            u = next;
        }
    }
    return 0;
}
