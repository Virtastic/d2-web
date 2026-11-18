// 1.13c D2Client+0x22DA0 --
// Remove the unit's sounds whose category matches the monster-sound config. Retail 32-bit layouts: the
// unit's sound list (unit+0x78, nodes {id,next}), the entry list (global 0x11BF30, nodes {entry*,next*},
// entry+4 index, +8 id), the table (0x11C24C, 0x8E-byte rows, count 0x11C250, category at +0x60) are
// read through `g` = {entry list head, table, count}. Callees go through the host seam in retail order.
#include <emscripten.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "client_host.h"

static uint32_t rd(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_22da0(uint32_t unit, const uint32_t* g, D2cHost host) {
    D2cCall c = {};
    c.rva = 0x22430; c.eax = unit; c.esi = unit;
    if (host(&c) != 1) return;
    c.rva = 0x22A70; c.eax = unit; c.esi = unit;
    uint32_t cfg = host(&c);
    if (!cfg) return;
    if (!unit) std::exit(-1);   // retail: fatal-error path
    static const uint8_t offs[] = { 0x04, 0x1C, 0x34, 0x68, 0x44, 0x48, 0x4C, 0x50, 0x70, 0x74, 0x78 };
    for (uint32_t n = rd(unit, 0x78); n;) {
        uint32_t id = rd(n, 0), next = rd(n, 4);
        n = next;
        int32_t idx = 0;
        if (id) for (uint32_t e = g[0]; e; e = rd(e, 4)) { uint32_t d = rd(e, 0); if (rd(d, 8) == id) { idx = (int32_t)rd(d, 4); break; } }
        int32_t cat = 0;
        if (g[1]) {
            if (idx < 0 || idx >= (int32_t)g[2]) std::abort();   // retail reads address 0x60 here (null row): access violation
            cat = (int32_t)rd(g[1] + (uint32_t)idx * 0x8E, 0x60);
        }
        for (uint8_t o : offs)
            if (cat == (int32_t)rd(cfg, o)) { c = {}; c.rva = 0x88C20; c.eax = id; c.s[0] = unit; c.s[1] = 1; host(&c); break; }
    }
}
