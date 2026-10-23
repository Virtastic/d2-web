// 1.13c D2Win+0x0F4E0 -- control activate/toggle handler, from the cached Ghidra decompile.
#include <cstdint>
#include <unistd.h>

// Host seams: gdwBitMasks, Ordinal_10070 (post-toggle notify), DAT_6f9014a8 (callback-enable counter).
struct D2WinF4E0Env {
    const uint32_t* bitMasks;
    void (*ord10070)();
    int32_t dat14a8;
    void (*fatal)(uint32_t strId);  // Ordinal_10265 + Ordinal_10024 then _exit(-1)
};

static void f4e0Fatal(const D2WinF4E0Env* e, uint32_t id) { if (e->fatal) e->fatal(id); _exit(-1); }

// param_1[0] = control (type 6). Dwords: [2] flags, [0xd] callback, [0x10] style (2 toggle, 4 no-notify),
// [0x11]/[0x12] state pair. Returns 0 if disabled, 1 otherwise. Wrong type fatal 0x1fd, callback with
// dat14a8<1 fatal 0x221. ponytail: 32-bit retail-layout callback pointer must fit 32 bits.
extern "C" uint32_t d2win_sub_F4E0(int32_t* param_1, const D2WinF4E0Env* env) {
    int32_t* c = (int32_t*)(intptr_t)param_1[0];
    if (c[0] != 6) f4e0Fatal(env, 0x1fd);
    if (!(env->bitMasks[2] & (uint32_t)c[2]) || !(env->bitMasks[0] & (uint32_t)c[2])) return 0;
    if (!(c[0x10] & 2)) {
        if (c[0x11] != 0 && c[0x12] != 0) return 1;
        c[0x11] = 1; c[0x12] = 1;
    } else {
        c[0x11] = c[0x11] == 0; c[0x12] = c[0x12] == 0;
    }
    if (!(c[0x10] & 4)) env->ord10070();
    if ((c[0x10] & 3) && c[0xd]) {
        if (env->dat14a8 < 1) f4e0Fatal(env, 0x221);
        ((void (*)(int32_t*))(intptr_t)(uint32_t)c[0xd])(param_1);
    }
    return 1;
}
