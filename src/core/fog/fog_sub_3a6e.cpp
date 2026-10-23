// 1.13c Fog+0x3A6E: CRT _lock(0xD) thunk (push 0xD; call Fog+0x3C3F; pop ecx; ret).
#include <cstdint>

struct FogSub3a6eEnv {
    void (*lock)(int32_t n);  // Fog+0x3C3F: EnterCriticalSection(table[n])
};

extern "C" void fog_sub_3a6e(const FogSub3a6eEnv* e) { e->lock(0xD); }
