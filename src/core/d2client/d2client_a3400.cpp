// 1.13c D2Client+0xA3400 over the FnCallees table. ESI = menu, stack word 0 = value (ret 4); pushes an 8-byte node {value, next}
// on the menu's list at +0xB08. Fatal 0x358 on a null menu; the node comes from 0xBF7A (ECX = 8, EDX = file name 0xD41D4, 0x35C, 0).
#include "d2client_batch_a3040.h"
namespace d2client_batch_a3040 {
void sub_A3400(FnCallees& s, uint32_t m, uint32_t v) {
 if (!m) s.fatal(0x358);
 const uint32_t old = fnAt<uint32_t>(fnPtr(m), 0xb08);
 const uint32_t n = s.h(0xbf7a, 0, 8, s.mod(0xd41d4), 0, 0, 0x35c, 0);
 fnAt<uint32_t>(fnPtr(m), 0xb08) = n;
 fnAt<uint32_t>(fnPtr(n), 4) = old;
 fnAt<uint32_t>(fnPtr(fnAt<uint32_t>(fnPtr(m), 0xb08)), 0) = v;
}
}
