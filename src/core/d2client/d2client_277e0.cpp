// 1.13c D2Client+0x277E0 (193 B): inventory-slot hover update. EDI = slot-table argument (register input).
//   cursor = Ordinal 11017 thunk (0xC202)(edi); no cursor or no hover (global 0xCC468) means nothing.
//   Ordinal 0xCF22(g[0xA5384], g[0xB99C], &info) fills a 0x10C-byte block, info[4] = slot count.
//   idx = global 0xA538C; idx < 0 or idx >= info[4] (unsigned byte) means nothing.
//   if !0xCF28(edi, idx) && 0xC850(cursor): 0x26820(idx) with ESI = 1; else 0x26820(idx) with ESI = (0xCF2E(edi, cursor, idx, &out) ? 3 : 0).
#include <cstdint>

struct D2Client277e0Env {
    uint32_t (*cursor)(uint32_t inv);                                    // 0xC202, stdcall 1 (the pushed EDI)
    void (*info)(uint32_t a, uint32_t b, uint8_t* block);                   // 0xCF22, stdcall 3
    uint32_t (*blocked)(uint32_t edi, uint32_t idx);                        // 0xCF28, stdcall 2
    uint32_t (*valid)(uint32_t cursor);                                     // 0xC850, stdcall 1
    uint32_t (*test)(uint32_t edi, uint32_t cursor, uint32_t idx, uint32_t* out);   // 0xCF2E, stdcall 4
    void (*select)(uint32_t idx, uint32_t mode);                            // 0x26820: ESI = mode, stack = idx
};
// g[0] = 0xCC468 hover enable, g[1] = 0xA538C selected slot, g[2] = 0xA5384, g[3] = 0xB99C
extern "C" void d2client_277e0_port(uint32_t edi, const uint32_t* g, const D2Client277e0Env* env) {
    const uint32_t cursor = env->cursor(edi);
    if (!cursor || !g[0]) return;
    uint8_t info[0x10c];
    env->info(g[2], g[3], info);
    const int32_t idx = (int32_t)g[1];
    if (idx < 0 || idx >= (int32_t)info[4]) return;
    if (!env->blocked(edi, (uint32_t)idx) && env->valid(cursor)) { env->select((uint32_t)idx, 1); return; }
    uint32_t out[1];
    env->select((uint32_t)idx, env->test(edi, cursor, (uint32_t)idx, out) ? 3 : 0);
}
