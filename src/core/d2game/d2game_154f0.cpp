// 1.13c D2Game+0x154F0 -- ordinal 11036(item) must be non-zero; then ITEMS_GetItemFormat picks 0x15240
// (non-zero) or 0x14F20 (zero). Both take the item in ESI and EDI as the one stack word (ret 4).
#include <cstdint>
#include <emscripten.h>
struct D2Game154F0Env {
    uint32_t (*ordinal11036)(uint32_t item);
    uint16_t (*ordinal10718)(uint32_t item);
    void (*sub15240)(uint32_t item, uint32_t edi);
    void (*sub14F20)(uint32_t item, uint32_t edi);
};
extern "C" void d2_game_154F0(uint32_t item, uint32_t edi, const D2Game154F0Env* env)
{
    if (env->ordinal11036(item) == 0) return;
    if (env->ordinal10718(item) >= 1) env->sub15240(item, edi);
    else env->sub14F20(item, edi);
}
