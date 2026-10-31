// D2Client 0x46880: the current unit's panel anchor (ESI = POINT; callers 0x48B20, 0x46EE0, 0x48930, 0x46C00, 0x49D80).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
extern "C" uint32_t* d2_client_46150(uint8_t, uint32_t, const uintptr_t*, uint32_t* (*)(uintptr_t), void (*)());
// 1.13c D2Client+0x46880 ; pt = ESI, view = [0x11C3C4] (left at +0x24, top at +0x28; SetRect's right/bottom are never read back),
// g9721/g971d/h = the 0x46150 lookup state. Ordinal_10628 (thunk 0xC21A, stack unit, pt) goes through host. The decompile's EBX/EBP
// inputs are the SetRect locals: the disassembly reads rect.left and rect.top back off the stack.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_46880(int32_t* pt, const uint8_t* view, uint8_t g9721, uint32_t g971d,
                                                        const uintptr_t* h, void (*fatal)(), D2cHost host) {
    if (!d2_client_46150(g9721, g971d, h, nullptr, fatal)) return 0;
    D2cCall c = {}; c.rva = 0xC21A;
    c.s[0] = (uint32_t)(uintptr_t)d2_client_46150(g9721, g971d, h, nullptr, fatal); c.s[1] = (uint32_t)(uintptr_t)pt; host(&c);
    int32_t left, top; std::memcpy(&left, view + 0x24, 4); std::memcpy(&top, view + 0x28, 4);
    pt[1] = pt[1] + (-0x96 - top);
    pt[0] -= left;
    if (pt[1] < 0x14) pt[1] = 0x14;
    return 1;
}
