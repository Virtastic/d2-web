// 1.13c D2Client+0xA3220 over the FnCallees table. EDI = optional e {kind @0, .. @0xc}; plain ret.
// 0xC442(0, 6, 0x32, kind, e[3]) (5 words); a non-zero handle gets callback A3160 (0xC2BC) and 0xC36A(e, handle, 1).
#include "d2client_batch_a3040.h"
namespace d2client_batch_a3040 {
void sub_A3220(FnCallees& s, uint32_t e) {
 const uint32_t a = e ? fnAt<uint32_t>(fnPtr(e), 0) : 6, b = e ? fnAt<uint32_t>(fnPtr(e), 12) : 0xffffffffu;
 const uint32_t w[5] = {0, 6, 0x32, a, b};
 const uint32_t hd = s.h(0xc442, 0, 0, 0, 0, 0, 0, 6, 0x32, a);
 s.blk(0xc442, w, 5);
 if (hd) {
  s.h(0xc2bc, 0, 0, 0, 0, 0, hd, s.mod(0xa3160));
  s.h(0xc36a, 0, 0, 0, 0, 0, e, hd, 1);
 }
}
}
