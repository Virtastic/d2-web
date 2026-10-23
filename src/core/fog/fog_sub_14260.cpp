// 1.13c Fog+0x14260 -- release one reference on the DbgHelp symbol session (inc is Fog+0x15AF0).
// Returns 1 if references remain, 0 if the session was never initialised, was already 0, or this drop
// hit zero (retail then calls SymCleanup(GetCurrentProcess()); no-op on wasm). Critical section is a no-op single-threaded.
#include <cstdint>

#include <emscripten.h>

extern "C" uint32_t g_fog_dbg_inited = 0;  // retail DAT_6ffa6e54, set by Fog+0x164A0, cleared by Fog+0x144C0
extern "C" int32_t g_fog_dbg_refs = 0;   // retail DAT_6ff82ae4

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_sub_14260(void) {  // 1.13c Fog+0x14260
    if (!g_fog_dbg_inited || !g_fog_dbg_refs) return 0;
    return --g_fog_dbg_refs != 0;
}
