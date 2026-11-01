// 1.13c D2Common+0x422f0
// fastcall(obj = ecx, x = edx), stack y, callee clean (ret 4). obj+0x14 = state S; S+4 = cells A, S+8 = row offsets A;
// S+0x2c = cells B, S+0x30 = row offsets B. Each cell write is a dword store of the value shown.
// A[rowA[y] + x] = 0; B[rowB[y] + x] = 0 (dword cells).
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t Rd32(uintptr_t p) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(p), 4); return v; }
inline void Wr32(uintptr_t p, uint32_t v) { std::memcpy(reinterpret_cast<void*>(p), &v, 4); }
}

extern "C" void D2Common_422F0(uint32_t pObj, int32_t x, int32_t y) {
    uintptr_t s = Rd32((uintptr_t)pObj + 0x14);
    uint32_t idxA = Rd32((uintptr_t)Rd32(s + 8) + (uint32_t)y * 4) + (uint32_t)x;
    Wr32((uintptr_t)Rd32(s + 4) + (uintptr_t)idxA * 4, 0);
    uint32_t idxB = Rd32((uintptr_t)Rd32(s + 0x30) + (uint32_t)y * 4) + (uint32_t)x;
    Wr32((uintptr_t)Rd32(s + 0x2c) + (uintptr_t)idxB * 4, 0);
}
