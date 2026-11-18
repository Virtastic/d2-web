// 1.13c D2Client+0x7D3C0 (stdcall, 2 stack words, ret 8): open the overhead speech/NPC panel on `unit` (0 closes it), `mode` the
// panel kind (compared and stored as a WORD). Written from the disassembly. Globals are reached
// through `at` (module RVA -> 32-bit address of the pinned bytes); module addresses handed to callees are logged 0xB2000000|rva.
// The fatal exits (0x5A: the old id names a non-player node; 0x1012: 0x7C690 fails) do not return in retail.
#include <cstdint>
#include "cl_env.h"
namespace {
constexpr uint32_t M = 0xB2000000u;
constexpr uint32_t kFile = M | 0xd4bdc;   // 0x6FB84BDC, the source-file string handed to the Fog free
}
struct D2C7D3C0G {
    void* ctx; uint32_t (*at)(void* ctx, uint32_t rva); uint32_t now;
    void* wctx; void (*wb)(void* wctx, const uint8_t* p, uint32_t n);   // the bytes of the 0x143E0 send (stack buffer, EBX = length)
};
extern "C" void d2c_7d3c0(uint32_t unit, uint32_t mode, const D2C7D3C0G& g, const cl::Env& e) {
    auto G = [&](uint32_t rva) { return g.at(g.ctx, rva); };
    auto R = [&](uint32_t rva) { return e.R(G(rva)); };
    auto W = [&](uint32_t rva, uint32_t v) { e.W(G(rva), v); };
    auto fatal = [&](uint32_t code) { const uint32_t m = e.H(0xbf92, code); e.H(0xbf80, M | 0xd359f, m); e.H(0x3664, 0xffffffffu); };
    W(0x11ec38, (uint32_t)(((int32_t)R(0xdbc48) - 0x145) / 2));
    W(0x113af6, 0); W(0xe1384, 0xc); W(0x113af0, 0); W(0xf46c8, 0x10); W(0xf46c4, 0x10);
    W(0x11c3dc, 0); W(0x11c3e0, 0); W(0x11c3f8, 0); W(0x11c3fc, 0); W(0x11c2f8, 0); W(0x11c2f4, 0);
    e.H(0x7c9f0);
    if (unit == 0) {
        if (R(0x113a84) != 0) {
            const uint32_t o = R(0x113a60);
            if (o != 0) {
                e.H(0x7b3d0, 0, 0, 0, 0, 0, e.R(o));
                e.H(0xbf86, 0x68a, 0, 0, 0, 0, 0, o, kFile);
                W(0x113a60, 0);
            }
            W(0x113a84, 0);
        }
        W(0x113aca, 0); W(0x113ac2, 0);
    } else {
        if (R(0x113aca) != 0) {
            const uint32_t id = R(0x113ac2);
            for (uint32_t n = R(0x10a808 + 4 * (id & 0x7f)); n != 0; n = e.R(n + 0xe4)) {
                if (e.R(n + 0xc) != id) continue;
                if (e.R(n) != 1) { fatal(0x5a); return; }
                if (n != unit) {
                    uint8_t pk[9] = { 0x30, 1, 0, 0, 0, (uint8_t)id, (uint8_t)(id >> 8), (uint8_t)(id >> 16), (uint8_t)(id >> 24) };
                    e.H(0x143e0, (uint32_t)(uintptr_t)pk, 0, 0, 0, 0, 0, 0, 0, 9);
                    if (g.wb) g.wb(g.wctx, pk, 9);
                    e.H(0x46a60, 0, 0, 0, 0, 0, 0, R(0x113ac2));
                }
                break;
            }
        }
        W(0x113aca, 1);
        W(0x113ac2, e.R(unit + 0xc));
    }
    e.H(0x7ae00);
    W(0x113ad2, (R(0x113ad2) & 0xffff0000u) | (mode & 0xffff));
    if (R(0x113a68) != 0) return;
    W(0x113a7c, 0);
    const uint32_t font = e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 8);
    const uint32_t s = e.H(0xd1ec, 2, 0, 0, 0, 0, 0, mode);
    const uint32_t o = e.H(0x7c690, s);
    W(0x113a68, o);
    if (o == 0) { fatal(0x1012); return; }
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, font);
    e.H(0x886f0);
    W(0x113b0b, 0);
    if (R(0xdf0d8) != 0) {
        for (uint32_t p = 0xdf398; R(p) != 0; p += 8) {
            if ((R(p + 4) & 0xffff) != (mode & 0xffff)) continue;
            const uint32_t v = R(p);
            e.H(0x22eb0, 0, 0, 0, 0, 0, unit);
            W(0x113b0b, e.H(0x88a70, R(0x11bbfc), 5, 0, 0));
            W(0x11bf9c, v);
            break;
        }
    }
    e.W(R(0x113a68) + 0x16, 0);
    W(0x113a80, 1);
    if (R(0x123448) == 0) {
        W(0x123448, 1);
        W(0x113aec, g.now);
        const uint32_t w = e.H(0xd0b4);
        e.H(0x3ccc0, w, M | 0xe1388, 0, 0, 0, 7);
    }
}
