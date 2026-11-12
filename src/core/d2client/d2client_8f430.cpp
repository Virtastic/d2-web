// 1.13c D2Client+0x8F430: the bottom-bar button layout by game-state code. With the active word D2Client+0x119721 set, finds the
// game id D2Client+0x11971D in the hash chain D2Client+0x10A808 (bucket id & 0x7f, key +0xC, next +0xE4); a record whose +0 is not
// 1 is fatal. Without the flag D2Client+0x1197A8 the record's +4 code picks layout COMMON, NINE_A or OFF; with it the alternate
// layout. The layout helpers 0x8BA30 / 0x8B9F0 / 0x8B9C0 / 0x8BA70 / 0x8BAB0 take their index in EAX. Writes the layout block
// D2Client+0x10B23C.., the mode byte D2Client+0x11BC50 and the pair D2Client+0xE11C4 / +0xE11C8 with retail's widths.
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t M = 0x6FAB0000u;
void W16(const cl::Env& e, uint32_t a, uint32_t v) {
    const uint32_t q = a & ~3u, sh = (a & 3u) * 8;
    e.W(q, (e.R(q) & ~(0xffffu << sh)) | ((v & 0xffffu) << sh));
}
void W8(const cl::Env& e, uint32_t a, uint32_t v) {
    const uint32_t q = a & ~3u, sh = (a & 3u) * 8;
    e.W(q, (e.R(q) & ~(0xffu << sh)) | ((v & 0xffu) << sh));
}
}
extern "C" void d2cl_8f430(const cl::Env& e) {
    if (!e.R(M + 0x119721u)) return;
    const uint32_t id = e.R(M + 0x11971Du);
    uint32_t p = e.R(M + 0x10A808u + (id & 0x7fu) * 4);
    if (!p) return;
    while (e.R(p + 0xc) != id) { p = e.R(p + 0xe4); if (!p) return; }
    if (e.R(p) != 1) __builtin_trap();   // Ordinal_10265(0x5a), Ordinal_10024, exit(-1)
    auto tail = [&]() {
        e.W(M + 0xE11C8u, 4); e.W(M + 0xE11C4u, 3);
        e.W(M + 0x10B23Cu, 0); e.W(M + 0x10B250u, 0); e.W(M + 0x10B264u, 0); e.W(M + 0x10B278u, 0);
    };
    auto common = [&]() {
        e.W(M + 0x10B274u, 1); W16(e, M + 0x10B27Cu, 0x1030); e.W(M + 0x10B27Eu, 9); W16(e, M + 0x10B282u, 0xa);
        tail();
    };
    if (e.R(M + 0x1197A8u)) {
        W8(e, M + 0x11BC50u, 1); e.H(0x8ba30, 0, 0, 0, 0, 0, 0); e.H(0x8b9f0, 0, 0, 0, 0, 0, 1);
        W16(e, M + 0x10B268u, 0); e.W(M + 0x10B26Au, 1); e.W(M + 0x10B270u, 1); W16(e, M + 0x10B26Eu, 0); e.W(M + 0x10B260u, 0);
        common();
        return;
    }
    const int32_t c = (int32_t)e.R(p + 4);
    int k = 2;   // 0 COMMON, 1 NINE_A, 2 OFF
    if (c == 0xfc || c == 0x195) k = 0;
    else if (c < 0xfc) {
        switch (c) {
        case 0x93: case 0x94: case 0xb1: case 0xc7: case 0xca: k = 0; break;
        case 0x9a: case 0xb2: k = 1; break;
        default: break;
        }
    } else if (c < 0x195) {
        switch (c) {
        case 0xfd: case 0x101: k = 1; break;
        case 0xfe: case 0xff: k = 0; break;
        default: break;
        }
    } else if (c == 0x1ff) k = 1;
    else if (c == 0x200 || c == 0x201) k = 0;
    if (k == 2) { e.W(M + 0xE11C8u, 0); return; }
    W8(e, M + 0x11BC50u, 3); e.H(0x8ba30, 0, 0, 0, 0, 0, 0); e.H(0x8b9f0, 0, 0, 0, 0, 0, 1);
    if (k == 1) { e.H(0x8ba70, 0, 0, 0, 0, 0, 2); e.H(0x8bab0, 0, 0, 0, 0, 0, 3); tail(); return; }
    e.H(0x8b9c0, 0, 0, 0, 0, 0, 2);
    common();
}
