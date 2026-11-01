// 1.13c D2Common+0x44870
// Ordering of two list nodes: type at +4 first, then the sub-record key at +0x10 for that type.
#include <cstdint>
#include <cstring>

namespace {
int32_t Rd32(const uint8_t* p, uint32_t off) { int32_t v; std::memcpy(&v, p + off, 4); return v; }
const uint8_t* Ptr(const uint8_t* p, uint32_t off) {
    uint32_t v;
    std::memcpy(&v, p + off, 4);
    return reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(v));
}
}

extern "C" int32_t D2Common_44870(const uint8_t* pA, const uint8_t* pB) {
    const int32_t typeA = Rd32(pA, 4);
    const int32_t typeB = Rd32(pB, 4);
    if (typeA < typeB) return 1;
    if (typeA != typeB || typeA < 0 || typeA > 3) return 0;
    const uint8_t* sa = Ptr(pA, 0x10);
    const uint8_t* sb = Ptr(pB, 0x10);
    switch (typeA) {
    case 0: return Rd32(sa, 4) < Rd32(sb, 4);
    case 1: return Rd32(sb, 0) < Rd32(sa, 0);
    case 2: return Rd32(sb, 4) < Rd32(sa, 4);
    default: return Rd32(sa, 0) < Rd32(sb, 0);
    }
}
