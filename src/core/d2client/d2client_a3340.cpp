// 1.13c D2Client+0xA3340 over the FnCallees table. EAX = menu, [esp+4] = v (ret 4): menu+0x5C = v; a null menu is fatal (0x529).
#include "d2client_batch_a3040.h"
namespace d2client_batch_a3040 {
void sub_A3340(FnCallees& s, uint32_t m, uint32_t v) {
 if (!m) s.fatal(0x529);
 fnAt<uint32_t>(fnPtr(m), 0x5c) = v;
}
}
