// 1.13c D2Client+0x49090 -- when [11976F] != 0: A3D20(); the object at [11977B] (if any) gets its first vtable entry called thiscall with 1 (ECX = object);
// [11977B] = 0; [11976F] = 0. Then 48B20() and the result 1. The vtable entry is a module address (the recorded rva comes from it).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_hcall.h"
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_49090(D2cHost h, uint32_t* g11976f, uint32_t* g11977b) {
    if (*g11976f) {
        d2cCall(h, 0xA3D20);
        uint32_t obj = *g11977b;
        if (obj) { uint32_t vt, fn; std::memcpy(&vt, (const void*)(uintptr_t)obj, 4); std::memcpy(&fn, (const void*)(uintptr_t)vt, 4); d2cCall(h, fn & 0xFFFFFF, 1, 0, 0, 0, 0, obj); }
        *g11977b = 0; *g11976f = 0;
    }
    d2cCall(h, 0x48B20);
    return 1;
}
