// 1.13c D2Client+0x38820: text-slider control constructor, stdcall (this, a1..a8), ret 0x24, returns this. a6 = entry count (u16),
// a3 = pixel width (<= 0: derived from the label width), a4 = flag. The 11 wide-char label constructions and the win2Unicode call write
// only locals that the width query 0xD35A reads; they are not seam calls (their import slots are not hookable).
#include "d2c_seam.h"
template <class T> static void wr(uint8_t* p, unsigned o, T v) { std::memcpy(p + o, &v, sizeof v); }
extern "C" uint32_t d2c_38820(const D2cSeam* s, uint8_t* o, uint32_t a1, uint32_t a2, int32_t a3, int32_t a4, uint32_t a5, uint16_t a6, uint32_t a7, uint32_t a8) {
    uint8_t buf[260];
    uint16_t wb[11] = {};
    wr<uint32_t>(o, 0, (uint32_t)s->mod(0xD5048));
    wr<uint32_t>(o, 0, (uint32_t)s->mod(0xD68C0));
    (*s)(0x3146, 0, 0, 0, 0, 0, (uintptr_t)buf, s->mod(0xD5078), s->mod(0xD357C));
    wr<uint32_t>(o, 0x52, (uint32_t)(*s)(0x2b420, (uintptr_t)buf, 0, 0, 0, 0, 0));
    wr<uint32_t>(o, 0x42, a7);
    const uint32_t bytes = (uint32_t)a6 * 2 + 2;
    wr<uint32_t>(o, 8, a1); wr<uint32_t>(o, 0xC, a2); wr<uint32_t>(o, 0x3E, 0); wr<uint32_t>(o, 0x46, 0);
    uint8_t* A = (uint8_t*)(uintptr_t)(uint32_t)(*s)(0xbf7a, 0, bytes, s->mod(0xD6918), 0, 0, 0x5d, 0);
    wr<uint32_t>(o, 0x28, (uint32_t)(uintptr_t)A);
    uint8_t* B = (uint8_t*)(uintptr_t)(uint32_t)(*s)(0xbf7a, 0, bytes, s->mod(0xD6918), 0, 0, 0x5e, 0);
    wr<uint32_t>(o, 0x2C, (uint32_t)(uintptr_t)B);
    std::memset(A, 0, bytes); std::memset(B, 0, bytes);
    wr<uint32_t>(o, 0x30, a5); wr<uint16_t>(o, 0x34, a6); wr<uint32_t>(o, 0x10, a1 + 1);
    wr<uint32_t>(o, 0x36, 0); wr<uint32_t>(o, 0x3A, 0); wr<uint32_t>(o, 0x4E, a8); wr<uint32_t>(o, 0x14, a2);
    wr<uint32_t>(o, 0x20, 1); wr<uint32_t>(o, 0x4A, 1);
    (*s)(0xd3a8, 0, a8);
    int32_t w = a3;
    if (a3 <= 0) {
        const int32_t r = (int32_t)(*s)(0xd35a, 0, (uintptr_t)wb);
        int32_t t = (int32_t)(((int64_t)r * 0x66666667LL) >> 32) >> 2;
        t += (int32_t)((uint32_t)t >> 31);
        w = (int32_t)((uint32_t)t * ((uint32_t)a6 + 2));
    }
    wr<int32_t>(o, 0x18, w);
    int32_t f = a4;
    if (a4 <= 0) { f = 1; wr<uint32_t>(o, 0x56, 1); } else wr<uint32_t>(o, 0x56, 0);
    wr<int32_t>(o, 0x1C, f);
    wr<uint32_t>(o, 0x24, 0); wr<uint32_t>(o, 4, 0);
    return (uint32_t)(uintptr_t)o;
}
