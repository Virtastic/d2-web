// 1.13c D2Client+0x65640 -- button-down handler of the tabbed panel: the tab under the cursor (hit test with
// EBX = y) becomes the selected one, the press routine runs with (x, ECX = y) and the press-pending word is raised.
#include "cl_pa4.h"
extern "C" uint32_t d2c_65640(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    const uint32_t y = P16(e, msg + 0xe), x = P16(e, msg + 0xc);
    const uint32_t r = e.H(0x64080, 0, 0, 0, 0, 0, 0, 0, 0, y, msg);
    if (r != 0xFFFFFFFFu) S(e, 0x11c058, r);
    e.H(0x64fd0, x, 0, 0, 0, 0, 0, y);
    S(e, 0x11c068, 1);
    return fin(e, msg);
}
