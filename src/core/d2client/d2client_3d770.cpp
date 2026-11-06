// 1.13c D2Client+0x3D770 (EBX = key, stdcall p1 = x, p2, p3; ret 0xC): look the key up in the 10-byte-stride table at +0x11A2E0 (dword id at +0, word at +4
// must not be 0xFFFF, dword mode at +6 must be 1), fetch its label with 0x39F30 (eax = key, edx = 1), and when the label is wider than 0x28 (Ordinal_10028 via
// 0xD35A, ECX = string) cut it into the 16-char static buffer and zero characters from the end until it fits; then draw it with 0xD37E (ECX = string, EDX = x,
// pulled left by (width - 13) when wider than 13; stack p2, p3, 0). The pinned dword array e.gl: +0 the static-init flag (+0x123700), +4 the 32-byte buffer
// (+0x1236E0), +0x24 the table (0x480 bytes at +0x11A2E0). `nlen` is what the imported Unicode::strlen answers for the buffer (an import the capture stubs);
// Unicode::strncpy and the closure constructor leave memory unchanged under the stubs. The 16 buffer slots are never indexed past the table of 16 words.
#include "cl_env.h"
extern "C" void d2c_3d770(uint32_t ebx, uint32_t x, uint32_t p2, uint32_t p3, const cl::Env& e, uint32_t nlen) {
    const uint32_t tb = e.gl + 0x24, buf = e.gl + 4;
    uint32_t i = 0, a = 0x11a2e6;
    for (;;) {
        const uint32_t o = tb + i * 10;
        if (e.R(o) == ebx && e.R(o + 6) == 1) break;
        a += 10; ++i;
        if (a >= 0x11a75a) return;
    }
    if ((e.R(tb + i * 10 + 4) & 0xffff) == 0xffff) return;
    if (!(e.R(e.gl) & 1)) e.W(e.gl, e.R(e.gl) | 1);
    uint32_t str = e.H(0x39f30, 0, 0, 0, 0, 0, ebx, 0, 1);
    uint32_t w = e.H(0xd35a, 0, 0, 0, 0, 0, 0, str);
    if ((int32_t)w > 0x28) {
        str = 0xB2000000u | 0x1236e0;
        uint32_t s = buf + nlen * 2;
        do {
            s -= 2;
            const uint32_t al = s & ~3u, sh = (s & 3) * 8;
            e.W(al, e.R(al) & ~(0xffffu << sh));
            w = e.H(0xd35a, 0, 0, 0, 0, 0, 0, str);
        } while ((int32_t)w > 0x28);
    }
    if ((int32_t)w > 0xd) x += 0xd - w;
    e.H(0xd37e, p2, p3, 0, 0, 0, 0, str, x);
}
