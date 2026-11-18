// 1.13c D2Client+0xA4040 (663 B): modal dialog-box node constructor, read off the disassembly.
// stdcall, six stack words (ret 0x18): a1/a2 position, a3 kind (0 = none), a4, a5 name index (the node is built only
// when a5 < 1), a6. Module globals are reached at base + RVA; callees go through Env::H with the registers and stack
// words retail passes. The node is 0xB10 bytes from the allocator (ECX size, EDX file string, stack line and 0).
#include <cstdint>
#include <initializer_list>
#include "cl_env.h"

namespace {
constexpr uint32_t kRun = 0xFADA8, kHead = 0x11BC00, kOpen = 0x11BC04, kCount = 0x11BC08, kSeq = 0x10B208, kNames = 0xDE920;
constexpr uint32_t kTick = 0xCEF5C;   // kernel32!GetTickCount IAT slot
void fatal(const cl::Env& e, uint32_t base, uint32_t code) {
    uint32_t v = e.H(0xbf92, code);
    e.H(0xbf80, base + 0xd359f, v);
    e.H(0x3664, 0xffffffffu);          // exits in retail; returns only under a recorder
}
}

// 1.13c D2Client+0xA4040
extern "C" uint32_t d2c_a4040(const cl::Env& e, uint32_t base, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                              int32_t a5, uint32_t a6) {
    if (e.R(base + kRun)) { uint32_t s = e.H(0xd0b4, base + 0xd0780); e.H(0x3cc60, s, 0, 0, 0, 0, 5); }
    e.W(base + 0xF46C8, 0x10); e.W(base + 0xF46C4, 0x10);
    e.W(base + 0xE0EA0, 0);
    for (uint32_t o = 0xE0EA8; o <= 0xE0EBC; o += 4) e.W(base + o, 0xffffffffu);
    for (uint32_t o : { 0x11BC28u, 0x11BC2Cu, 0x11BC38u, 0x11BC3Cu, 0x11C3DCu, 0x11C3E0u, 0x11C3F8u, 0x11C3FCu, 0x11C2F8u, 0x11C2F4u })
        e.W(base + o, 0);
    if (a3 == 0 || a5 >= 1) return 0;
    e.W(base + kCount, e.R(base + kCount) + 1);
    uint32_t n = e.H(0xbf7a, 0x399, 0, 0, 0, 0, 0, 0xb10, base + 0xd41d4);
    for (uint32_t o = 0; o < 0xb10; o += 4) e.W(n + o, 0);
    e.W(n + 0xb0c, e.R(base + kHead));
    for (uint32_t o = 0; o < 0xb10; o += 4) e.W(n + o, 0);   // retail clears twice; the list link ends 0
    e.W(base + kHead, n);
    e.W(n + 0x34, 0); e.W(n + 0x38, 0); e.W(n + 0x50, 0); e.W(n + 0x4c, 0); e.W(n + 0x44, 0xffffffffu);
    e.W(n + 0x5c, 2); e.W(n + 0x2c, 0); e.W(n + 0x30, 0); e.W(n + 0xb08, 0);
    e.W(n + 0x60, a3); e.W(n + 0x64, a4); e.W(n + 0x58, 0); e.W(n + 0x14, 0);
    e.W(n, e.H(kTick));
    e.W(n + 0x18, a6); e.W(n + 0x48, 0xffffffffu); e.W(n + 0x10, 0); e.W(n + 0x54, 0); e.W(n + 0xc, 1);
    char buf[0x100];
    uint32_t pb = (uint32_t)(uintptr_t)buf;
    e.H(0x3146, pb, base + 0xd41c4, base + 0xd357c, e.R(base + kNames + (uint32_t)a5 * 4));
    e.W(n + 8, e.H(0x2b420, 0, 0, 0, 0, 0, pb));
    uint32_t f[0x12] = {};
    f[0xd] = e.R(n + 8);
    uint32_t h = e.H(0xc0e8, (uint32_t)(uintptr_t)f);
    if (!h) fatal(e, base, 0x3b6);
    e.W(n + 0x34, e.H(0xc0d6, h));
    e.W(n + 0x38, e.H(0xc0e2, h));
    e.W(n + 0x24, a1); e.W(n + 0x28, a2); e.W(n + 0x2c, a1); e.W(n + 0x30, a2);
    e.W(base + kOpen, 1);
    e.H(0xa3a20);
    e.H(0x3ccc0, e.H(0xd0b4, base + 0xde858), 0, 0, 0, 0, 0xe);
    if (e.H(0x42090)) e.H(0x3ccc0, e.H(0xd0b4, base + 0xde900), 0, 0, 0, 0, 1);
    e.W(base + kSeq, e.R(base + kSeq) + 1);
    if (e.R(base + kCount) != 1) fatal(e, base, 0x3ce);
    return n;
}
