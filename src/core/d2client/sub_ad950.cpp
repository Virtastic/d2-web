// 1.13c D2Client+0xAD950 -- env-seam style; callees are injected with the registers and stack words retail passes.
// The 12-byte local L (esp+4) is filled by 0xAC140 (esi = &L). Unaligned reads of it: u16 at +1, u32 at +3, +5 and u16 at +7.
#include <cstdint>
#include <cstring>
#include "sub_ad950.h"
namespace d2client_ad950 {
// 1.13c D2Client+0xAD950 (ecx = the value 0xAC140 takes in eax)
void sub_AD950(const Env& e, uint32_t ecx) {
 uint8_t L[12] = {0};
 e.fnAC140(ecx, L);
 if (!e.player) return;
 uint16_t w1; memcpy(&w1, L + 1, 2);
 e.fnC40C(e.player, 0xa, (uint32_t)w1 << 8, 0);
 uint32_t cx = L[7], ax = L[8], d5, d3;
 memcpy(&d5, L + 5, 4); memcpy(&d3, L + 3, 4);
 if ((int32_t)cx > 0x80) cx -= 0x100;
 if ((int32_t)ax > 0x80) ax -= 0x100;
 ax = (uint16_t)(ax + d5);
 cx = (uint16_t)(cx + d3);
 e.fn2A1B0(d5, 0, cx, ax);
}
}
