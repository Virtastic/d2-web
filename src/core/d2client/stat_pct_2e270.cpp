// 1.13c D2Client+0x2E270 (90 B): adds stat 0x14A (percent bonus) of the live player to two values.
// ABI: ECX = this (object; byte at +0x1DC must be 2), [esp+4] = pA (ebp), EBX = pB, callee pops 4.
// Reads DAT 0x6FBCBBFC (player) via the stat getter D2Common!#10973 (veneer 0x0C1C6).
// Caller: 0x2EDD0.
#include <cstdint>
#include <emscripten.h>

extern "C" int32_t d2_client_14a0(int32_t a, int32_t b, int32_t d);

extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_2e270(const uint8_t* self, int32_t* pA, int32_t* pB, uint32_t player,
                     int32_t (*getStat)(uint32_t unit, uint32_t stat, uint32_t idx)) {
    if (!self || self[0x1DC] != 2) return;
    const int32_t v = getStat(player, 0x14A, 0);
    if (!v) return;
    const int32_t a = *pA;
    *pA = d2_client_14a0(v, a, 100) + a;
    const int32_t b = *pB;
    *pB = d2_client_14a0(v, b, 100) + b;
}
