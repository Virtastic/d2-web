// 1.13c D2Client+0x9B530 -- left-button-release listener of the panel modes 0xC and 0xD (stdcall msg, ret 4). With the panel mode [0x11BC34] neither
// only [0x11BEEC] clears. Otherwise the point (x = msg+0xC word, y = msg+0xE word) is stored ([0x10B2A4], [0x10B430]) and the latches [0x11BEDC],
// [0x11BEE0] clear. The panel is closed when ([0x11C474] zero or 0x26320 (EAX = msg) zero) and 0xC202 (player+0x60) is zero and 0x8BD90 (ECX = x, EDX = y)
// is nonzero: 0xBF8C runs, [0xDD5D0] must be 0x26 (else the 0x139B assertion), [0xFADE4] is read and cleared and, when it was set, 0xBF720, the panel
// origin clear ([0x11C418], [0x11C414], 0xD1B0(0), 0x8A4D0), 0x8F140 and the string 0xDBDB8 (through 0xD0B4, handed to 0x3CC60 with EAX = 8) are run,
// 0xBE2E0 runs, [0x11C434] := 1 and the first time ([0x12349C] clear -> 1) the string 0xF51EC (EAX = 2, only when [0x123498] is set, which then
// clears) and the string 0xF51BC (0x3CCC0, EAX = 4) follow. Both ends clear the panel origin again and call 0x8A4D0 (EAX = [0xF9E18], ECX = [0xF9E14],
// EDX = 0, ESI = [0x11C3C4], stack = 0). Any other state: 0x8CD00 nonzero clears [0x11BE84] (0x8CCB0 when set), [0x11BE88], [0x11BEEC] and finishes the
// message (+0x18 = 1, +0x1C = 0, 0xBE84); else [0x11BE84], [0x11BE88] clear, with [0x11C474] zero 0x8D3D0 (EAX = y, EDI = x) nonzero and [0x11BE78] clear
// runs 0xC30E0(3), 0x8B6B0 (EAX = y) nonzero with [0x11BE78] clear runs 0xC30E0(4), and 0x26320 equal to 1 clears [0x11BE6C], [0x11BE88] and returns
// unfinished. Then [0x11BE80], [0x11BE7C] clear; with [0x11BEEC] clear the message is finished and [0x11BEEC], [0x11BE84], [0x11BE64] clear, otherwise
// [0x11BEEC] clears, [0x11BE6C] set resets [0x11BE64] (when 1), [0x11BE6C] clear resets [0x11BE84] (when [0x11BE88] and [0x11BE84] are set), [0x11BE6C]
// and [0x11BE88] clear and the message is finished.
#include "cl_pa4.h"
extern "C" uint32_t d2c_9b530(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    const uint32_t mode = G(e, 0x11bc34);
    if (mode != 0xc && mode != 0xd) { S(e, 0x11beec, 0); return 0; }
    const uint32_t sv = e.R(G(e, 0x11bbfc) + 0x60);
    const uint32_t x = P16(e, msg + 0xc), y = P16(e, msg + 0xe);
    const uint32_t tab = G(e, 0x11c474);
    S(e, 0x11bedc, 0);
    S(e, 0x11bee0, 0);
    S(e, 0x10b2a4, x);
    S(e, 0x10b430, y);
    bool close = false;
    if (tab == 0 || e.H(0x26320, 0, 0, 0, 0, 0, msg) == 0)
        close = e.H(0xc202, sv) == 0 && e.H(0x8bd90, 0, 0, 0, 0, 0, 0, x, y) != 0;
    if (close) {
        const uint32_t old = G(e, 0xfade4);
        e.H(0xbf8c);
        if (G(e, 0xdd5d0) != 0x26) { assertAt(e, 0x139b); return 0; }
        S(e, 0xfade4, 0);
        if (old != 0) {
            e.H(0xbf720);
            S(e, 0x11c418, 0);
            S(e, 0x11c414, 0);
            e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
            e.H(0x8a4d0, 0, 0, 0, 0, 0, G(e, 0xf9e18), G(e, 0xf9e14), 0, 0, G(e, 0x11c3c4));
            e.H(0x8f140);
            e.H(0x3cc60, e.H(0xd0b4, kBase + 0xdbdb8), kBase + 0xdbdb8, 0, 0, 0, 8);
            e.H(0xbe2e0);
            const uint32_t seen = G(e, 0x12349c);
            S(e, 0x11c434, 1);
            if (seen == 0) {
                const uint32_t pend = G(e, 0x123498);
                S(e, 0x12349c, 1);
                if (pend != 0) {
                    e.H(0x3cc60, e.H(0xd0b4, kBase + 0xf51ec), kBase + 0xf51ec, 0, 0, 0, 2);
                    S(e, 0x123498, 0);
                }
                e.H(0x3ccc0, e.H(0xd0b4, kBase + 0xf51bc), kBase + 0xf51bc, 0, 0, 0, 4);
            }
        }
        S(e, 0x11c418, 0);
        S(e, 0x11c414, 0);
        e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
        e.H(0x8a4d0, 0, 0, 0, 0, 0, G(e, 0xf9e18), G(e, 0xf9e14), 0, 0, G(e, 0x11c3c4));
        return 0;
    }
    if (e.H(0x8cd00) != 0) {
        if (G(e, 0x11be84) != 0) {
            S(e, 0x11be84, 0);
            e.H(0x8ccb0);
        }
        S(e, 0x11be88, 0);
        S(e, 0x11beec, 0);
        return fin(e, msg);
    }
    const bool noTab = G(e, 0x11c474) == 0;
    S(e, 0x11be84, 0);
    S(e, 0x11be88, 0);
    if (noTab) {
        const uint32_t hit = e.H(0x8d3d0, 0, 0, 0, 0, 0, y);
        e.Ej(0x8d3d0, x);
        if (hit != 0 && G(e, 0x11be78) == 0) e.H(0xc30e0, 3);
    }
    if (e.H(0x8b6b0, 0, 0, 0, 0, 0, y) != 0 && G(e, 0x11be78) == 0) e.H(0xc30e0, 4);
    if (e.H(0x26320, 0, 0, 0, 0, 0, msg) == 1) {
        S(e, 0x11be6c, 0);
        S(e, 0x11be88, 0);
        return 0;
    }
    const bool held = G(e, 0x11beec) != 0;
    S(e, 0x11be80, 0);
    S(e, 0x11be7c, 0);
    if (!held) {
        e.W(msg + 0x18, 1);
        e.W(msg + 0x1c, 0);
        e.H(0xbe84, msg);
        S(e, 0x11beec, 0);
        S(e, 0x11be84, 0);
        S(e, 0x11be64, 0);
        return 0;
    }
    const bool lat = G(e, 0x11be6c) != 0;
    S(e, 0x11beec, 0);
    if (lat) {
        if (G(e, 0x11be64) == 1) S(e, 0x11be64, 0);
    } else if (G(e, 0x11be88) != 0 && G(e, 0x11be84) != 0) {
        S(e, 0x11be84, 0);
    }
    S(e, 0x11be6c, 0);
    S(e, 0x11be88, 0);
    return fin(e, msg);
}
