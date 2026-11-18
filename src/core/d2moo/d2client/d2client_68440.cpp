// 1.13c D2Client+0x68440 (ECX = list block, one ignored stack word, ret 4): select the next row. Block words: +0x180730 first visible, +0x180738 selected,
// +0x180748 u16 count, +0x18074A flag (dword), +0x18074E visible rows. A zero flag first resets the selection to first + visible - 1, capped to count - 1 (signed
// compare). If selected < count - 1: select + 1, refresh (0x88A70: EBX = 1, four zero stack words), then keep the selection in view. Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_68440(uint32_t b, const cl::Env& e) {
    const uint32_t S = 0x180730;
    auto cnt = [&]() { return (int32_t)(e.R(b + S + 0x18) & 0xffff); };
    if (e.R(b + S + 0x1a) == 0) {
        int32_t sel = (int32_t)(e.R(b + S + 0x1e) + e.R(b + S) - 1);
        e.W(b + S + 8, (uint32_t)sel);
        if (sel >= cnt()) e.W(b + S + 8, (uint32_t)(cnt() - 1));
    }
    int32_t sel = (int32_t)e.R(b + S + 8);
    if (sel < cnt() - 1) {
        sel++;
        e.W(b + S + 8, (uint32_t)sel);
        e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 1);
        sel = (int32_t)e.R(b + S + 8);
        if (sel < (int32_t)e.R(b + S)) e.W(b + S, (uint32_t)sel);
        int32_t top = (int32_t)e.R(b + S);
        if ((int32_t)e.R(b + S + 8) >= (int32_t)e.R(b + S + 0x1e) + top) e.W(b + S, (uint32_t)(top + 1));
    }
    return 1;
}
