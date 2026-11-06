// 1.13c D2Common+0x3bbd0
// 1.13c D2Common+0x3BBD0: stdcall (room), ret 4, void. A null room does nothing. With d = [room + 0x10], the callee at RVA 0x12DC0 (EAX = unit,
// plain ret) runs for each i in [0, [d + 0x2C]) on the pointer [[d + 8] + 4 * i]; the count is re-read after every call, as retail does.
#include <cstdint>
#include <cstring>
namespace {
inline uint32_t Rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void Wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
}

struct D2Common3BBD0Env { void (*animate)(uint32_t eax); };
extern "C" void D2Common_3BBD0_Raw(uint32_t room, const D2Common3BBD0Env* env)
{
    if (room == 0) return;
    const uint32_t d = Rd32(room + 0x10);
    for (int32_t i = 0; i < (int32_t)Rd32(d + 0x2C); ++i) env->animate(Rd32(Rd32(d + 8) + 4u * (uint32_t)i));
}
