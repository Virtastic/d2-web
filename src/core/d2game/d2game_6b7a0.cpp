// 1.13c D2Game+0x6B7A0. Retail: stdcall 2 (arg1, arg2 stack slot; ret 8), ESI = the caller's register input.
// Callees: 0xA544 (stdcall 2: unit, out {x, y}); 0xA478 (stdcall 1 for P; then stdcall 4 with the arg2 slot as its out-pointer);
// 0xEEF10 (fastcall ECX = record, plain ret); 0xB15C (fastcall ECX, EDX; no stack words); 0xEF650 (fastcall ECX = arg2 value,
// EDX = 1, stdcall 6: arg1, out, x, y, 3, 8).
// Record R (10 dwords, the stack block at ECX): {arg1, P, 0, arg2, 1, 0, x, y, -1, 0x88}. The 0x88 is a 16-bit store, so its high
// half is residue (the port zeroes it, the capture masks the word).
// 1.13c D2Game+0x6B7A0
#include <cstdint>
#include <emscripten.h>

struct D2game6b7a0Env {
    void (*a544)(uint32_t unit, uint32_t* xy);
    uint32_t (*a478_1)(uint32_t unit);
    uint32_t (*a478_4)(uint32_t unit, uint32_t x, uint32_t tag, uint32_t* out);
    uint32_t (*eef10)(uint32_t* rec);
    void (*b15c)(uint32_t ecx, uint32_t* edx);
    void (*ef650)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5);
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2game_6b7a0(uint32_t unit, uint32_t arg1, uint32_t* arg2slot, const D2game6b7a0Env* env) {
    const uint32_t arg2 = *arg2slot;
    uint32_t xy[2] = { 0, 0 };
    env->a544(unit, xy);
    const uint32_t x = xy[0], y = xy[1];
    const uint32_t p = env->a478_1(unit);
    uint32_t rec[10];
    rec[0] = arg1; rec[1] = p; rec[2] = 0; rec[3] = arg2; rec[4] = 1;
    rec[5] = 0; rec[6] = x; rec[7] = y; rec[8] = 0xFFFFFFFFu; rec[9] = 0x88;
    const uint32_t found = env->eef10(rec);
    if (found) return;
    const uint32_t yv = env->a478_4(unit, found, 0x3F11, arg2slot);
    env->b15c(yv, &rec[1]);
    const uint32_t out = *arg2slot;
    if (!out) return;
    env->ef650(arg2, 1, arg1, out, x, y, 3, 8);
}
