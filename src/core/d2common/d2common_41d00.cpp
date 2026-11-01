// 1.13c D2Common+0x41d00
// Equality of two 16-byte name fields: both NUL-terminated names must be at most 8 characters, then dword 0 and dword 1 must match.
#include <cstdint>
#include <cstring>

namespace {
uint32_t Rd32(const char* p, uint32_t off) { uint32_t v; std::memcpy(&v, p + off, 4); return v; }
}

extern "C" bool D2Common_41D00(const char* pNameA, const char* pNameB) {
    // 1.13c: a name longer than 8 characters is a fatal exit (Ordinal_10024 + _exit), not reproduced: returns false.
    if (strnlen(pNameA, 9) > 8 || strnlen(pNameB, 9) > 8) return false;
    if (Rd32(pNameA, 0) != Rd32(pNameB, 0)) return false;
    return Rd32(pNameA, 4) == Rd32(pNameB, 4);
}
