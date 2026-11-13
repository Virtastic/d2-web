// 1.13c D2Client+0x65590 -- key handler of the tabbed panel: activates the selected tab and finishes the message.
#include "cl_pa4.h"
extern "C" uint32_t d2c_65590(uint32_t msg, const cl::Env& e) {
    e.H(0x65180);
    return pa4::fin(e, msg);
}
