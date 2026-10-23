// Mapping lane: D2Client+0xC3090, teardown of a global object. Decompile derived; not yet vectored.
// Seam: callees via D2cHost by RVA (no args). Object vtable[0](this-less, arg 1) is the deleting dtor; slot 0 call goes through 0xC3090's own pointer.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32;
struct D2cC3090G {
    u32** obj /*1C977B (RVA 0x11977B): object ptr, first dword = vtable*/;
    u32* live /*1C976F (RVA 0x11976F): nonzero while obj owned*/;
};
extern "C" {
// 1.13c D2Client+0xC3090
EMSCRIPTEN_KEEPALIVE void d2c_c3090(D2cHost h, D2cC3090G* g, D2cHost vcall) {
    D2cCall c = {};
    c.rva = 0x7EE50; h(&c);
    if (*g->obj == nullptr) return;
    if (*g->live != 0) {
        c = {}; c.rva = 0xA3D20; h(&c);
        if (*g->obj != nullptr) { D2cCall v = {}; v.rva = **g->obj; v.s[0] = 1; vcall(&v); }  // (**vtbl)(1)
        *g->obj = nullptr;
        *g->live = 0;
    }
    c = {}; c.rva = 0x48B20; h(&c);
}
}
