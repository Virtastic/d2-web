// 1.13c D2Client+0xC0250 -- tear down the client game state when leaving a game. No arguments.
// gl block (pinned module globals): +0x000 [0x11991C] the slot-table index (low byte), +0x004 the 0xE2500 table (0x22-byte slots,
// the dword at +0 is a handle), +0x104 the range 0x122494.. (byte arrays at 0x122496 and 0x1224C0, 0x29 bytes each),
// +0x15C [0xFAD54] a handle, +0x160 [0x11B9DC] and [0x11B9E0] handles, +0x168 [0x11C288] (flag byte 0x11C28B in its top byte),
// +0x16C [0x11C3EC] the Always Run value, +0x170 [0x11B834], +0x174 the range 0x11B854.. (words at +0, +4, +0x10 = 0x11B854/58/864).
// Order: 0x74D80; the indexed slot handle h: 0xC18A(h), 0xBF86(ECX = h, EDX = &str, 0x7A, 0), slot := 0; 0x3D470; both byte arrays
// cleared; 0x46A20; 0x59740(0); 0x7BEB0(0x113AA4); 0x7BEB0(0x113AB0); the flag byte cleared; the three handles likewise
// (0xC18A, 0xBF86, cell := 0); 0xBEF6(&"Diablo II", &"Always Run", 0, [0x11C3EC]); [0x11B834] := 0, [0x11B858] := 0,
// [0x11B864] := 1, [0x11B854] := 5.
#include <cstdint>
#include "cl_env.h"
extern "C" void d2c_c0250(const cl::Env& e) {
    const uint32_t g = e.gl;
    auto wr8 = [&](uint32_t a, uint32_t v) { uint32_t b = a & ~3u, s = (a & 3) * 8; e.W(b, (e.R(b) & ~(0xffu << s)) | (v << s)); };
    auto freeHandle = [&](uint32_t cell) {
        uint32_t h = e.R(cell);
        if (!h) return;
        e.H(0xc18a, h);
        e.H(0xbf86, 0x7a, 0, 0, 0, 0, 0, h, 0xB2000000u | 0xd6ab0u);
        e.W(cell, 0);
    };
    e.H(0x74d80);
    freeHandle(g + 4 + e.R8(g) * 0x22);
    e.H(0x3d470);
    for (uint32_t i = 0; i < 0x29; i++) { wr8(g + 0x104 + 2 + i, 0); wr8(g + 0x104 + 0x2c + i, 0); }
    e.H(0x46a20);
    e.H(0x59740, 0);
    e.H(0x7beb0, 0xB2000000u | 0x113aa4u);
    e.H(0x7beb0, 0xB2000000u | 0x113ab0u);
    wr8(g + 0x168 + 3, 0);
    freeHandle(g + 0x15c);
    freeHandle(g + 0x160);
    freeHandle(g + 0x164);
    e.H(0xbef6, 0xB2000000u | 0xd3588u, 0xB2000000u | 0xd5144u, 0, e.R(g + 0x16c));
    e.W(g + 0x170, 0);
    e.W(g + 0x174 + 4, 0);
    e.W(g + 0x174 + 0x10, 1);
    e.W(g + 0x174, 5);
}
