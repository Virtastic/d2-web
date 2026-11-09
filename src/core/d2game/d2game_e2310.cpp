// 1.13c D2Game+0xE2310, from the disassembly. Walks the list at unit+0xDC (next at +0x24) twice (nodes of kind 0, then kind 1) through
// the +0x8E6F0 routine, sets unit+0x44 = (game[0xA8] - p2) << 8, takes (count, limit) from unit+0x3C/+0x34 (unit+0x30 set) or from
// int16 unit+0x4C / unit+0x48, and then steps the fixed-point position from p2 << 8 by count up to limit, calling +0x8EA80 for each
// table value 1..4 read from unit+0x50 (+0x10 + index) or ordinal +0xA5F8. A null unit skips the list walks only (retail then
// dereferences it).
#include "d2game_e2310.h"
#include <cstring>
namespace {
uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
}
uint32_t d2game_e2310_port(uint32_t ebx, uint32_t game, uint32_t p2, const D2GameE2310Env* e) {
    if (ebx) {
        for (uint32_t kind = 0; kind < 2; kind++)
            for (uint32_t n = rd(ebx, 0xdc); n;) {
                const uint32_t next = rd(n, 0x24);
                if (*(const uint8_t*)(uintptr_t)n == kind) e->e6f0(n, game);
                n = next;
            }
    }
    int32_t edi = (int32_t)rd(game, 0xa8);
    const uint32_t v44 = (uint32_t)(edi - (int32_t)p2) << 8;
    std::memcpy((void*)(uintptr_t)(ebx + 0x44), &v44, 4);
    int32_t cnt, limit;
    if (rd(ebx, 0x30)) { cnt = (int32_t)rd(ebx, 0x3c); limit = (int32_t)rd(ebx, 0x34); }
    else { int16_t w; std::memcpy(&w, (const void*)(uintptr_t)(ebx + 0x4c), 2); cnt = w; limit = (int32_t)rd(ebx, 0x48); }
    if (cnt == 0) return e->ea80(game, ebx, 1, (uint32_t)(rd(game, 0xa8) + 1), 0, 0, 0);
    const uint32_t tbl = rd(ebx, 0x50);
    int32_t esi = (int32_t)p2 - 1;
    int32_t ecx = (int32_t)((uint32_t)p2 << 8);
    int32_t eax = ecx + cnt;
    while (eax < limit) {
        ecx += cnt; eax += cnt; edi++;
        const int32_t hi = ecx >> 8;
        if (!(esi > hi)) {
            do {
                int32_t val;
                if (rd(ebx, 0x30)) { uint32_t o = 0; e->a5f8(rd(ebx, 0x30), (uint32_t)esi << 8, &o); val = (int32_t)o; }
                else val = *(const uint8_t*)(uintptr_t)(tbl + (uint32_t)esi + 0x10);
                if (val > 0 && val <= 4) e->ea80(game, ebx, 0, (uint32_t)edi, 0, (uint32_t)val, 0);
                esi++;
            } while (esi <= hi);
        }
    }
    if (edi == (int32_t)rd(game, 0xa8)) edi++;
    edi++;
    return e->ea80(game, ebx, 1, (uint32_t)edi, 0, 0, 0);
}
