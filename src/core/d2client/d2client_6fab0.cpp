// 1.13c D2Client+0x6FAB0 (548 B, stdcall msg, ret 4): the IME composition message. imc = 0xBE2A(0xD0B4()) (ImmGetContext);
// with one, a 32-wide-char buffer (W) and a 200-byte ANSI buffer (A) follow each other on the stack, A zeroed first.
//  msg+0xC bit 0x800 (result string): n = 0xBE42(imc, 0x800, 0, 0) clamped to 199, 0xBE42(imc, 0x800, A, n), A[199] = 0,
//    MultiByteToWideChar(0, 1, A, -1, W, 0x20), W[31] = 0; the composition (0x11EC40) is appended to the chat line (0x11EC80)
//    when both lengths sum below 0x7F (the line length 0x11C028 grows by it), then W likewise; composition and reading
//    (0x11FE98) first words := 0.
//  msg+0xC bit 8 (composition string) and 0xBE60(imc) (ImmGetOpenStatus): n = 0xBE42(imc, 8, 0, 0); n > 0: A and W zeroed,
//    n clamped to 199, 0xBE42(imc, 8, A, n) > 0: A[199] = 0, MultiByteToWideChar as above, len = wcslen(W): below 0x1F W[len] = 0
//    else W[31] = 0; len below 0x20 copies W to the reading, else 0x1F chars of it, reading[31] = 0 and 0x88A70(0, 0, 0, 0).
//  Then 0xBE3C(0xD0B4(), imc) (ImmReleaseContext). Always msg+0x18 = 1, msg+0x1C = 0, and the EAX of 0xBE84(msg) is returned.
// MultiByteToWideChar goes through the host as 0xF0000010 (s0 = W, s1 = A, s2 = the wide capacity).
#include <cstring>
#include "client_host.h"
#include "d2client_hw.h"

struct D2c6fab0State {
    uint16_t* comp;    // D2Client+0x11EC40, 0x20 wide chars
    uint16_t* chat;    // D2Client+0x11EC80, 0x80 wide chars
    uint16_t* read;    // D2Client+0x11FE98, 0x22 wide chars
    int32_t* lineLen;  // D2Client+0x11C028
    uint32_t msgTag;   // how the host names msg in a call record
};

namespace {
int32_t wl(const uint16_t* s) { int32_t n = 0; while (s[n]) ++n; return n; }
void wcat(uint16_t* d, const uint16_t* s) { d += wl(d); while ((*d++ = *s++)) {} }
}

// 1.13c D2Client+0x6FAB0
extern "C" uint32_t d2c_6fab0(D2c6fab0State* s, uint32_t* msg, D2cHost h) {
    uint8_t frame[0x40 + 0xc8];                       // W then A, as retail lays them out
    uint16_t* W = (uint16_t*)frame; char* A = (char*)frame + 0x40;
    memset(frame, 0, sizeof frame);
    const uint32_t imc = hc(h, 0xbe2a, 0, 0, 0, 0, 0, hc(h, 0xd0b4));
    if (imc) {
        const uint32_t pW = (uint32_t)(uintptr_t)W, pA = (uint32_t)(uintptr_t)A;
        if (msg[3] & 0x800) {
            uint32_t n = hc(h, 0xbe42, 0, 0, 0, 0, 0, imc, 0x800, 0, 0);
            if (n > 0xc7) n = 0xc7;
            hc(h, 0xbe42, 0, 0, 0, 0, 0, imc, 0x800, 0xB3000000u, n);
            A[0xc7] = 0;
            hc(h, 0xf0000010, 0, 0, 0, 0, 0, pW, pA, 0x20);
            W[31] = 0;
            const int32_t lw = wl(W);
            if (wl(s->comp) + wl(s->chat) < 0x7f) { wcat(s->chat, s->comp); *s->lineLen += wl(s->comp); }
            if (wl(s->chat) + lw < 0x7f) { wcat(s->chat, W); *s->lineLen += lw; }
            s->comp[0] = 0; s->read[0] = 0;
        }
        if ((msg[3] & 8) && hc(h, 0xbe60, 0, 0, 0, 0, 0, imc)) {
            uint32_t n = hc(h, 0xbe42, 0, 0, 0, 0, 0, imc, 8, 0, 0);
            if (n) {
                memset(frame, 0, sizeof frame);
                if (n > 0xc7) n = 0xc7;
                if ((int32_t)hc(h, 0xbe42, 0, 0, 0, 0, 0, imc, 8, 0xB3000000u, n) > 0) {
                    A[0xc7] = 0;
                    hc(h, 0xf0000010, 0, 0, 0, 0, 0, pW, pA, 0x20);
                    const int32_t len = wl(W);
                    if (len < 0x1f) W[len] = 0; else W[31] = 0;
                    if (len < 0x20) { uint16_t* d = s->read; const uint16_t* q = W; while ((*d++ = *q++)) {} }
                    else {
                        int32_t i = 0;
                        for (; i < 0x1f && W[i]; ++i) s->read[i] = W[i];
                        for (; i < 0x1f; ++i) s->read[i] = 0;
                        s->read[0x1f] = 0;
                        hc(h, 0x88a70);
                    }
                }
            }
        }
        hc(h, 0xbe3c, 0, 0, 0, 0, 0, hc(h, 0xd0b4), imc);
    }
    msg[6] = 1; msg[7] = 0;
    return hc(h, 0xbe84, 0, 0, 0, 0, 0, s->msgTag);
}
