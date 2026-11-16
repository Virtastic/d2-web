// 1.13c D2Game+0xB27D0: clear the second dword of a player's +0x34 record (caller 0xE0860).
// EAX = pUnit, plain ret, no result. A null unit, a non-player or a null record is retail's fatal assert (lines 0xC5 / 0xC9).
#include <cstdint>
struct GB27D0Env {
    uint32_t (*playerData)(uint32_t unit);   // D2Common thunk 0xA4C0 (stdcall, ret 4)
    void (*fatal)(uint32_t line);           // Fog assert + exit(-1); does not return in retail
};
static uint32_t rd32(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
// 1.13c D2Game+0xB27D0
void g_b27d0(uint32_t unit, const GB27D0Env& env)
{
    if (unit == 0 || rd32(unit) != 0) { env.fatal(0xC5); return; }
    const uint32_t q = rd32(env.playerData(unit) + 0x34);
    if (q == 0) { env.fatal(0xC9); return; }
    *(uint32_t*)(uintptr_t)(q + 4) = 0;
}
