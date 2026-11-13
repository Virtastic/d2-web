// 1.13c D2Client+0x42F00 -- button-up handler of the centre-bottom button: inside the box it raises the release
// flag; the four button latches are dropped and the message finished either way.
#include "cl_pa4.h"
namespace pa4 { bool inCentreButton(const cl::Env& e); }
extern "C" uint32_t d2c_42f00(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    if (inCentreButton(e)) S(e, 0x11c410, 1);
    clearButtons(e);
    return fin(e, msg);
}
