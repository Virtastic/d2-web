// 1.13c D2Client+0x3F010 -- wheel message handler of the two-way control (ECX = control, stack word 0 = message): the
// message's signed word at +0xA picks the flag (+0x34 for positive, +0x38 for negative, one set and the other cleared)
// and raises the changed flag (+0x20); zero changes nothing. The message is then marked handled (+0x18 = 1, +0x1C = 0),
// finished through the shared message tail, and the handler answers 1.
#include "cl_pa4.h"
extern "C" uint32_t d2c_3f010(uint32_t self, uint32_t msg, const cl::Env& e) {
    const int32_t w = e.R16s(msg + 0xa);
    if (w > 0) {
        e.W(self + 0x34, 1);
        e.W(self + 0x38, 0);
        e.W(self + 0x20, 1);
    } else if (w < 0) {
        e.W(self + 0x34, 0);
        e.W(self + 0x38, 1);
        e.W(self + 0x20, 1);
    }
    pa4::fin(e, msg);
    return 1;
}
