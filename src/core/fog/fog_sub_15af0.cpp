// 1.13c Fog+0x15AF0 -- crash-report DbgHelp SymInitialize wrapper (refcounted, under the Fog critical section).
// Retail returns 0 when DbgHelp was never loaded (flag DAT_6ffa6e54 == 0). wasm has no DbgHelp, so that is always the case.
#include <cstdint>

#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_sub_15af0(void) {  // 1.13c Fog+0x15AF0
    return 0;  // DbgHelp absent: no refcount change, no module enumeration
}
