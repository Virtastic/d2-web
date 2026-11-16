// 1.13c D2Client+0x81FF0 (387 bytes): an item's owner-side refresh after an item packet.
// EDI = item, ESI = unit, stack flag; `ret 4`. Retail 32-bit layout; the D2Common imports go through the D2cHost seam in
// retail's call order with retail's stack words:
//   unit in state 0x14 (#10744 0xC1CC): item must be type 4 (fatal 0x30); #10695 (0xC1BA)(unit ? unit+4 : -1) (fatal 0x32),
//     #10806 (0xC80E)(that +0xF0) (fatal 0x34), #10519 (0xCA7E)(item, 0), #10996 (0xCA72)(2, 0, unit, b, a);
//   else in state 0x4A: the same with codes 0x3A/0x3C/0x3E and #10996(5, item, unit, b, a); else #10600 (0xCA4E)(item, unit, 0, 0).
//   Then #10202 (0xC1A8)(unit, 0x4000, 0x48, file) set: done. An item of type 0 whose #10747 (0xC6E8) holds: owner =
//   #10061 (0xCA0C)(item+0x60), n = 0 when owner is another unit in state 0x2D else 1, and #10202(unit, 0x100, 0x64, file) set
//   with flag 0 stops, else #10164 (0xC4FC)(item, unit, n). Otherwise #10202(unit, 0x100, 0x70, file) set with flag 0 stops,
//   else #10164(item, unit, 1).
// Fatal: 0xBF92 (#10265) and 0xBF80 (#10024) then 0x3664 (exit); the port stops there.
#include <cstdint>
#include <cstring>
#include "client_host.h"

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint32_t call(D2cHost h, uint32_t rva, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0) {
    D2cCall c{ rva, 0, 0, 0, 0, 0, { s0, s1, s2, s3 }, { 0, 0, 0, 0 }, s4, 0 };
    return h(&c);
}
void fatal(D2cHost h, uint32_t code) {
    const uint32_t m = call(h, 0xBF92, code);
    call(h, 0xBF80, 0x6FB8359Fu, m);
    call(h, 0x3664, 0xFFFFFFFFu);
}
constexpr uint32_t kFile = 0x6FB84964u;   // the source-file string handed to #10202
}  // namespace

// 1.13c D2Client+0x81FF0
extern "C" void d2_client_81ff0(uint32_t item, uint32_t unit, uint32_t flag, D2cHost h) {
    if (!item) return;
    const bool s14 = call(h, 0xC1CC, unit, 0x14) != 0;
    if (s14 || call(h, 0xC1CC, unit, 0x4A)) {
        const uint32_t base = s14 ? 0x30 : 0x3A;
        if (rd32(item) != 4) { fatal(h, base); return; }
        const uint32_t t = call(h, 0xC1BA, unit ? rd32(unit + 4) : 0xFFFFFFFFu);
        if (!t) { fatal(h, base + 2); return; }
        const uint32_t b = call(h, 0xC80E, rd32(t + 0xF0));
        if (!b) { fatal(h, base + 4); return; }
        const uint32_t a = call(h, 0xCA7E, item, 0);
        if (s14) call(h, 0xCA72, 2, 0, unit, b, a);
        else call(h, 0xCA72, 5, item, unit, b, a);
    } else {
        call(h, 0xCA4E, item, unit, 0, 0);
    }
    if (call(h, 0xC1A8, unit, 0x4000, 0x48, kFile)) return;
    if (rd32(item) == 0) {
        const uint32_t owner = call(h, 0xCA0C, rd32(item + 0x60));
        if (call(h, 0xC6E8, item)) {
            uint32_t n = 1;
            if (owner && unit != owner && call(h, 0xC1CC, unit, 0x2D)) n = 0;
            if (call(h, 0xC1A8, unit, 0x100, 0x64, kFile) && !flag) return;
            call(h, 0xC4FC, item, unit, n);
            return;
        }
    }
    if (call(h, 0xC1A8, unit, 0x100, 0x70, kFile) && !flag) return;
    call(h, 0xC4FC, item, unit, 1);
}
