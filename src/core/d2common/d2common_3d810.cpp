// 1.13c D2Common+0x3D810: EAX = table {+0 dword array A, +4 dword array B}, ECX = c, EDX = d, ESI = out (9 dwords). Gathers a 3x3 window:
// with r(k) = B[c + k] + d (B indexed from 0), out[0..8] = A[r(1)-1], A[r(0)-1], A[r(-1)-1], A[r(1)], A[r(0)], A[r(-1)], A[r(1)+1], A[r(0)+1], A[r(-1)+1],
// where out[j] sits at +4*j. EAX is left holding the last gathered value (the ninth store); the function has no result.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t Rd(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" void D2Common_3D810_Raw(uint32_t tbl, int32_t c, int32_t d, uint32_t out)
{
    const uint32_t A = Rd(tbl), B = Rd(tbl + 4);
    auto at = [&](int32_t k, int32_t o) { return Rd(A + 4u * (Rd(B + 4u * static_cast<uint32_t>(c + k)) + static_cast<uint32_t>(d + o))); };
    // k = -1, 0, +1 index B[c-1], B[c], B[c+1]; o = -1, 0, +1 offset the A index (A is read at ebx + idx*4 + 4*o, with idx already including d)
    uint32_t v[9];
    v[2] = at(-1, -1) ; v[1] = at(0, -1); v[0] = at(1, -1);
    v[5] = at(-1, 0);   v[4] = at(0, 0);  v[3] = at(1, 0);
    v[8] = at(-1, 1);   v[7] = at(0, 1);  v[6] = at(1, 1);
    memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(out)), v, sizeof v);
}
