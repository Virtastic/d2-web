// 1.13c D2Client+0xA3040 over the FnCallees table (the form the FN probe and d2client_batch_a3040.h declare).
// EAX = optional {kind @0, .. @0xc}; [esp+4] = p1 (ret 4); 0x2A750(ECX 0x16, EDX p1, {0,-1,kind,e[3],1,0,0}, 1); returns 1.
#include "d2client_batch_a3040.h"
namespace d2client_batch_a3040 {
uint32_t sub_A3040(FnCallees& s, uint32_t e, uint32_t p1) {
 int32_t r[7] = {0, -1, 6, -1, 1, 0, 0};
 if (e) { r[2] = fnAt<int32_t>(fnPtr(e), 0); r[3] = fnAt<int32_t>(fnPtr(e), 12); }
 s.h(0x2a750, 0, 0x16, p1, 0, 0, (uint32_t)(uintptr_t)r, 1);
 s.blk(0x2a750, r, 7);
 return 1;
}
}
