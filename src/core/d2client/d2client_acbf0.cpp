// 1.13c D2Client+0xACBF0 packetTypeDispatch, written from the disassembly: EAX = packet; the dword at +1 minus 1 indexes a 26-entry jump table
// (default for anything above 0x19, which includes 0); every target loads a constant into EAX and tail-jumps to 0x43550 (EAX = value).
// The decompile's switch hides the constants: case 1..6 give 0..5, 7..21 give 0xA..0x18, 22 gives 9, 23 gives 0x19, 24 gives 0x1A, 25 gives 0x1B, 26 gives 0x1C.
#include "cl_env.h"
// 1.13c D2Client+0xACBF0
extern "C" uint32_t d2c_acbf0(uint32_t pkt, const cl::Env& e) {
    const uint32_t k = e.R(pkt + 1) - 1;
    uint32_t v;
    if (k > 0x19) v = 9;
    else if (k < 6) v = k;
    else if (k < 21) v = k + 4;
    else {
        static const uint8_t t[5] = { 9, 0x19, 0x1a, 0x1b, 0x1c };
        v = t[k - 21];
    }
    return e.H(0x43550, 0, 0, 0, 0, 0, v);
}
