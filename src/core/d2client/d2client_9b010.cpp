// 1.13c D2Client+0x9B010 -- left-button-release listener of the panel mode 0xE (stdcall msg, ret 4). With the panel mode [0x11BC34] not 0xE only
// [0x11BEEC] clears. Otherwise the point (x = msg+0xC word, y = msg+0xE word) is stored ([0x10B2A4], [0x10B430]) and the latches [0x11BEDC],
// [0x11BEE0] clear. With [0x11C474] zero, 0xC202 (player+0x60) zero and 0x8BD90 (ECX = x, EDX = y) nonzero the panel is closed: 0xBF8C runs,
// [0xDD5D0] must be 0x26 (else the 0x139B assertion), [0xFADE8] is read and cleared and, when it was set, the panel origin clears ([0x11C418],
// [0x11C414], 0xD1B0(0), 0x8A4D0), 0x97840 and the string 0xDBE18 (through 0xD0B4, handed to 0x3CC60 with EAX = 7) are run, 0xBE2E0 runs, [0x11C434] := 1
// and, the first time ([0x12349C] clear -> 1), the string 0xF51EC (EAX = 2, only when [0x123498] is set, which then clears) and the string 0xF51BC
// (0x3CCC0, EAX = 4) follow. Both ends clear the panel origin again and call 0x8A4D0 (EAX = [0xF9E18], ECX = [0xF9E14], EDX = 0, ESI = [0x11C3C4],
// stack = 0) and return. Any other state takes the hit tests: 0x8B650 (EAX = x, EDX = y) nonzero clears [0x11BE8C] (running 0x97770 when it was
// set) and [0x11BEEC], [0x11BE90]; else 0x8B600 nonzero clears [0x11BE94] (0x8CB90 when set), [0x11BE98], [0x11BEEC]; else the four latches
// [0x11BE8C..98] clear and 0x26320 (EAX = msg) equal to 1 clears [0x11BE6C], [0x11BE90], [0x11BE98] and returns unfinished. Otherwise 0x8D3D0 (EAX = y) nonzero
// with [0x11BE78] clear runs 0xC30E0(1), [0x11BE7C] clears and, with [0x11BEEC] clear, the message is finished and [0x11BEEC], [0x11BE8C], [0x11BE94],
// [0x11BE64] clear; with it set the state words update by [0x11BE6C] / [0x11BE90] / [0x11BE98] and the message is finished (+0x18 = 1, +0x1C = 0, 0xBE84).
#include "cl_pa4.h"
extern "C" uint32_t d2c_9b010(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    if (G(e, 0x11bc34) != 0xe) { S(e, 0x11beec, 0); return 0; }
    const uint32_t tab = G(e, 0x11c474);
    const uint32_t sv = e.R(G(e, 0x11bbfc) + 0x60);
    const uint32_t y = P16(e, msg + 0xe), x = P16(e, msg + 0xc);
    S(e, 0x11bedc, 0);
    S(e, 0x11bee0, 0);
    S(e, 0x10b2a4, x);
    S(e, 0x10b430, y);
    if (tab == 0 && e.H(0xc202, sv) == 0 && e.H(0x8bd90, 0, 0, 0, 0, 0, 0, x, y) != 0) {
        const uint32_t old = G(e, 0xfade8);
        e.H(0xbf8c);
        if (G(e, 0xdd5d0) != 0x26) { assertAt(e, 0x139b); return 0; }
        S(e, 0xfade8, 0);
        if (old != 0) {
            S(e, 0x11c418, 0);
            S(e, 0x11c414, 0);
            e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
            e.H(0x8a4d0, 0, 0, 0, 0, 0, G(e, 0xf9e18), G(e, 0xf9e14), 0, 0, G(e, 0x11c3c4));
            e.H(0x97840);
            e.H(0x3cc60, e.H(0xd0b4, kBase + 0xdbe18), kBase + 0xdbe18, 0, 0, 0, 7);
            e.H(0xbe2e0);
            const uint32_t seen = G(e, 0x12349c);
            S(e, 0x11c434, 1);
            if (seen == 0) {
                S(e, 0x12349c, 1);
                if (G(e, 0x123498) != 0) {
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
    if (e.H(0x8b650, 0, 0, 0, 0, 0, x, 0, y) != 0) {
        if (G(e, 0x11be8c) != 0) {
            S(e, 0x11be8c, 0);
            e.H(0x97770);
        }
        S(e, 0x11beec, 0);
        S(e, 0x11be90, 0);
        return fin(e, msg);
    }
    if (e.H(0x8b600, 0, 0, 0, 0, 0, x, 0, y) != 0) {
        if (G(e, 0x11be94) != 0) {
            S(e, 0x11be94, 0);
            e.H(0x8cb90);
        }
        S(e, 0x11be98, 0);
        S(e, 0x11beec, 0);
        return fin(e, msg);
    }
    S(e, 0x11be8c, 0);
    S(e, 0x11be90, 0);
    S(e, 0x11be94, 0);
    S(e, 0x11be98, 0);
    if (e.H(0x26320, 0, 0, 0, 0, 0, msg) == 1) {
        S(e, 0x11be6c, 0);
        S(e, 0x11be90, 0);
        S(e, 0x11be98, 0);
        return 0;
    }
    const uint32_t hit = e.H(0x8d3d0, 0, 0, 0, 0, 0, y);
    e.Ej(0x8d3d0, x);
    if (hit != 0 && G(e, 0x11be78) == 0) e.H(0xc30e0, 1);
    S(e, 0x11be7c, 0);
    if (G(e, 0x11beec) == 0) {
        e.W(msg + 0x18, 1);
        e.W(msg + 0x1c, 0);
        e.H(0xbe84, msg);
        S(e, 0x11beec, 0);
        S(e, 0x11be8c, 0);
        S(e, 0x11be94, 0);
        S(e, 0x11be64, 0);
        return 0;
    }
    S(e, 0x11beec, 0);
    if (G(e, 0x11be6c) != 0) {
        if (G(e, 0x11be64) != 0) S(e, 0x11be64, 0);
    } else if (G(e, 0x11be90) == 0) {
        if (G(e, 0x11be98) != 0 && G(e, 0x11be94) != 0) S(e, 0x11be94, 0);
    } else if (G(e, 0x11be8c) != 0) {
        S(e, 0x11be8c, 0);
    }
    S(e, 0x11be6c, 0);
    S(e, 0x11be98, 0);
    S(e, 0x11be90, 0);
    return fin(e, msg);
}
