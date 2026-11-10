// 1.13c D2Client+0xA32D0 over the FnCallees table. EAX = idx, EDI = menu; plain ret (tail-calls the font restore).
// 0xD3A8 (ECX = font) selects the item's font, 0xD35A (ECX = item text) measures it; item+0x160 = width, +0x15C = centring pad.
#include "d2client_batch_a3040.h"
namespace d2client_batch_a3040 {
void sub_A32D0(FnCallees& s, int32_t idx, uint32_t m) {
 const uint32_t prev = s.h(0xd3a8, 0, 1);
 const uint32_t it = m + (uint32_t)idx * 0x110;
 s.h(0xd3a8, 0, fnAt<uint32_t>(fnPtr(it), 0x168));
 const int32_t w = (int32_t)s.h(0xd35a, 0, it + 0x68);
 const int32_t mw = fnAt<int32_t>(fnPtr(m), 0x34);
 fnAt<int32_t>(fnPtr(it), 0x160) = w;
 fnAt<int32_t>(fnPtr(it), 0x15c) = w >= mw ? 0 : ((mw - w) + 1) / 2 + 1;
 s.h(0xd3a8, 0, prev);
}
}
