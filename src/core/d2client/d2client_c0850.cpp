// 1.13c D2Client+0xC0850 (0x490 B, stdcall, one stack word = the hovered unit P, ret 4): the item-hover name line and its
// description line.
// The font is switched to 1 (D2Win 10184 thunk 0xD3A8, ECX = font, returns the old one) and restored at the end, except on the
// early return of a mode-0xC item whose 10057(10331(P)) test is set. The item row (items table, stride 0x1A8, through the
// twice-indirect sgptDataTables slot 0xCE474: base +0xA78, count +0xA80) gates the line on its flags byte +0xD against
// gdwBitMasks[1]/[7] and its word +6. The line is strcpy/strcat'd into a 0x800-char stack buffer from 10003 lookups (0xF9B spacer,
// 0x90B), the unit name (0xA5D90, EAX = P) and "(" / ")" widened through win2Unicode, measured (D2Win 10028, ECX = buffer) and
// drawn centred on the rect 0xBEA10 fills (D2Win 10070: ECX buffer, EDX x, stack 0x20, colour, 0, 0xC0590(EDI = P)).
// The description line goes through 0xC01E0 (ECX = P, stack = string id) and 0xBE6D0 (EDX = text, stack = colour), then 0xBFCA0(P).
// The other branch clears the word at 0x11C780 and, with 0x119721 and 0x113A68 both zero, draws the unit name at
// (0xDBC54, 0xDBC58 - row byte +0xD) through D2Win 10085.
// Every import and callee goes through e.H with the registers and stack words retail passes; the Unicode ctor runs real in
// retail and is not a call here.
#include "cl_env.h"
namespace {
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
constexpr uint32_t kStr = 0xCEACC, kStrcpy = 0xCEAB8, kStrcat = 0xCEABC, kW2U = 0xCEAC0;
uint32_t str(const cl::Env& e, uint32_t id) { return e.H(kStr, 0, 0, 0, 0, 0, 0, id); }
void cpy(const cl::Env& e, uint32_t d, uint32_t s) { e.H(kStrcpy, 0, 0, 0, 0, 0, 0, d, s); }
void cat(const cl::Env& e, uint32_t d, uint32_t s) { e.H(kStrcat, 0, 0, 0, 0, 0, 0, d, s); }
inline uint32_t A(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
}  // namespace
// 1.13c D2Client+0xC0850
extern "C" void d2c_c0850(uint32_t P, const cl::Env& e) {
    volatile uint16_t buf[0x800];
    volatile uint16_t sm[2];
    volatile uint32_t ch;
    volatile int32_t rc[4];
    volatile uint16_t line[0x200];
    const uint32_t oldFont = e.H(0xD3A8, 0, 0, 0, 0, 0, 0, 1);
    int32_t cls = -1;
    uint32_t row = 0;
    if (P) {
        cls = (int32_t)e.R(P + 4);
        if (cls >= 0) {
            const uint32_t dt = e.R(e.R(G(0xCE474)));
            if (cls < (int32_t)e.R(dt + 0xA80)) row = e.R(dt + 0xA78) + (uint32_t)cls * 0x1A8;
        }
    }
    uint32_t paren = 0;
    const uint32_t fl = e.R8(row + 0xD);
    const uint32_t bm = e.R(G(0xCEE50));
    const auto sub58 = [&]() {
        const uint32_t s = e.H(0x1260, 0, 0, 0, 0, 0, P ? e.R(P + 4) : 0xFFFFFFFFu);
        if (s) e.W(G(0xDBC58), e.R(G(0xDBC58)) - e.R8(s + 0xD));
    };
    const auto clr780 = [&]() { e.W(G(0x11C780), e.R(G(0x11C780)) & 0xFFFF0000u); };
    if ((e.R(bm + 4) & fl) || !(e.R(bm + 0x1C) & fl) || (e.R(row + 6) & 0xFFFF) == 0) {
        clr780();
        if (e.R(G(0x119721)) == 0 && e.R(G(0x113A68)) == 0) {
            sub58();
            const uint32_t y = e.R(G(0xDBC58));
            const uint32_t nm = e.H(0xA5D90, 0, 0, 0, 0, 0, P);
            e.H(0xD384, y, 0, 1, 0, 0, 0, nm, e.R(G(0xDBC54)));
        }
        e.H(0xD3A8, 0, 0, 0, 0, 0, 0, oldFont);
        return;
    }
    sub58();
    const uint32_t B = A(buf);
    cpy(e, B, str(e, 0xF9B));
    cat(e, B, str(e, 0xF9B));
    cat(e, B, e.H(0xA5D90, 0, 0, 0, 0, 0, P));
    cat(e, B, str(e, 0xF9B));
    if (P && e.R(P + 0x10) == 0xC) {
        if (e.H(0xC208, e.H(0xC1F0, P))) return;   // retail leaves the font switched
        paren = 1;
        if (e.H(0xD1E6) == 0) {
            cat(e, B, str(e, 0x90B));
        } else {
            ch = 0x28; e.H(kW2U, 2, 0, 0, 0, 0, 0, A(sm), A(&ch));
            cat(e, B, A(sm));
            cat(e, B, str(e, 0x90B));
            ch = 0x29; e.H(kW2U, 2, 0, 0, 0, 0, 0, A(sm), A(&ch));
            cat(e, B, A(sm));
        }
        cat(e, B, str(e, 0xF9B));
    }
    cat(e, B, str(e, 0xF9B));
    const uint32_t col = e.H(0xC0590);
    e.Ej(0xC0590, P);
    uint32_t m = 0;
    if (P && e.R(P) == 1) {
        const uint32_t d = e.R(P + 0x14);
        if (d && (e.R8(d + 0x16) & 8)) m = 4;
        if (d && (e.R8(d + 0x16) & 4)) m = 3;
    }
    if (paren) m = 1;
    switch (cls) {
    case 0xF3: case 0x14D: case 0xF2: case 0xD3: case 0x220: case 0x23A: case 0xE5:
    case 0x2C4: case 0x2C3: case 0x2C2: case 0x2C1: case 0x2C5: case 0x2C0: m = 4; break;
    default: break;
    }
    if (e.H(0xBEA10, 0, 0, 0, 0, 0, 0, A(rc)) == 0) { e.H(0xD3A8, 0, 0, 0, 0, 0, 0, oldFont); return; }
    const int32_t wpx = (int32_t)e.H(0xD35A, 0, 0, 0, 0, 0, 0, B);
    const int32_t x = ((rc[0] + rc[2]) >> 1) - (wpx >> 1);
    e.H(0xD360, 0x20, m, 0, col, 0, 0, B, (uint32_t)x);
    if (paren) { clr780(); e.H(0xD3A8, 0, 0, 0, 0, 0, 0, oldFont); return; }
    const uint32_t g = e.H(0x1230, 0, 0, 0, 0, 0, P ? e.R(P + 4) : 0xFFFFFFFFu);
    uint32_t sv = 0;
    if (e.H(0xC20E, P)) sv = 0x275E;
    else if (e.H(0xC1D8, P)) sv = 0x275D;
    if (e.H(0xC01E0, sv, 0, 0, 0, 0, 0, P) == 0) {
        const uint32_t gid = e.R(g + 8) & 0xFFFF;
        if (sv & 0xFFFF) {
            const uint32_t L = A(line);
            cpy(e, L, str(e, sv));
            cat(e, L, str(e, 0xF9B));
            if (gid != 0x1506) cat(e, L, str(e, gid));
            e.H(0xBE6D0, m, 0, 0, 0, 0, 0, 0, L);
        } else if (gid != 0x1506) {
            e.H(0xBE6D0, m, 0, 0, 0, 0, 0, 0, str(e, gid));
        }
    }
    e.H(0xBFCA0, P);
    e.H(0xD3A8, 0, 0, 0, 0, 0, 0, oldFont);
}
