// 1.13c D2Common+0x50020: stdcall(p), ret 4, retail-layout (32-bit) reader. Returns 1 when p is non-null, *(p) == 1, *(p + 0x10) == 0xC,
// 0x33C20(EAX = p, ECX = *(dt + 0x150)) is 0 (dt = sgptDataTables, retail 0x99E1C), and 0x1470(EAX = *(p + 4), EDX = 7) and
// 0x1470(EAX = *(p + 4), EDX = 0xC) are both non-zero; else 0. Callees are seams. (Retail also compares the type with itself a second
// time; that test never fails.)
#include <cstdint>
#include <cstring>

struct D2Common50020Env {
    int32_t (*flagTest)(uint32_t p, uint32_t ecx);      // 0x33C20
    uint32_t (*lookup)(uint32_t eax, uint32_t edx);     // 0x1470
};

namespace {
inline uint32_t Rd(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" uint32_t D2Common_50020_Raw(uint32_t p, const uint8_t* dt, const D2Common50020Env* env)
{
    if (p == 0 || Rd(p) != 1 || Rd(p + 0x10) != 0xC) return 0;
    uint32_t tbl; memcpy(&tbl, dt + 0x150, 4);
    if (env->flagTest(p, tbl) != 0) return 0;
    const uint32_t s = Rd(p + 4);
    if (env->lookup(s, 7) == 0) return 0;
    if (env->lookup(s, 0xC) == 0) return 0;
    return 1;
}
