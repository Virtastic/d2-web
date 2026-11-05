// Close the open book/teleport panel by its object class: 'bks ' closes plain, 'bkd ' closes plain or (when flagged) with the
// teleport close, 'tr1 ' takes the teleport close; any other class does nothing.
#include "cl_env.h"
// 1.13c D2Client+0x7DE00.
extern "C" void d2cl_7de00(const cl::Env& e) {
    uint32_t p = e.R(0x6FAB0000u + 0x113ae8);
    if (!p) return;
    uint32_t tag = e.R(p + 0x80);
    if (tag == 0x20736b62u) { e.H(0x7cc60, 0); return; }
    if (tag == 0x20646b62u) {
        if (!e.R(0x6FAB0000u + 0x113b14)) { e.H(0x7cc60, 1); return; }
    } else if (tag != 0x20317274u) return;
    e.H(0x7c380);
}
