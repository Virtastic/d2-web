// D2Common 1.13c batch: seeded range roll. p (retail ECX/EAX/EDX args) = a, b, ctx; ctx[0]==4, ctx[5] = seed holder.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x464D0: random int in [min(a,b), max(a,b)] (+1 span if holder short @+0x30 != 0).
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_464d0(int32_t a, int32_t b, const int32_t* ctx) {
 if (a == b) return b;
 int32_t lo = a, hi = b;
 if (a > b) { lo = b; hi = a; }
 if (!ctx || ctx[0] != 4 || !ctx[5]) __builtin_trap(); // retail: Ordinal_10024 error + _exit(-1)
 uint8_t* h = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(static_cast<uint32_t>(ctx[5])));
 uint32_t span = static_cast<uint32_t>(hi - lo);
 if (*reinterpret_cast<int16_t*>(h + 0x30)) ++span;
 if (static_cast<int32_t>(span) <= 0) return lo;
 uint64_t* s = reinterpret_cast<uint64_t*>(h + 4);
 *s = static_cast<uint64_t>(static_cast<uint32_t>(*s)) * 0x6ac690c5ull + static_cast<uint32_t>(*s >> 32);
 const uint32_t r = static_cast<uint32_t>(*s);
 return static_cast<int32_t>((span & (span - 1)) == 0 ? ((span - 1) & r) + lo : r % span + lo);
}
}
