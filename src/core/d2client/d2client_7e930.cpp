// 1.13c D2Client+0x7E930
// Leave the targeting mode: run 0x7CEF0 when the 0x113ACA latch is clear, else 0x486A0 with EAX = the dword at 0x113AC2 (a register
// the decompile lost); then, when the 0xDD5D4 flag is set, restart the repeat timer (0xFAD50 = GetTickCount() + 100); clear the latch.
// g (pinned module globals): +0 0x113ACA latch, +4 0x113AC2, +8 0xDD5D4 flag, +12 0xFAD50 deadline.
#include <emscripten.h>
#include <cstdint>
#include <cstring>

struct D2c7e930Env {
    void (*c7cef0)();
    void (*c486a0)(uint32_t eax);
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_7e930(uint8_t* g, uint32_t now, const D2c7e930Env* env) {
    uint32_t latch, ac2, flag;
    memcpy(&latch, g + 0, 4); memcpy(&ac2, g + 4, 4); memcpy(&flag, g + 8, 4);
    if (latch == 0) env->c7cef0(); else env->c486a0(ac2);
    if (flag != 0) { uint32_t d = now + 100; memcpy(g + 12, &d, 4); }
    uint32_t z = 0; memcpy(g + 0, &z, 4);
}
