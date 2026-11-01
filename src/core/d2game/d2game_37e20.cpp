// D2Game unit timer arming in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
#include <cstdint>
#include <cstring>
#include <unistd.h>

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

struct D2Game37E20Env {
    uint32_t (*ordinal11017)(uint32_t v);                                          // D2Common thunk 0xA6A6, stdcall 1 arg
    // the .rdata function pointer at D2Game+0xFA400 (0x782F0, checked with IsBadCodePtr): ECX = unit, EDX = second; stack (1, a1, a2)
    void (*hook)(uint32_t unit, uint32_t second, uint32_t one, uint32_t a1, uint32_t a2);
    // 0x8EA80: ECX = unit, EDX = second; stack (kind, frame, 0, 0, 0), ret 0x14
    void (*addTimer)(uint32_t unit, uint32_t second, uint32_t kind, uint32_t frame, uint32_t z0, uint32_t z1, uint32_t z2);
};

// 1.13c D2Game+0x37E20
// ESI = unit, EDI = second struct; stack a1, a2 (ret 8). A null unit exits through Fog with 0x804. When
// the dword at EDI+0x60 is non-zero and #11017 of it is non-zero the hook is skipped; otherwise the
// module function pointer must pass IsBadCodePtr (else Fog 0x811) and is called with (1, a1, a2). Then
// two timers are added: kind 3 at unit frame (+0xA8) + 1, and kind 0xB at frame + 0xFA.
void SUNIT_ArmTimers_37E20(uint32_t unit, uint32_t second, uint32_t a1, uint32_t a2, const D2Game37E20Env& env)
{
    if (unit == 0) _exit(-1);   // Fog fatal 0x804
    const uint32_t v = rd32(second + 0x60);
    if (v == 0 || env.ordinal11017(v) == 0) {
        if (env.hook == nullptr) _exit(-1);   // Fog fatal 0x811
        env.hook(unit, second, 1, a1, a2);
    }
    env.addTimer(unit, second, 3, rd32(unit + 0xA8) + 1, 0, 0, 0);
    env.addTimer(unit, second, 0xB, rd32(unit + 0xA8) + 0xFA, 0, 0, 0);
}
