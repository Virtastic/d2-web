// 1.13c D2Common+0x38dc0
// 1.13c D2Common+0x38DC0: forwarder, retail layout. stdcall (a0, a1, a2), a0 unused; calls the callee at RVA 0x38B70 with stack (a1, a2, 0), ret 0xC,
// and returns its result.
#include <cstdint>
struct D2Common38DC0Env { uint32_t (*callee)(uint32_t w0, uint32_t w1, uint32_t w2); };
extern "C" uint32_t D2Common_38DC0_Raw(uint32_t a0, uint32_t a1, uint32_t a2, const D2Common38DC0Env* env)
{
    (void)a0;
    return env->callee(a1, a2, 0);
}
