// 1.13c D2Client+0x7EA90 (EAX = the frame rect source, no stack arguments, plain ret): the per-frame speech/UI tick. Written from the disassembly
//. Globals are reached through `ga` (module RVA -> 32-bit address of the pinned bytes); module addresses handed
// to callees are the retail constants, logged by the capture as 0xB2000000|rva. The fatal exits (0x5A, 0xCFE) do not return in retail.
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t M = 0xB2000000u;   // a module address as the capture records it
}
struct D2C7EA90G { void* ctx; uint32_t (*at)(void* ctx, uint32_t rva); uint32_t now; };
extern "C" void d2c_7ea90(uint32_t a, const D2C7EA90G& g, const cl::Env& e) {
    auto G = [&](uint32_t rva) { return g.at(g.ctx, rva); };
    auto R = [&](uint32_t rva) { return e.R(G(rva)); };
    auto W = [&](uint32_t rva, uint32_t v) { e.W(G(rva), v); };
    W(0x113ace, R(0x113ace) + 1);                                  // inc dword
    const uint32_t l = e.R(a + 0x24), t = e.R(a + 0x28);
    // SetRect(0x113AD4, l, t, [a+0xC]-[a+4]+l, t-[a+8]+[a+0x10])
    W(0x113ad4, l); W(0x113ad8, t); W(0x113adc, e.R(a + 0xc) - e.R(a + 4) + l); W(0x113ae0, t - e.R(a + 8) + e.R(a + 0x10));
    if (R(0x113a80) == 1) {
        const uint32_t x = R(0x11ec38), y = R(0xe1384);
        W(0x113960, x); W(0x113968, x + 0x145); W(0x113964, y); W(0x113ae4, 1); W(0x11396c, y + 0x70);
    } else {
        W(0x113ae4, 0);
    }
    e.H(0xa4da0, M | 0x10a608, 0, 0, 0, 0, 0, 0, 0, M | 0x7dc00);   // EBX = the per-node callback, stack = the bucket table
    e.H(0xa4da0, M | 0x10a808, 0, 0, 0, 0, 0, 0, 0, M | 0x7daa0);
    e.H(0xa4da0, M | 0x10aa08, 0, 0, 0, 0, 0, 0, 0, M | 0x7daa0);
    uint32_t it = 0;
    const uint32_t own = R(0x113a68);
    bool ownerPath = false;
    if (R(0x113a80) == 1 && own != 0) {
        const uint32_t st = e.R(own + 0x16);
        ownerPath = st == 1 || st == 0;
    }
    if (ownerPath) {
        it = e.H(0x7e970, 0, 0, 0, 0, 0, R(0x113a7c), own);
        const uint32_t aca = R(0x113aca), id = R(0x113ac2);
        uint32_t node = 0;
        if (aca != 0) {
            for (node = R(0x10a808 + 4 * (id & 0x7f)); node != 0; node = e.R(node + 0xe4))
                if (e.R(node + 0xc) == id) break;
            if (node != 0 && e.R(node) != 1) {                     // push 0x5A; the fatal exit
                const uint32_t m = e.H(0xbf92, 0x5a);
                e.H(0xbf80, M | 0xd359f, m);
                e.H(0x3664, 0xffffffffu);
                return;
            }
        }
        const uint32_t bb = R(0x11bbfc);
        if (bb == 0 || e.R(bb + 0x10) == 0x11 || (aca != 0 && node != 0 && e.R(node + 0x10) == 0xc)) {
            if (aca != 0) { e.H(0x14910, 0, 0, 0, 0, 0, 0x30, 1); e.H(0x46a60, 0, 0, 0, 0, 0, 0, R(0x113ac2)); }
            e.H(0x7e930);
            if (R(0x123448) == 1) {
                W(0x123448, 0);
                const uint32_t w = e.H(0xd0b4);
                e.H(0x3cc60, w, M | 0xe1388, 0, 0, 0, 7);
            }
        }
    }
    if (R(0x113a84) != 0) {
        if (R(0x113a88) == 0) {
            const uint32_t m = e.H(0xbf92, 0xcfe);
            e.H(0xbf80, M | 0xd359f, m);
            e.H(0x3664, 0xffffffffu);
            return;
        }
        uint32_t live = 0;
        if (R(0x113a64) >= g.now) { e.H(0x7bad0, 0, 0, 0, 0, 0, 0, R(0x113a60), 0, 0, M | 0x113a90); live = 1; }
        else { e.H(0x7c650); W(0x113a84, 0); W(0x113aca, 0); }
        it += live;
    }
    if (it == 0 && R(0x113a80) == 1) {
        W(0x113aba, 0); W(0x113aca, 0); W(0x113a7c, 1); W(0x113a80, 0);
        if (R(0x123448) == 1) {
            W(0x123448, 0);
            const uint32_t w = e.H(0xd0b4);
            e.H(0x3cc60, w, M | 0xe1388, 0, 0, 0, 7);
        }
    }
    W(0x113a7c, 0);
    if (R(0xfade0) == 0) { e.H(0x7b730); e.H(0x7b580); }
    const uint32_t a70 = R(0x113a70);
    if (a70 != 0) e.H(0x53840, 0, 0, 0, 0, 0, 0, 0, 0, 0, a70);
}
