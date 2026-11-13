// 1.13c D2Client+0x65E00 -- close handler: EDX points at the window handle; clear the flag at 0xF8C9C, send the window message 0x10 (close), raise 0x11C404, refresh with 1.
#include "cl_cmd.h"
extern "C" uint32_t d2c_65e00(uint32_t edx, const cl::Env& e) {
    const uint32_t hwnd = e.R(edx);
    clc::S(e, 0xF8C9C, 0);
    e.H(0xCF118, hwnd, 0x10, 0, 0);
    clc::S(e, 0x11C404, 1);
    return e.H(0x656b0, 1);
}
