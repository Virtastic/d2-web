// 1.13c D2Client+0xAB600 (91 B): validates a struct's leading tag, then forwards three of its words.
// Decompile: build-wasm/ghidra-d2client/decomp/0xAB600.c. Import ordinals marked (unverified) are
// read from the decompile only; unrecovered callees are host seams (env-seam pattern).
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

struct D2ClientAb600Env {
    uint32_t tagGlobal;                                  // DAT_6fbc97f8 (RVA 0x1197F8), value read by the caller
    uint32_t (*callerReturnAddress)(uint32_t code);      // Fog 10265 (0x0BF92)
    void (*displayHalt)(uint32_t msg, uint32_t eip);     // Fog 10024 (0x0BF80), DAT_6fb8359f message
    void (*exitProcess)(int32_t code);                   // 0x03664 _exit
    uint32_t (*imp10048)(uint32_t a, uint32_t b, uint32_t c, uint32_t* o1, uint32_t* o2); // (unverified) ordinal 10048
    void (*imp412)(uint32_t v);                          // (unverified) ordinal 412 (0x0BE9C)
};

// in: 4 words {tag, a, b, c}.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_ab600(const uint32_t* in, const D2ClientAb600Env* env) {
    uint32_t o1 = 0, o2 = 0;
    if (in[0] != env->tagGlobal) {
        env->displayHalt(0, env->callerReturnAddress(0x12a));
        env->exitProcess(-1);
        return;
    }
    env->imp412(env->imp10048(in[1], in[2], in[3], &o1, &o2));
}
