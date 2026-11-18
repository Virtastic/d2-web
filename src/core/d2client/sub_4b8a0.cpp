// 1.13c D2Client+0x4B8A0, from the disassembly: AL = slot, stack p1 p2 p3, ret 0xC, void.
// Slot 6 only forwards (Fog 10795). Otherwise the entry for (p1, slot) is looked up in the 0x200-byte-per-slot table at
// 0x10A608 (0xA4E20: EAX = p1 & 0x7F, ECX = p1, EDX = table, stack slot). No entry: the player's data block +0x150..+0x15C is
// cleared (player type 0), a 9-byte '0' message (p3 low byte, p1) goes out through 0x143E0 and 0x46A60(ECX = p1) runs. An entry:
// it is registered (0xA4DA0, EBX = 0x46770), flag bit 1 of +0xC4 set, the '/' message (slot, p1) sent, the cursor/panel globals
// reset, then by the state flags it either hands off to 0x46070, opens the slot 10778 answered, the one 10167 answers, or the
// fallbacks 0x4A4F0 / 0xC2790, ending in the 0x47B20 + 10227 tail. Both _exit paths are mirrored up to the exit thunk.
#include <cstdint>
#include <initializer_list>
#include "cl_env.h"
namespace {
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
constexpr uint32_t T(uint32_t rva) { return 0xB2000000u | rva; }   // a module address handed to a callee, logged as the capture tags it
inline uint32_t P(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
void W8(const cl::Env& e, uint32_t a, uint32_t v) { e.W(a, (e.R(a) & ~0xFFu) | (v & 0xFF)); }
void W16(const cl::Env& e, uint32_t a, uint32_t v) { e.W(a, (e.R(a) & ~0xFFFFu) | (v & 0xFFFF)); }
void Send(const cl::Env& e, uint8_t tag, uint32_t a, uint32_t b) {
    volatile uint8_t m[12] = {};
    m[0] = tag;
    for (int i = 0; i < 4; ++i) { m[1 + i] = (uint8_t)(a >> 8 * i); m[5 + i] = (uint8_t)(b >> 8 * i); }
    e.H(0x143E0, P(m), 0, 0, 0, 0, 0, 0, 0, 9);
}
void Exit(const cl::Env& e, uint32_t code) {
    const uint32_t s = e.H(0xBF92, code);
    e.H(0xBF80, T(0xD359F), s);
    e.H(0x3664, 0xFFFFFFFFu);
}
}
extern "C" void d2c_sub_4b8a0(const cl::Env& e, uint32_t eax, uint32_t p1, uint32_t p2, uint32_t p3) {
    const uint32_t al = eax & 0xFF;
    if (al == 6) { e.H(0xCE0E, e.R(G(0x11973B)), p2, 0x60, 0); return; }
    const uint32_t ent = e.H(0xA4E20, al, 0, 0, 0, 0, p1 & 0x7F, p1, T(0x10A608 + (al << 9)));
    if (!ent) {
        const uint32_t pl = e.R(G(0x11BBFC));
        if (pl && e.R(pl) == 0 && e.R(pl + 0x14))
            for (uint32_t o = 0x150; o < 0x160; o += 4) e.W(e.R(pl + 0x14) + o, 0);
        Send(e, '0', p3 & 0xFF, p1);
        e.H(0x46A60, 0, 0, 0, 0, 0, 0, p1);
        return;
    }
    e.H(0xA4DA0, T(0x10A808), 0, 0, 0, 0, 0, 0, 0, T(0x46770));
    e.W(ent + 0xC4, e.R(ent + 0xC4) | 2);
    e.H(0xCE0E, e.R(G(0x11973B)), p2, 0x60, 0);
    e.H(0x471F0);
    Send(e, '/', al, p1);
    e.W(G(0xF46C8), 0x10); e.W(G(0xF46C4), 0x10);
    for (uint32_t a : { 0x11C3DCu, 0x11C3E0u, 0x11C3F8u, 0x11C3FCu, 0x11C2F8u, 0x11C2F4u }) e.W(G(a), 0);
    if (e.R8(G(0x119660))) {
        W8(e, G(0x119661), 1);
        e.H(0x46070, 0, 0, 0, 0, 0, ent);
        e.W(ent + 0xC4, e.R(ent + 0xC4) & ~2u);
        return;
    }
    const uint32_t n = e.R(G(0x113B10));
    if (!n) { Exit(e, 0x1060); return; }
    const uint32_t w = e.H(0xCE08, n, 0);
    const uint32_t id = e.R(ent + 4);
    if (e.H(0x1760, 0, 0, 0, 0, 0, id, 0, 9) && id != 0x219 && id != 0x21A && id != 0x21B && id != 0x20F)
        e.H(0xA5600, 0, 0, 0, 0, 0, ent);
    bool tail = e.H(0xC202, e.R(e.R(G(0x11BBFC)) + 0x60)) != 0;
    if (!tail) {
        W16(e, G(0x1197A6), 0);
        if ((w & 0xFFFF) != 0xFFFF) {
            e.H(0xBE350, 0, 0);
            W16(e, G(0x11966F), w);
            e.H(0x7D3C0, ent, w);
            e.H(0x45D30, 0, 0, 0, 0, 0, 0, w);
            W16(e, G(0x119671), e.H(0x45FF0, 0, 0, 0, 0, 0, 0, 0, 0, w));
            e.H(0x14910, 0, 0, 0, 0, 0, 0, e.R(ent + 0xC), w & 0xFFFF);
            W8(e, G(0x119661), 1);
            e.W(G(0x11BF94), G(0x4B230)); e.W(G(0x11BF98), 0);
            e.H(0x46070, 0, 0, 0, 0, 0, ent);
            e.W(G(0x1197A8), 0);
            return;
        }
        const uint32_t sv = e.H(0xCE2C, n, 0);
        if (e.R(ent + 4) != 0x20F) e.H(0xA5540);
        if (const uint32_t x = e.R(ent + 0x2C)) e.H(0xCDD8, x);
        if ((sv & 0xFFFF) != 0xFFFF) {
            e.H(0x8BBF0);
            e.H(0xBE350, 0, 0);
            W16(e, G(0x11966F), sv);
            W16(e, G(0x119671), e.H(0x45FF0, 0, 0, 0, 0, 0, 0, 0, 0, sv));
            e.H(0x4A8C0, ent);
            e.W(G(0x1197A8), 0);
            return;
        }
        const uint32_t id2 = e.R(ent + 4);
        if (!e.H(0x1760, 0, 0, 0, 0, 0, id2, 0, 9)) {
            const uint32_t edi = e.R(G(0x11971D));
            e.H(0x47B20); e.Ej(0x47B20, edi);
            e.H(0xC2790, 0, 0, 0, 0, 0, 0, 8, 1);
            e.W(G(0x1197A8), 0);
            return;
        }
        if (!(e.H(0x45C50, 0, 0, 0, 0, 0, id2) & 0xFF)) {
            e.H(0x8BBF0);
            e.H(0xBE350, 0, 0);
            e.H(0x4A4F0, ent, 0);
            e.W(G(0x1197A8), 0);
            return;
        }
    }
    e.W(G(0x119721), 1);
    e.W(G(0x11971D), e.R(ent + 0xC));
    e.W(G(0x119725), e.R(ent + 4));
    e.H(0x47B20); e.Ej(0x47B20, e.R(ent + 0xC));
    const uint32_t keep = e.R(G(0xFADA0));
    e.H(0xBF8C);
    if (e.R(G(0xDD5D0)) == 0x26) {
        e.W(G(0xFADA0), 0);
        if (!keep) return;
        e.H(0xBE2E0);
        e.H(0x492F0);
        return;
    }
    Exit(e, 0x139B);
}
