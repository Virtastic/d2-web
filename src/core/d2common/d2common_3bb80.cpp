// 1.13c D2Common+0x3bb80
// 1.13c D2Common+0x3BB80: stdcall (room, value), ret 8, void. A null room is a fatal error in retail (error report, then exit); `fatal` stands for that
// path. Otherwise every node of the list at [[room + 0x10] + 0x4C] (next at +4) gets +8 := value.
#include <cstdint>
#include <cstring>
namespace {
inline uint32_t Rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void Wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
}

struct D2Common3BB80Env { void (*fatal)(); };
extern "C" void D2Common_3BB80_Raw(uint32_t room, uint32_t value, const D2Common3BB80Env* env)
{
    if (room == 0) { env->fatal(); return; }
    for (uint32_t n = Rd32(Rd32(room + 0x10) + 0x4C); n != 0; n = Rd32(n + 4)) Wr32(n + 8, value);
}
