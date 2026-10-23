// src/core/d2client/dup_group_9d9d0.cpp -- 1.13c D2Client+0x9D9D0, mapping lane. Global 11CBC14 (0x6FBCBC14) is a parameter.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static inline int16_t rd16(uint32_t a) { return *(const int16_t*)(uintptr_t)a; }
typedef uint32_t (*Find4D630)(uint32_t ecx, uint32_t id);

// 1.13c D2Client+0x9D9D0 (80 B): eax=rec (rec[0]==0), rec2 = 9D630(ecx, rec[3]); if rec2 has s16 [+0x22] != -1,
// returns 1 when another node on the 11CBC14 chain (next +0x80) shares that [+0x22] with [+0x10] != rec[3].
// GUESS: extraout_EDX == rec[3] (the id passed to 9D630), as in 9D670.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_9d9d0(uint32_t ecx, uint32_t rec, uint32_t head_bc14, Find4D630 sub_4d630) {
    if (!rec || rd32(rec)) return 0;
    uint32_t id = rd32(rec + 0xc), r2 = sub_4d630(ecx, id);
    if (!r2 || rd16(r2 + 0x22) == -1) return 0;
    for (uint32_t n = head_bc14; n; n = rd32(n + 0x80))
        if (rd16(n + 0x22) == rd16(r2 + 0x22) && rd32(n + 0x10) != id) return 1;
    return 0;
}
