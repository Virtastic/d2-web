// 1.13c D2Game+0x237C0 -- flag a type-1 unit and notify (ECX = unit, EDX = passed through, [esp+4] = param, ret 4).
// Needs a type-1 unit and a non-zero param: the unit's +0x14 record gets byte +0x16 |= 0x20, 0x21EC0(EBX = unit,
// EDI = 0x64) runs, then 0x22DA0(ECX = unit, EDX = edx, param). EDI is not observable in the recorder.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
struct D2Game237C0Env {
    void (*sub21EC0)(uint32_t unit, uint32_t pct);
    void (*sub22DA0)(uint32_t unit, uint32_t edx, uint32_t param);
};
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_237C0(uint32_t unit, uint32_t edx, uint32_t param, const D2Game237C0Env* e)
{
    uint32_t type = 0; if (unit) std::memcpy(&type, (const uint8_t*)(uintptr_t)unit, 4);
    if (unit == 0 || type != 1 || param == 0) return;
    uint32_t pd; std::memcpy(&pd, (const uint8_t*)(uintptr_t)unit + 0x14, 4);
    if (pd) *((uint8_t*)(uintptr_t)pd + 0x16) |= 0x20;
    e->sub21EC0(unit, 0x64);
    e->sub22DA0(unit, edx, param);
}
