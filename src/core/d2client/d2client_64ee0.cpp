// 1.13c D2Client+0x64EE0 -- key handler of the tabbed panel: steps the selected tab backwards (wrapping to the
// last), skipping unused records (first dword -1) and records whose availability callback (+0x110) answers 0,
// then plays the click sound and finishes the message.
#include "cl_pa4.h"
extern "C" uint32_t d2c_64ee0(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    for (;;) {
        int32_t i = (int32_t)G(e, 0x11c058) - 1;
        S(e, 0x11c058, (uint32_t)i);
        if (i < 0) {
            i = (int32_t)e.R(G(e, 0x11c05c)) - 1;
            S(e, 0x11c058, (uint32_t)i);
        }
        const uint32_t row = G(e, 0x11c060) + (uint32_t)i * 0x550;
        if (e.R(row) == 0xFFFFFFFFu) continue;
        const uint32_t fp = e.R(row + 0x110);
        // retail: `call eax` with EAX = the callback, ECX = the tab index, EDX = index * 0x550
        if (!fp || slot(e, fp, fp, (uint32_t)i, (uint32_t)i * 0x550u)) break;
    }
    e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    return fin(e, msg);
}
