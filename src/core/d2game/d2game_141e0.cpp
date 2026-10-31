// 1.13c D2Game+0x141E0 -- item format dispatch: ITEMS_GetItemFormat(item) non-zero -> 0x13C20(item, arg),
// zero -> 0x13A30 (item in EDI). Answer is the callee's EAX. Callees are seams on the env.
#include <cstdint>
#include <emscripten.h>
struct D2Game141E0Env {
    uint16_t (*ordinal10718)(uint32_t item);
    uint32_t (*sub13C20)(uint32_t item, uint32_t arg);
    uint32_t (*sub13A30)();
};
extern "C" uint32_t d2_game_141E0(uint32_t item, uint32_t arg, const D2Game141E0Env* env)
{
    if (env->ordinal10718(item) >= 1) return env->sub13C20(item, arg);
    return env->sub13A30();
}
