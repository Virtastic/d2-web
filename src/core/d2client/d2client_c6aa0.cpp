// 1.13c D2Client+0xC6AA0, written from the disassembly. EAX = unit, stack (s0, s1), ret 8; returns 0 (blocked) or 1.
// A = 0xCB68(s0), B = 0xCB6E(s0). k = the unit's [[unit+0x54]+0x30] block when all three are set (k word +0xC >> 11, else 0). isP = unit && [unit] == 1.
// For a player the class [unit+4] is mapped through the 0x1A8-stride table at dt+0xA78 (count dt+0xA80) to its word +2; a class outside the count is -1.
// k != 0: the unit's box (type 2, or 4..5: [path+0xC] / [path+0x10]; else 0xC3BE / 0xC2EC of the path, 0 without one) must equal (A, B). If it does not:
// a player adds 8 when the class is 0x4E, or 0xC / 0xA from 0xC4D8(unit, A, B) < 2 when 0x21C; anyone else 0xA; 0xCB56(unit, that),
// [unit+0x48] = ([unit+0x48] & ~0xFF) + 0x100, return 0. Otherwise (or k == 0) 0xC6060(ESI = unit, EBX = A; B) runs, the k block's words 4, 8, 0xC clear,
// s1 != 0 calls 0x6B300(EAX = 0x50; unit, 2, 0 x5), and 0x22FB0(EAX = unit; 1) feeds 0x88A70(EBX = it; unit, 0, 0, 0): a player with class 0x4E then
// 0xC6150(EAX = s0; unit, A, B), anyone else a second 0x88A70(EBX = 0x9D5; unit, 0, 0, 0) (a non-player takes the first one with the 22FB0 result too).
#include "cl_mb2.h"
// 1.13c D2Client+0xC6AA0
extern "C" uint32_t d2c_c6aa0(uint32_t u, uint32_t s0, uint32_t s1, const clm::Env& e) {
    using clm::Env;
    const uint32_t A = e.H(0xcb68, 0, 0, 0, 0, 0, s0);
    const uint32_t B = e.H(0xcb6e, 0, 0, 0, 0, 0, s0);
    uint32_t k = 0;
    if (u) {
        const uint32_t b = e.R(u + 0x54);
        if (b) {
            const uint32_t q = e.R(b + 0x30);
            if (q) k = (uint32_t)((int32_t)e.R(q + 0xc) >> 11);
        }
    }
    const uint32_t type = u ? e.R(u) : 6u;
    const bool isP = type == 1;
    uint32_t cls = 0xffffffffu;
    if (isP) {
        int32_t c = (int32_t)e.R(u + 4);
        if (c >= 0) {
            uint32_t dt = e.dt;
            if (c < (int32_t)e.R(dt + 0xa80)) {
                const uint32_t row = (uint32_t)c * 0x1a8u + e.R(dt + 0xa78);
                if (row) c = (int16_t)(e.R(row) >> 16);
            }
            if (c >= 0) {
                dt = e.dt;
                if (c < (int32_t)e.R(dt + 0xa80)) cls = (uint32_t)c;
            }
        }
    }
    if (k) {
        const int32_t t = (int32_t)e.R(u);
        const bool box = t == 2 || (t > 3 && t <= 5);
        const uint32_t path = e.R(u + 0x2c);
        uint32_t w = box ? e.R(path + 0xc) : (path ? e.H(0xc3be, 0, 0, 0, 0, 0, path) : 0u);
        bool same = w == A;
        if (same) {
            const int32_t t2 = (int32_t)e.R(u);
            const bool box2 = t2 == 2 || (t2 > 3 && t2 <= 5);
            const uint32_t p2 = e.R(u + 0x2c);
            w = box2 ? e.R(p2 + 0x10) : (p2 ? e.H(0xc2ec, 0, 0, 0, 0, 0, p2) : 0u);
            same = w == B;
        }
        if (!same) {
            uint32_t m = 0xa;
            if (isP) {
                m = 0;
                if (cls == 0x4e) m = 8;
                else if (cls == 0x21c) m = e.H(0xc4d8, 0, 0, 0, 0, 0, u, A, B) < 2 ? 0xcu : 0xau;
            }
            e.H(0xcb56, 0, 0, 0, 0, 0, u, m);
            e.W(u + 0x48, (e.R(u + 0x48) & 0xffffff00u) + 0x100);
            return 0;
        }
    }
    e.H(0xc6060, 0, 0, 0, A, u, B);
    if (u) {
        const uint32_t b = e.R(u + 0x54);
        if (b) {
            const uint32_t q = e.R(b + 0x30);
            if (q) { e.W(q + 4, 0); e.W(q + 8, 0); e.W(q + 0xc, 0); }
        }
    }
    if (s1) e.H(0x6b300, 0x50, 0, 0, 0, 0, u, 2, 0, 0, 0, 0, 0);
    const uint32_t eb = e.H(0x22fb0, u, 0, 0, 0, 0, 1);
    e.H(0x88a70, 0, 0, 0, eb, 0, u, 0, 0, 0);
    if (isP) {
        if (cls == 0x4e) e.H(0xc6150, s0, 0, 0, 0, 0, u, A, B);
    } else {
        e.H(0x88a70, 0, 0, 0, 0x9d5, 0, u, 0, 0, 0);
    }
    return 1;
}
