// D2Common 1.13c batch. Seeded random in [lo,hi] from a kind-4 holder (+0x14 -> state; seed u64 at state+4).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x463F0: a=EAX, b=ECX, holder=EDX. Retail error paths (_exit(-1)) kept as traps.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_463f0(int32_t a, int32_t b, const int32_t* holder) {
 if (a == b) return b;
 int32_t lo = b, hi = a;
 if (a < b) { hi = b; lo = a; }
 if (!holder || holder[0] != 4) __builtin_trap();
 uint8_t* st = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(static_cast<uint32_t>(holder[5])));
 if (!st) __builtin_trap();
 uint32_t n = static_cast<uint32_t>(hi - lo);
 if (*reinterpret_cast<int16_t*>(st + 0x30)) ++n;
 uint64_t* s = reinterpret_cast<uint64_t*>(st + 4);
 if (static_cast<int32_t>(n) <= 0) return lo;
 *s = static_cast<uint64_t>(static_cast<uint32_t>(*s)) * 0x6ac690c5ull + static_cast<uint32_t>(*s >> 32);
 const uint32_t r = static_cast<uint32_t>(*s);
 return static_cast<int32_t>((n & (n - 1)) == 0 ? ((n - 1) & r) : r % n) + lo;
}
}
