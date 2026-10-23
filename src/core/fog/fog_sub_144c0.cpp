#include <cstdint>
#include <emscripten.h>

extern "C" uint32_t g_fog_dbg_inited;  // retail DAT_6ffa6e54

// 1.13c Fog+0x144C0: dbghelp worker shutdown; handles/module are host state, only the flag exists here.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_sub_144c0(void) {
    g_fog_dbg_inited = 0;
    return 1;
}

// 1.13c Fog+0x16900 (ordinal 10001): WSAStartup(0x101) == 0; no sockets on wasm, reports success.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t fog_sub_16900(void) { return 1; }
