// 1.13c D2Client+0xA3370 over the FnCallees table. ECX = menu, ESI = idx; plain ret. menu+0x44 = idx (-1 clears); fatal 0x4F2 null
// menu, 0x4F8 idx >= count (+0x50), 0x51A item (+0x174 + i*0x110) disabled, 0x51E idx not found.
#include "d2client_batch_a3040.h"
namespace d2client_batch_a3040 {
void sub_A3370(FnCallees& s, uint32_t m, int32_t idx) {
 if (!m) s.fatal(0x4f2);
 const int32_t n = fnAt<int32_t>(fnPtr(m), 0x50);
 if (idx >= n) s.fatal(0x4f8);
 if (idx == -1) { fnAt<int32_t>(fnPtr(m), 0x44) = -1; return; }
 for (int32_t i = 0; i < n; ++i)
  if (i == idx) {
   if (fnAt<uint32_t>(fnPtr(m), 0x174 + (uint32_t)i * 0x110)) { fnAt<int32_t>(fnPtr(m), 0x44) = idx; return; }
   s.fatal(0x51a);
  }
 s.fatal(0x51e);
}
}
