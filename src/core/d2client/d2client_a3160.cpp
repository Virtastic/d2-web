// 1.13c D2Client+0xA3160 over the FnCallees table. ECX = unit (ret 4). 0xC310 selected skill, 0xC1FC (unit, 0x12), 0xC628 flags,
// 0xC42A Skills.cpp line 0x93C lookup; the skill row's u16 @+0x2e picks 0x2B4C0 / 0xC6450 (EDI = unit) or 0xC7290 (EAX = unit).
#include "d2client_batch_a3040.h"
namespace d2client_batch_a3040 {
void sub_A3160(FnCallees& s, uint32_t unit) {
 const uint32_t u = s.h(0xc310, 0, 0, 0, 0, 0, unit);
 if (!u) return;
 if (s.h(0xc1fc, 0, 0, 0, 0, 0, unit, 0x12)) return;
 if (s.h(0xc628, 0, 0, 0, 0, 0, u) & 2) return;
 const int32_t i = (int32_t)s.h(0xc42a, 0, 0, 0, 0, 0, u, s.mod(0xd44b4), 0x93c);
 if (i < 0) return;
 const uintptr_t dt = fnPtr(fnAt<uint32_t>(fnPtr(*s.g(0xce474)), 0));
 if (i >= fnAt<int32_t>(dt, 0xba0)) return;
 const uint32_t row = (uint32_t)i * 0x23c + fnAt<uint32_t>(dt, 0xb98);
 if (!row) return;
 switch (fnAt<int16_t>(fnPtr(row), 0x2e)) {
 case 0x43: s.h(0x2b4c0); s.edi(0x2b4c0, unit); break;
 case 0x4c: s.h(0xc6450); s.edi(0xc6450, unit); break;
 case 0x4d: case 0x4e: s.h(0xc7290, unit); break;
 }
}
}
