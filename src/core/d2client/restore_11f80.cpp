#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x11F80: one-shot deferred handler. gate = [0x6FBCC540], flag = [0x6FBD349C], seed = src dwords
// [0x6FBC9E68+4i] = seed + 0x01010101*i (0x11D dwords copied to 0x6FBCA2E0). Returns word k of the record after the call:
// 0..3 dst hash/first/last/guard, 4 [0x6FBCC434], 5 [0x6FBD349C], 6 [0x6FBCC2F8], 7 [0x6FBCC2F4], 8 [0x6FBCC540], 9 nCalls,
// 10.. call[i][j] = {id, eax, arg0, arg1} (id 1 = 0x114F0(0), 2 = 0x3A3A0, 3 = Ordinal_10048, 4 = 0xCCC0(h,0x6FBA51BC), 5 = 0xF2880).
// Pre-state of the untouched words: dst 0xDEADBEEF, guard 0xC0DEC0DE, [0x6FBCC434] 0x5A5A5A5A, [..2F8] 0x77, [..2F4] 0x88.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_11f80(uint32_t gate, uint32_t flag, uint32_t seed, uint32_t k) {
    uint32_t w[10] = {}, h = 2166136261u;
    uint32_t p434 = 0x5A5A5A5A, p2f8 = 0x77, p2f4 = 0x88, n = 0;
    for (uint32_t i = 0; i < 0x11D; i++) {
        uint32_t v = gate ? seed + 0x01010101u * i : 0xDEADBEEFu;
        if (i == 0) w[1] = v;
        if (i == 0x11C) w[2] = v;
        for (int b = 0; b < 4; b++) { h ^= (v >> (8 * b)) & 0xFF; h *= 16777619u; }
    }
    w[0] = h; w[3] = 0xC0DEC0DEu;
    uint32_t call[5][4] = {};
    if (gate) {
        call[n][0] = 1; call[n][1] = gate; n++;
        call[n][0] = 2; call[n][1] = gate; n++;
        p434 = 1;
        if (!flag) { flag = 1; call[n][0] = 3; call[n][1] = 1; n++; call[n][0] = 4; call[n][1] = 4; call[n][2] = 0xD2480048u; call[n][3] = 0x6FBA51BCu; n++; }
        call[n][0] = 5; call[n][1] = n == 4 ? 4 : 1; n++;
        p2f8 = p2f4 = 0; gate = 0;
    }
    w[4] = p434; w[5] = flag; w[6] = p2f8; w[7] = p2f4; w[8] = gate; w[9] = n;
    if (k < 10) return w[k];
    k -= 10;
    return k < 20 ? call[k >> 2][k & 3] : 0;
}
