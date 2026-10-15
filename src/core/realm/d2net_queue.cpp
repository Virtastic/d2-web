// 1.13c D2Net+0x6190 D2Net+0x61D0 D2Net+0x6000 D2Net+0x6170 D2Net+0x6070 D2Net+0x6250 D2Net+0x5F10: thunks onto the Fog queue at D2Net+0xB424.
// Specs: and siblings. Model = the fresh-init, empty queue the retail captures was taken on.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace { int32_t g_word884 = 1; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_realm_net_queue_reset() { g_word884 = 1; }

// Returns the thunk's eax; buf16 = the 0xA5-filled buffer; st = queue +0x86C/+0x87C/+0x884 after the call.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_realm_net_queue(uint32_t fn, uint32_t a, uint8_t* buf16, uint32_t* st) {
    int32_t ret = 0;
    switch (fn) {
    case 0x6190: case 0x61D0: ret = -1; break;        // Fog 10156 on an empty pool; dest untouched
    case 0x6000: ret = 0; break;                      // a ? Fog 10173 (nothing queued) : D2Net+0xB428 (0)
    case 0x6170: buf16[0] = 0; break;                 // Fog 10159 miss writes 0 to the out byte
    case 0x6070: ret = 0x51ABCDEF; break;             // void; eax is the queue handle
    case 0x6250: g_word884 = a ? (int32_t)a : 1; break;  // Fog 10151 stores the mode word
    case 0x5F10: break;                               // void (Fog 10183), no visible effect
    }
    st[0] = 8; st[1] = 3; st[2] = (uint32_t)g_word884;
    return ret;
}
