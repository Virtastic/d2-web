// 1.13c D2Client+0x63620 and 1.13c D2Client+0x636A0: the two x87 lerp helpers (the CRT's __ftol at 0xCACC4 truncates the extended result to a
// 64-bit integer, returned in edx:eax; the FN vector records eax). Every fild is an unsigned conversion (a negative word gets 2^32 added).
// The retail registers are named as the disassembly uses them: 0x63620(eax = a, ecx = b, edx = obj), 0x636A0(eax, ecx, edx, [esp+4] = obj).
#include <cstdint>
namespace d2client_lerp {
static long double u(uint32_t v) { return (long double)v; }
static uint64_t trunc64(long double v) { return v <= 0.0L ? 0ull : (uint64_t)v; }
// 1.13c D2Client+0x63620: (a - b) / (n - 1) * m + b, n = obj[0x120], m = obj[0x124]
uint64_t lerp63620(uint32_t a, uint32_t b, uint32_t n, uint32_t m) {
    return trunc64(u(a - b) / u(n - 1u) * u(m) + u(b));
}
// 1.13c D2Client+0x636A0: (n - 1) * (eax - edx + 1) / (ecx - edx), n = obj[0x120]
uint64_t lerp636a0(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t n) {
    return trunc64(u(n - 1u) * u(eax - edx + 1u) / u(ecx - edx));
}
}
