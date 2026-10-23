// D2Client rva 0xA30C0: start-skill dispatch. Callees go through the host seam in retail order.
#include <cstdint>
#include "client_host.h"

static uint32_t go(D2cHost h, D2cCall c) { return h(&c); }

// 1.13c D2Client+0xA30C0. ECX = unit (param_1, +0 type), EDX = req (param_2: +0 a, +4 b, +8 idx, +0xC x).
// tab = address of D2Client+0xBA608 table (rows 0x80 bytes, indexed by req[2]).
extern "C" uint32_t d2_client_a30c0(D2cHost h, uint32_t unit, const uint32_t* unitp, uint32_t reqaddr, const uint32_t* req, uint32_t tab) {
    if (go(h, { 0xC62E, 0, 0, 0, 0, 0, { unit, 0, 0, 0 } }) != 0 && (unit == 0 || unitp[0] != 1))
        go(h, { 0xA0660, 0, 0, 0, 0, 0, { reqaddr, 0, 0, 0 } });
    uint32_t p = go(h, { 0xA4E20, 0, 0, 0, 0, 0, { req[3], tab + req[2] * 0x80, req[2], 0 } });
    if (p) {
        go(h, { 0xC7C0, 0, 0, 0, 0, 0, { unit, p, 0, 0 } });
        go(h, { 0xA5FB0, 0, 0, 0, 0, 0, { 0, 0, 0, 0 } });
        return go(h, { 0xA1CF0, 0, 0, 0, 0, 0, { req[0], req[1], 0, 0 } });
    }
    go(h, { 0x2A750, 0, 0, 0, 0, 0, { 7, unit, 0, 0 } });
    go(h, { 0xC6E8, 0, 0, 0, 0, 0, { 1, 2, 4, 0x7c2 } });  // Skills.cpp line 0x7c2 "StartsStartSkillID - Invalid Target"
    return 0;
}
