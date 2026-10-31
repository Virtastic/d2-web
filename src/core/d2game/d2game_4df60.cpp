// D2Game+0x4DF60 -- divide a value by a record's divisor.
// al = record id, esi = value. r = #a7ba(id); if r and its dword +0x1c are non-zero, value / r[+0x1c] (signed), else value.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game4DF60Env { uint32_t (*a7ba)(uint32_t id); };

// 1.13c D2Game+0x4DF60
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_4df60(uint8_t id, int32_t value, const D2Game4DF60Env* env)
{
    const uint32_t r = env->a7ba(id);
    if (r == 0) return value;
    int32_t d; std::memcpy(&d, (const void*)(uintptr_t)(r + 0x1c), 4);
    if (d == 0) return value;
    return value / d;
}
