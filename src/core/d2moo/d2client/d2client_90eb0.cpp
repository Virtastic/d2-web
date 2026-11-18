// 1.13c D2Client+0x90EB0 (stdcall 1, ret 4): draw a name clipped to 0xA0 pixels. 32 Unicode ctors on a stack buffer,
// old = 0xD3A8(ECX 6) (set font), 0xA5D90(EAX id) returns the name, Unicode::strncpy(buf, name, 0x20), buf[31] = 0;
// while 0xD35A(ECX buf) (text width) > 0xA0 the last char is zeroed, from buf[30] down; then
// 0xD37E(ECX buf, EDX [0x11B9A0] + 0xF, push 0xD6 - [0x11B9A4], 0, 0) (draw) and 0xD3A8(ECX old).
#include <cstdint>
#include <emscripten.h>
#include "d2client_callee.h"

// EAX at return: the restoring 0xD3A8's result.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_90eb0(const D2cSvc& v, uint32_t id, int32_t g9a0, int32_t g9a4) {
    char16_t buf[32] = {};
    const uint32_t old = d2c_call(v, 0xd3a8, 0, 6, 0, 0, 0);
    const char16_t* s = (const char16_t*)(uintptr_t)d2c_call(v, 0xa5d90, id, 0, 0, 0, 0);
    int i = 0;
    for (; i < 32 && s[i]; ++i) buf[i] = s[i];   // Unicode::strncpy(buf, s, 0x20)
    if (i < 32) buf[i] = 0;
    buf[31] = 0;
    const uint32_t b = (uint32_t)(uintptr_t)buf;
    char16_t* p = &buf[31];
    while ((int32_t)d2c_call(v, 0xd35a, 0, b, 0, 0, 0) > 0xa0) *--p = 0;
    d2c_call(v, 0xd37e, 0, b, (uint32_t)(g9a0 + 0xf), 0, 0, (uint32_t)(0xd6 - g9a4), 0, 0);
    return d2c_call(v, 0xd3a8, 0, old, 0, 0, 0);
}
