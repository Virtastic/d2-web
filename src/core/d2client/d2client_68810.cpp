// 1.13c D2Client+0x68810 (stdcall, one argument = cache block): walk the 0x200 bucket heads at block+0xC; each chain entry has next at +0, flags at +4
// (only bit 0 of the low byte is tested), size at +8, payload at +0x14. A flagged entry takes (-0x1C - size) off the running total (module global at
// +0x11C044, g) and frees the payload (Fog free line 0xE3C) then the entry (line 0xE3D); the next link is read before the frees.
#include "cl_env.h"
extern "C" void d2c_68810(uint32_t blk, uint32_t g, const cl::Env& e) {
    const uint32_t kFile = 0xB20D4FA4u;
    for (uint32_t i = 0; i < 0x200; i++) {
        uint32_t n = e.R(blk + 0xc + i * 4);
        while (n) {
            uint32_t nxt = e.R(n);
            if (e.R(n + 4) & 1) {
                e.W(g, e.R(g) + (0xffffffe4u - e.R(n + 8)));
                e.H(0xbf86, 0xe3c, 0, 0, 0, 0, 0, e.R(n + 0x14), kFile);
                e.H(0xbf86, 0xe3d, 0, 0, 0, 0, 0, n, kFile);
            }
            n = nxt;
        }
    }
}
