// 1.13c D2Client+0x684E0 (ECX = list block, one ignored stack word, ret 4): select the previous row. A zero flag (+0x18074A) first sets the selection
// (+0x180738) to the first visible row (+0x180730). If selected > 0 (signed): select - 1, refresh (0x88A70: EBX = 1, four zero stack words), pull the
// first visible row down to it, and scroll one row when it fell below the window (+0x18074E rows). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_684e0(uint32_t b, const cl::Env& e) {
    const uint32_t S = 0x180730;
    if (e.R(b + S + 0x1a) == 0) e.W(b + S + 8, e.R(b + S));
    int32_t sel = (int32_t)e.R(b + S + 8);
    if (sel > 0) {
        sel--;
        e.W(b + S + 8, (uint32_t)sel);
        e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 1);
        sel = (int32_t)e.R(b + S + 8);
        if (sel < (int32_t)e.R(b + S)) e.W(b + S, (uint32_t)sel);
        int32_t top = (int32_t)e.R(b + S);
        if (sel >= (int32_t)e.R(b + S + 0x1e) + top) e.W(b + S, (uint32_t)(top + 1));
    }
    return 1;
}
