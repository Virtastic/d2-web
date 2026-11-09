// 1.13c D2Client+0x7BB40 -- the credits / message scroll draw, written from the disassembly. stdcall(p) ret 4.
// p = scroll object: +0 head node, +4 count, +8 offset, +0xC speed, +0x1A last tick, +0x1E start tick, +0x22 rate, +0x26 accum, +0x2A u16 frames
// (all unaligned dwords); a node is {string, next}. tick = the value timeGetTime() answers (the import is pinned per tuple, so it is an argument here).
// g = pinned globals: +0 [0x113AF6] +4 [0x113AB6] +8 [0x113ABA] +0xC [0xE1384] +0x10 [0x11EC38] +0x14 [0x11BFA0].
// Draws through 0xD3A8 (font swap, ECX=8 then ECX=the saved font), 0xD0BA (frame, 6 words), 0xBE4C0 (EAX = a 4-dword rect on the stack),
// 0xD37E / 0xD3DE (ECX = node string, EDX = [0x11EC38]+0x10) and 0xD38A. A null p is fatal (0x709, never vectored).
// Returns 1 once the scroll is past its end; [0x11BFA0] is cleared on that return only when it was 1; otherwise 0.
#include "cl_env.h"
extern "C" uint32_t d2c_7bb40_r(const cl::Env& e, uint32_t p, uint32_t tick, uint32_t g) {
    auto fatal = [&](uint32_t id) { uint32_t m = e.H(0xbf92, id); e.H(0xbf80, 0xd359f, m); e.H(0x3664, 0xffffffffu); };
    if (!p) { fatal(0x709); return 0; }
    uint32_t saved = e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 8);
    uint32_t vis3ab6 = e.R(g + 4), vis3aba = e.R(g + 8);
    bool vis = vis3ab6 || vis3aba;
    if (e.R(p + 0x1e) == 0) {
        e.W(p + 0x1a, tick);
        e.W(p + 0x1e, tick);
    } else {
        uint32_t last = e.R(p + 0x1a), esi = (tick - e.R(p + 0x1e)) >> 2, ebx = 0;
        if (last == tick) {
            uint32_t rate = e.R(p + 0x22);
            if (rate == 0) e.W(p + 0x2a, (e.R(p + 0x2a) & 0xffff0000u) | (((e.R(p + 0x2a) & 0xffffu) + 1u) & 0xffffu));
            else { ebx = e.R(p + 0x26) + rate; e.W(p + 0x26, ebx); }
        } else {
            e.W(p + 0x26, 0);
            uint32_t fr = e.R(p + 0x2a) & 0xffffu;
            if (e.R(p + 0x22) == 0 && fr != 0) e.W(p + 0x22, (tick - last) / fr);
            e.W(p + 0x1a, tick);
        }
        e.W(p + 8, (uint32_t)((int32_t)(ebx + esi) * (int32_t)e.R(p + 0xc)));
    }
    uint32_t ec38 = e.R(g + 0x10), g91 = e.R(g + 0xc);
    if (e.R(g) == 0 && vis) {
        e.Xs(1);
        e.H(0xd0ba, ec38, g91 - 5, ec38 + 0x145, g91 + 0x75, 0);
        volatile uint32_t rect[4] = {ec38 - 1, g91 - 6, ec38 + 0x146, g91 + 0x75};
        uint32_t ra = (uint32_t)(uintptr_t)&rect[0];
        e.H(0xbe4c0, 0, 0, 0, 0, 0, ra);
        if (e.blk) e.blk(e.ctx, 0xbe4c0, ra, 4);
    }
    uint32_t cur = e.R(p), first = 0, last = 0, lastT = 0, firstT = 0, rem = e.R(p + 8);
    if (rem != 0 && cur != 0) {
        do {
            if (rem < 0x17800) {
                if (vis) e.H(0xd37e, g91 + (0x70 - (rem >> 10)), 0, 0, 0, 0, 0, e.R(cur), ec38 + 0x10);
                if (first == 0) { first = cur; firstT = rem; }
                lastT = rem;
                last = cur;
            }
            if (rem < 0x4800) break;
            cur = e.R(cur + 4);
            rem -= 0x4800;
        } while (cur != 0);
        if (first != 0 && e.R(p) != first) {
            uint32_t prev = e.R(p), nx = e.R(prev + 4);
            while (nx != first) { prev = nx; nx = e.R(prev + 4); }
            int32_t a = 0x5e - (int32_t)(firstT >> 10), b = a < 0x12 ? a : 0x12;
            if (vis) e.H(0xd3de, g91 + (uint32_t)a, 3, 0, (uint32_t)b, 0, 0, e.R(prev), ec38 + 0x10);
        }
        if (last != 0) {
            uint32_t nxt = e.R(last + 4);
            if (nxt != 0) {
                int32_t esi = 0x70 - (int32_t)((lastT + 0x400) >> 10), edi = esi - 0x5e;
                int32_t h = (int32_t)e.H(0xd38a);
                if (edi < h && vis) e.H(0xd3de, g91 + (uint32_t)esi + 0x13, 0, (uint32_t)edi, (uint32_t)(h - edi), 0, 0, e.R(nxt), ec38 + 0x10);
            }
        }
    }
    if (first == 0 && last == 0) {
        uint32_t ecx = e.R(p), nx = e.R(ecx + 4), edx = e.R(p + 8);
        while (nx != 0) { ecx = nx; nx = e.R(nx + 4); edx -= 0x4800; }
        if (edx < 0x1c000) {
            int32_t eax = 0x70 - (int32_t)((edx + 0x400) >> 10), w = 0x12;
            bool draw = true;
            if (eax < 0x12) { w = eax; if (eax == 0) draw = false; }
            if (draw && vis) e.H(0xd3de, g91 + (uint32_t)eax, 3, 0, (uint32_t)w, 0, 0, e.R(ecx), ec38 + 0x10);
        }
    }
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, saved);
    int32_t k = (int32_t)e.R(p + 4) - 1;
    if (k <= 0) k = 1;
    if ((e.R(p + 8) >> 10) <= (uint32_t)(k * 0x12 + 0x70)) return 0;
    if (e.R(g + 0x14) == 1) e.W(g + 0x14, 0);
    return 1;
}
