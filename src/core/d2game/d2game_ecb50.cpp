// 1.13c D2Game+0xECB50 -- packet cursor dispatch by section code
// Registers: EAX = end, ECX = src (param_1), EDX = op, EBX = &cursor, EDI = edi argument; stack word 1 = error out (ret 8).
// Callees are seams in retail order: 0x54D90 (thiscall ECX = cursor, 5 stack words) for op 0x47, 0x54810 (EAX = cursor, ECX = src,
// 5 stack words) for 0x48..0x60. Both write the consumed size through the last word.
#include <cstdint>
#include <emscripten.h>

struct D2GameECB50Env {
    int32_t (*f54D90)(uint32_t cur, uint32_t src, uint32_t edi, uint32_t len, uint32_t zero, uint32_t* out);
    int32_t (*f54810)(uint32_t cur, uint32_t src, uint32_t edi, uint32_t op, uint32_t len, uint32_t zero, uint32_t* out);
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_ECB50(uint32_t op, uint32_t src, uint32_t edi, uint32_t end, uint32_t* ebx, uint32_t* errOut, const D2GameECB50Env* env) {  // 1.13c D2Game+0xECB50
    const uint32_t cur = *ebx;
    uint32_t out = 0;
    int32_t r;
    if (op == 0x47) {
        r = env->f54D90(cur, src, edi, end - cur, 0, &out);
    } else if (op < 0x48 || op > 0x60) {
        r = 1;
    } else {
        r = env->f54810(cur, src, edi, op, end - cur, 0, &out);
    }
    if (r == 0) { *ebx = out + cur; return 0; }
    if (errOut != nullptr) *errOut = (uint32_t)r;
    return 0x14;
}
