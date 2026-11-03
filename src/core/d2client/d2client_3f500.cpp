// 1.13c D2Client+0x3F500: constructor (stdcall, 5 words: this, a1..a4). this[0] = base vtable (D2Client+0xD5048) then derived (+0xD675C);
// this[2] = the child built by 0x3F3D0(Storm_401(0x54, source, -1, 0), a1..a4), or 0 when the allocation returns 0. Returns this.
#include "d2c_seam.h"
extern "C" uint32_t d2c_3f500(const D2cSeam* s, uint32_t* o, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4) {
    o[0] = 0x6FB85048u;
    o[0] = 0x6FB8675Cu;
    const uint32_t m = (uint32_t)(*s)(0xbe66, 0, 0, 0, 0, 0, 0x54, s->mod(0xD04F0), 0xffffffffu, 0);
    o[2] = m ? (uint32_t)(*s)(0x3f3d0, 0, 0, 0, 0, 0, m, a1, a2, a3, a4) : 0;
    return (uint32_t)(uintptr_t)o;
}
