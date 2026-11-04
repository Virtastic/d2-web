// 1.13c D2Client+0x68CB0 (EAX = unit, DL = value): the unit's client block is unit+0x54 (null unit or null block: logged fatal exit 0xBBA, not returning).
// Store DL at block bytes 0,2,4..0x26 (twenty byte stores two bytes apart); the odd bytes are untouched.
#include "cl_env.h"
extern "C" void d2c_68cb0(uint32_t unit, uint32_t edx, const cl::Env& e) {
    uint32_t blk = unit ? e.R(unit + 0x54) : 0;
    if (!blk) {
        uint32_t m = e.H(0xbf92, 0xbba);
        e.H(0xbf80, 0xB20D359Fu, m);
        e.H(0x3664, 0xffffffffu);
        return;
    }
    for (uint32_t i = 0; i < 0x14; i++) e.W(blk + i * 2, (e.R(blk + i * 2) & 0xffffff00u) | (edx & 0xffu));
}
