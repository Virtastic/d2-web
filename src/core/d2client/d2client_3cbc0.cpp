#include <emscripten.h>
#include <cstdint>
#include <cstring>

// 1.13c D2Client+0x3CBC0 (stack = p): SendMessageA(p[0], 0x10 (WM_CLOSE), 0, 0); p[6] = 1; p[7] = 0.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_3cbc0(uint32_t* p, void (*sendMessage)(uint32_t, uint32_t, uint32_t, uint32_t)) {
    sendMessage(p[0], 0x10, 0, 0);
    p[6] = 1; p[7] = 0;
}
