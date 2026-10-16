// 1.13c Storm+0x1350, Storm+0x83B0, Storm+0x3EFE6, Storm+0x3EFCB, D2Gfx+0x58F0: MSVC CRT leaves + one Storm offset.
#include <emscripten.h>
#include <cstdint>
// 1.13c Storm+0x1350: eax=n, ecx=base -> base + (n*3+6)*4
uint32_t Storm_Sub1350(uint32_t n, uint32_t base) { return base + (n * 3 + 6) * 4; }
// 1.13c Storm+0x83B0 / D2Gfx+0x58F0: strcpy; returns dst. Byte-for-byte equal to the DLL's word-at-a-time copy.
uint8_t* Crt_Strcpy(uint8_t* d, const uint8_t* s) { uint8_t* o = d; while ((*d++ = *s++) != 0) {} return o; }
// 1.13c Storm+0x3EFE6 __IsZeroMan: three dwords all zero
uint32_t Storm_IsZeroMan(const uint32_t* p) { return p[0] == 0 && p[1] == 0 && p[2] == 0; }
// 1.13c Storm+0x3EFCB __CopyMan: 3 dwords src -> dst
void Storm_CopyMan(uint32_t* d, const uint32_t* s) { d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; }
#define P(T, i) ((T)(uintptr_t)r[i])
extern "C" {
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_1350(uint32_t* r) { r[4] = Storm_Sub1350(r[0], r[1]); }
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_83b0(uint32_t* r) { r[4] = (uint32_t)(uintptr_t)Crt_Strcpy(P(uint8_t*, 0), P(const uint8_t*, 1)); }
EMSCRIPTEN_KEEPALIVE void d2_fn_d2gfx_58f0(uint32_t* r) { r[4] = (uint32_t)(uintptr_t)Crt_Strcpy(P(uint8_t*, 0), P(const uint8_t*, 1)); }
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_3efe6(uint32_t* r) { r[4] = Storm_IsZeroMan(P(const uint32_t*, 0)); }
EMSCRIPTEN_KEEPALIVE void d2_fn_storm_3efcb(uint32_t* r) { Storm_CopyMan(P(uint32_t*, 0), P(const uint32_t*, 1)); r[4] = 0; }
}
