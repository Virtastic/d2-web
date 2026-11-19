// 1.13c D2Common+0x3CDF0: stdcall(obj, levelId, mode, out1, out2, tag), ret 0x18. A null obj is fatal (string id 0x22C). 0x1A130 (EAX = &X, ECX = *(obj + 0x48), EDX = levelId,
// stack mode, &Y; ret 8) answers a handle and writes the anchor pair; 0 returns 0 with the outs untouched. Otherwise both coordinates become c*5 + 3 (Y feeds the in/out size slot,
// X the second local), 0x4D140 is called with nine stack words (handle, &size, 0, tag, 0x1C09, 0x1C09, 0, 0x32, 1) and may rewrite the size slot; a null answer is fatal (0x238).
// *out1 = size slot after the call, *out2 = X*5 + 3; the result is whatever 0x1330 returns for (EAX = handle, EBX = *out1). The three callees are seams.
#include <cstdint>
#include <cstring>

struct D2Common3CDF0Env {
    uint32_t (*anchor)(uint32_t drlg, uint32_t levelId, uint32_t mode, int32_t* x, int32_t* y);   // 0x1A130
    uint32_t (*alloc)(uint32_t handle, int32_t* size, uint32_t tag);                              // 0x4D140
    uint32_t (*finish)(uint32_t handle, uint32_t size);                                           // 0x1330
    void (*fatal)(uint32_t id);                                                                   // 0x921C + 0x9216 + exit
};

extern "C" uint32_t D2Common_3CDF0_Raw(uint32_t obj, uint32_t levelId, uint32_t mode, uint32_t out1, uint32_t out2, uint32_t tag, const D2Common3CDF0Env* env)
{
    if (obj == 0) env->fatal(0x22C);
    uint32_t drlg; memcpy(&drlg, reinterpret_cast<const void*>(static_cast<uintptr_t>(obj + 0x48)), 4);
    int32_t x = 0, y = 0;
    const uint32_t h = env->anchor(drlg, levelId, mode, &x, &y);
    if (h == 0) return 0;
    int32_t size = y * 5 + 3;
    const int32_t second = x * 5 + 3;
    if (env->alloc(h, &size, tag) == 0) env->fatal(0x238);
    memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(out1)), &size, 4);
    memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(out2)), &second, 4);
    return env->finish(h, static_cast<uint32_t>(size));
}
