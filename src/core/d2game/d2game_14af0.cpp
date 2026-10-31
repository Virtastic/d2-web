// 1.13c D2Game+0x14AF0 -- same dispatch for the second layout: non-zero format -> 0x13E60 (item in EDI,
// arg on the stack), zero -> 0x137F0 (item in EDI). Answer is the callee's EAX.
#include <cstdint>
#include <emscripten.h>
struct D2Game14AF0Env {
    uint16_t (*ordinal10718)(uint32_t item);
    uint32_t (*sub13E60)(uint32_t arg);
    uint32_t (*sub137F0)();
};
extern "C" uint32_t d2_game_14AF0(uint32_t item, uint32_t arg, const D2Game14AF0Env* env)
{
    if (env->ordinal10718(item) >= 1) return env->sub13E60(arg);
    return env->sub137F0();
}
