#include <cstdint>
#include <emscripten.h>
#include "../d2gfx/d2gfx_cinit.h"

// 1.13c D2Gfx+0x013DE -- MSVC 2003 Release CRT __cinit, the module's
// C/C++ initializer dispatcher, called once by __CRT_INIT@12 0x1686 with init = 0. Retail base
// 0x6FA80000; body 0x13DE..0x1447, 106 bytes. If the dynamic-initializer hook at +0x1EA38 is non-NULL
// it is called with the argument (the slot sits in unbacked .data, so the shipped image has NULL).
// Walk the C-initializer table +0x10010..+0x10024 (five dwords, end-exclusive) in order, checking the
// accumulated status before every slot, and stop at the first nonzero one. When the walk succeeded,
// register the exit thunk 0x1AE6 with _atexit 0x1674, then walk the C++-initializer table
// +0x10000..+0x1000C (three dwords) and call every non-NULL slot without looking at its result; return
// 0 (retail `xor eax,eax`).
//
// The tables read out of the shipped .data image (RVA 0x10000):
//   C  initializers @ 0x10010: [NULL, 0x1614 ___onexitinit, 0x3FEB, 0x5870 ___initmbctable, 0xC94A]
//   C++ initializers @ 0x10000: [NULL, 0x6026 ___security_init_cookie, 0xCC20]
// Slot 1 now points at the ported onexit initializer; the other unported slots remain NULL.
extern "C" int32_t d2gfx_onexitinit();
extern "C" uint32_t d2gfx_onexit(uint32_t func);
static int32_t register_exit(uint32_t func) {
    return d2gfx_onexit(func) == 0 ? -1 : 0;
}
static const D2GfxCinitState g_module_initializers{
    nullptr,
    {nullptr, d2gfx_onexitinit, nullptr, nullptr, nullptr},
    register_exit,
    {nullptr, nullptr, nullptr}
};

extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2gfx_cinit(int32_t init) {
    return d2gfx_cinit_state(init, &g_module_initializers);
}
