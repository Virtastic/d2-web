// 1.13c BNClient+0xDB40 (ECX = an out flag): one step of the callee at 0xD610, which answers through a local dword.
// The local starts as the incoming ECX. Answer 0 or 1: returns 1. Answer 2: the out flag is set to 1 and 0 is
// returned. Any other answer returns 0.
#include "bnclient_send.h"

// 1.13c BNClient+0xDB40
extern "C" uint32_t bnclient_db40(const BnCallEnv& e, uint32_t* flag) {
    uint32_t state = bn_ptr(flag);
    bn_call(e, 0xd610, 0, 0, 0, 0, 0, bn_ptr(&state));
    if (state == 0 || state == 1) return 1;
    if (state == 2) *flag = 1;
    return 0;
}
