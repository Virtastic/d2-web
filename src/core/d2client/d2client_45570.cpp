// D2Client 0x45570: flush the current target's slot value, call 0x888B0, then latch the six
// stdcall args into the 0x11C350.. block (callers 0x23200, 0x53070, 0x530D0).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
static uint32_t rd45570(const uint8_t* p, uint32_t o) { uint32_t v; std::memcpy(&v, p + o, 4); return v; }
static void wr45570(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
// 1.13c D2Client+0x45570 ; ebx = EBX (implicit), p1..p6 = the stdcall args (ret 0x18). g348 = the 0x2C bytes at 0x11C348 (+0 target,
// +8 = 0x11C350 .. +0x28 = 0x11C370 written), g250 = the 0x1C bytes at 0x11C250 (+0 count, +0xC = 0x11C25C base, +0x18 = 0x11C268),
// rt = [0x11C24C] (0x8E-stride records; 0 or an out-of-range target leaves the record null), slots = [0x1175B8], list = [0x11BF30]
// (nodes {item, next}; item +4 id, +8 handle), frame = [0x1197F8]. 0x88080 (ECX = EDX = handle) and 0x888B0 go through host.
// A nonzero EBX with a null record faults in retail (it reads [ebx*4+0x50]); the port reads through `at` the same way.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_45570(uint32_t ebx, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5,
                                                    uint32_t p6, uint8_t* g348, const uint8_t* g250, uint32_t rt, uint32_t slots,
                                                    uint32_t list, uint32_t frame, uint32_t* (*at)(uint32_t), D2cHost host) {
    int32_t tgt = (int32_t)rd45570(g348, 0);
    if (tgt) {
        uint32_t rec = (rt && tgt >= 0 && tgt < (int32_t)rd45570(g250, 0)) ? rt + (uint32_t)tgt * 0x8E : 0;
        for (uint32_t n = list; n; n = at(n)[1]) {
            const uint32_t* it = at(at(n)[0]);
            if ((int32_t)it[1] != tgt) continue;
            uint32_t hnd = it[2];
            if (hnd) {
                D2cCall c = {}; c.rva = 0x88080; c.ecx = hnd; c.edx = hnd;
                if (host(&c)) {
                    uint32_t v = ebx ? at(rec + ebx * 4 + 0x50)[0] : 0;
                    at(slots + (uint32_t)(tgt - (int32_t)rd45570(g250, 0xC)) * 4)[0] = v;
                    wr45570(g348, 8, rd45570(g250, 0x18));
                }
            }
            break;
        }
    }
    D2cCall d = {}; d.rva = 0x888B0; host(&d);
    wr45570(g348, 0x10, p1); wr45570(g348, 0x0C, 1); wr45570(g348, 0x18, 1); wr45570(g348, 0x14, p2 + frame);
    wr45570(g348, 0x1C, p4); wr45570(g348, 0x20, frame + p5); wr45570(g348, 0x24, p6); wr45570(g348, 0x28, frame + p3);
}
