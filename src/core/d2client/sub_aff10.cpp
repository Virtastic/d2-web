// D2Client sub_AFF10: build a 0x5C-byte request block and hand it to sub_B30F0.
#include <cstdint>
namespace d2client_aff10 {
using P = uintptr_t;
struct Env { P (*f630f0)(const uint32_t*); };  // D2Client+0xB30F0
// 1.13c D2Client+0xAFF10
void sub_AFF10(const Env& e, uint32_t a1, P a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7) {
 (void)a1;
 if (!a2) return;
 uint32_t b[0x17] = {};
 b[0] = 1; b[1] = (uint32_t)a2; b[4] = a3; b[5] = a4; b[6] = a5; b[11] = a6; b[12] = a7;
 e.f630f0(b);
}
}
