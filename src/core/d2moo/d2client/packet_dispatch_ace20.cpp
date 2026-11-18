// 1.13c D2Client+0xACE20 (491 B): server-packet dispatch loop, read off the disassembly.
// Buffer in EAX, length the one stack word (ret 4). Module globals are reached at base + RVA (base = the module's
// load address; the table at 0xDDE60 is 0xAF entries of 12 bytes {handler, size, flag}); callees go through Env::H
// with the registers and stack words retail passes. GetTickCount is called first and its result is unused.
#include <cstdint>
#include "cl_env.h"

namespace {
constexpr uint32_t kTbl = 0xDDE60, kHash = 0x10A608, kBytes = 0x11C4DC, kPass = 0x11C4E4, kBig = 0x11C4EC, kLast = 0x123414;

void fatal(const cl::Env& e, uint32_t base, uint32_t code) {
    uint32_t v = e.H(0xbf92, code);
    e.H(0xbf80, base + 0xd359f, v);
    e.H(0x3664, 0xffffffffu);
}
// the player list (type 1) bucket walk: chain at +0xE4, id at +0xC
uint32_t findPlayer(const cl::Env& e, uint32_t base, uint32_t id) {
    for (uint32_t u = e.R(base + kHash + 0x200 + (id & 0x7f) * 4); u; u = e.R(u + 0xe4))
        if (e.R(u + 0xc) == id) return u;
    return 0;
}
}

// 1.13c D2Client+0xACE20
extern "C" void d2c_ace20(const cl::Env& e, uint32_t base, uint32_t buf, int32_t len) {
    if (len == -1) return;
    if (len > 500) e.W(base + kBig, e.R(base + kBig) + 1);
    int32_t left = len;
    if (left > 0) for (;;) {
        uint32_t op = e.R8(buf), n = 0;
        if (!e.H(0xd246, (uint32_t)(uintptr_t)&n, 0, 0, 0, 0, 0, buf, (uint32_t)len)) return;
        if (op >= 0xaf) { fatal(e, base, 0x1448); return; }
        uint32_t ent = base + kTbl + op * 12, sz = e.R(ent + 4), n16 = n & 0xffff;
        if (sz != 0xffffffffu && n16 != sz) { fatal(e, base, 0x144f); return; }
        if (e.R(ent + 8)) {
            uint32_t unit;
            if (op >= 0x67 && op <= 0x6d) {
                uint32_t id = e.R(buf + 1);
                unit = findPlayer(e, base, id);
                if (unit && e.R(unit) != 1) { fatal(e, base, 0x5a); return; }
            } else {
                uint32_t ty = e.R8(buf + 1), id = e.R(buf + 2);
                unit = e.H(0xa4e20, ty, 0, 0, 0, 0, id & 0x7f, id, base + kHash + ty * 0x200);
            }
            if (unit) e.H(0xac630, buf, 0, 0, 0, 0, n16, unit);
        }
        uint32_t loc = 0;
        switch (op) {
        case 0x18: e.H(0xac240, 0, 0, 0, 0, 0, buf, 0, 0, 0, (uint32_t)(uintptr_t)&loc); break;
        case 0x95: e.H(0xac1b0, 0, 0, 0, 0, 0, buf, 0, 0, 0, (uint32_t)(uintptr_t)&loc); break;
        case 0x96: e.H(0xac140, 0, 0, 0, 0, 0, buf, 0, 0, 0, (uint32_t)(uintptr_t)&loc); break;
        case 0x0d:
            if (e.R8(buf + 1) == 1) {
                uint32_t u = findPlayer(e, base, e.R(buf + 2));
                if (u && e.R(u) != 1) { fatal(e, base, 0x5a); return; }
            }
            break;
        default: break;
        }
        if (uint32_t hv = e.R(ent)) e.H(hv, 0, 0, 0, 0, 0, 0, buf);
        e.W(base + kBytes, e.R(base + kBytes) + n16);
        buf += n16;
        left -= (int32_t)n16;
        e.W(base + kLast, (e.R(base + kLast) & 0xffffff00u) | op);
        if (left <= 0) break;
    }
    e.W(base + kPass, e.R(base + kPass) + 1);
}
