// 1.13c D2Client+0x711D0 -- message handler: raises the second request word, clears the hover word, drops the four
// button latches and finishes the message.
#include "cl_pa4.h"
extern "C" uint32_t d2c_711d0(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    S(e, 0x11c018, 1);
    S(e, 0x11c00c, 0);
    clearButtons(e);
    return fin(e, msg);
}
