// D2Client 0xAFD10..0xAFF80 batch: env-seam implementations from the decompile (0xB0000..0xB0DF0 live in their own d2client_b????.cpp files).
// Imported ordinals are Env function pointers (names are positional guesses); retail memory via rd32/wr32.
#include <cstdint>
#include "d2client_batch_afd10.h"
namespace d2client_batch_afd10 {
// 1.13c D2Client+0xAFD10 (ECX = a, EAX = b)
uint32_t sub_AFD10(const Env& e, uint32_t a, uint32_t b) { e.o10034(a, b); e.f3cb00(0xAFC40); return 1; }   // a = ECX, b = EAX; EBX = D2Client+0xAFC40 (a callback)
// 1.13c D2Client+0xAFF80  (in_EAX = v)
int32_t sub_AFF80(int32_t v) {
 if (v < 0x96) return 0x14; if (v < 200) return 0xf; if (v < 0xe1) return 10;
 return (((0x176 < v) - 1) & 0xffffffec) + 0x19;
}
} // namespace d2client_batch_afd10
