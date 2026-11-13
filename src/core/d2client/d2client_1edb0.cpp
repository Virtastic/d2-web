// 1.13c D2Client+0x1EDB0: pointer-up handler of the belt/inventory slot drag. Stack arg ev (ret 4): ev+0x0C x (u16), +0x0E y (u16);
// writes ev+0x18 / +0x1C and the drag state. The module globals it touches are passed as memory images of their ranges:
//  A = image at +1223E0 (+0x00 slot count, +0x10/+0x11/+0x12/+0x13/+0x14 flag bytes at +1223F0..F4, +0x15 selected slot dword (+1223F5),
//      +0x19 last slot dword (+1223F9), +0x1D slot table dwords (+1223FD))
//  B = image at +123394 (+0x01 table index dword (+123395), +0x09..+0x0C four flag bytes at +12339D..A0)
//  T = image at +0F8108 (slot boxes, stride 0x10: x, y)   O = image at +11B9A0 (origin x, y)   C = image at +11C3DC (+0x00, +0x04, +0x1C, +0x20)
//  S = image at +0DBC48 (screen w, h; +0x08 is the scripted D2Gfx 10031 answer in the FN probe only, never read by the port)
// Callees: 0x1E940 (stdcall one word), 0xD0AE (D2Gfx ordinal 10031, no args), 0xBE84 (stdcall ev), 0x1DFA0 (ESI = last slot).
// 0x1C7A0 (slot box lookup, EBX = x, EDI = y) and 0x1C800 (EAX = x, ECX = y range test) are inlined.
#include <cstring>
#include "d2client_callee.h"
namespace {
constexpr uint32_t kOpen = 0x1e940, kGfx = 0xd0ae, kNotify = 0xbe84, kPick = 0x1dfa0, kStrId = 0xbf92, kMsg = 0xbf80, kExit = 0x3664;
inline uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t x; std::memcpy(&x, p + o, 4); return x; }
inline void wr(uint8_t* p, uint32_t o, uint32_t x) { std::memcpy(p + o, &x, 4); }
}
// 1.13c D2Client+0x1EDB0
extern "C" void d2c_1edb0(const D2cSvc& v, uint8_t* ev, uint8_t* A, uint8_t* B, const uint8_t* T, const uint8_t* O, uint8_t* C, const uint8_t* S) {
    uint16_t x16, y16; std::memcpy(&x16, ev + 0xc, 2); std::memcpy(&y16, ev + 0xe, 2);
    const int32_t x = (int32_t)(x16 - rd(O, 0)), y = (int32_t)(y16 + rd(O, 4));
    const int32_t w = (int32_t)rd(S, 0), h = (int32_t)rd(S, 4);
    if (A[0x12] != 0) {
        if (x > 0x115 && x < 0x13a && y > 0x187 && y < 0x1aa) d2c_call(v, kOpen, 0, 0, 0, 0, 0, 0);
    } else if (A[0x13] == 0) {
        if (rd(A, 0x15) == 0xffffffffu) {
            if (A[0x14] == 1 && x > 0xe5 && x < 0x104 && y > 0x186 && y < 0x1a8 && rd(A, 0x19) != 0xffffffffu)
                d2c_call(v, kPick, 0, 0, 0, 0, rd(A, 0x19));
        } else {
            const uint32_t n = rd(A, 0);
            uint32_t hit = 0xffffffffu;
            if (n > 6) {                                   // retail shows an error box and exits the process
                d2c_call(v, kMsg, 0, 0, 0, 0, 0, 0xd359f, d2c_call(v, kStrId, 0, 0, 0, 0, 0, 0x738));
                d2c_call(v, kExit, 0, 0, 0, 0, 0, 0xffffffffu);
            } else {
                for (uint32_t i = 0; i < n; ++i) {
                    const int32_t bx = (int32_t)rd(T, i * 0x10), by = (int32_t)rd(T, i * 0x10 + 4);
                    if (x >= bx && x < bx + 0x38 && y >= by && y < by + 0x34) { hit = i; break; }
                }
            }
            if (hit == rd(A, 0x15)) { wr(A, 0x1d + rd(B, 1) * 4, hit); wr(A, 0x19, hit); }
        }
    }
    A[0x11] = 0; A[0x12] = 0; A[0x10] = 0; wr(A, 0x15, 0xffffffffu); A[0x14] = 0;
    B[9] = 0; B[0xa] = 0; B[0xb] = 0; B[0xc] = 0;
    if ((int32_t)x16 > w / 2) return;
    const int32_t limit = d2c_call(v, kGfx, 0, 0, 0, 0, 0) == 2 ? h - 0x2f : h - 0x31;
    if ((int32_t)y16 > limit) return;
    wr(C, 0, 0); wr(C, 4, 0); wr(C, 0x1c, 0); wr(C, 0x20, 0);
    wr(ev, 0x18, 1); wr(ev, 0x1c, 0);
    d2c_call(v, kNotify, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)ev);
}
