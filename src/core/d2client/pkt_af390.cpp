// 1.13c D2Client+0xAF390 (82 B): packet handler -> sub_2A750(pkt[5], unit, &cmd) with a 28-byte command.
// Ordinal: none (internal static, unverified). Callee is a host seam.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2ClientAf390Env {
    void (*sub2a750)(uint32_t mode, void* unit, uint32_t* cmd);   // D2Client+0x2A750
};

// 1.13c D2Client+0xAF390  (__fastcall: ECX=unit, EDX=packet)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_af390(void* unit, const uint8_t* pkt, D2ClientAf390Env* e) {
    uint16_t a, b; memcpy(&a, pkt + 6, 2); memcpy(&b, pkt + 8, 2);
    uint32_t c[7] = { a, b, pkt[10], 2, 0, 4, pkt[11] };
    e->sub2a750(pkt[5], unit, c);
}
