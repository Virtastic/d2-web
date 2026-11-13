// 1.13c D2Client+0x64F50 -- key handler of the tabbed panel: steps the selected tab forwards (wrapping to the
// first), skipping unused records (first dword -1) and records whose availability callback (+0x110) answers 0,
// then plays the click sound and finishes the message.
#include "cl_pa4.h"
extern "C" uint32_t d2c_64f50(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    for (;;) {
        int32_t i = (int32_t)G(e, 0x11c058) + 1;
        const int32_t last = (int32_t)e.R(G(e, 0x11c05c)) - 1;
        S(e, 0x11c058, (uint32_t)i);
        if (i > last) {
            i = 0;
            S(e, 0x11c058, 0);
        }
        const uint32_t row = G(e, 0x11c060) + (uint32_t)i * 0x550;
        if (e.R(row) == 0xFFFFFFFFu) continue;
        const uint32_t fp = e.R(row + 0x110);
        if (!fp || slot(e, fp)) break;
    }
    e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    return fin(e, msg);
}
