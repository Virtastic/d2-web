// 1.13c D2Client+0x6C130: the per-tick step of the timed-event list hung off
// ctx[+0x54][+0x2c]. EAX = ctx, no stack args, plain ret. Each pass stamps one unstamped node (+0x2c := tick), runs
// its kind arm, then restarts at the list head; the pass ends when every node carries this tick.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd(uint32_t a, uint32_t o) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
inline void wr(uint32_t a, uint32_t o, uint32_t v) { memcpy((void*)(uintptr_t)(a + o), &v, 4); }
}

struct D2cSeam6C130 {
    uint32_t (*tick)();                                   // kernel32!GetTickCount ([0xCEF5C])
    uint32_t (*curId)(uint32_t esiUnit);                  // 0x1AF0: ESI = ctx
    void (*unlink)(uint32_t ecxNode, uint32_t ctx);       // 0x69180: thiscall, ret 4
    void (*queue)(uint32_t eax, const uint32_t* st7);     // 0x6B300: EAX + 7 stack words, ret 0x1C
    uint32_t (*o10494)(uint32_t ctx, uint32_t v);         // thunk 0xC1FC (D2Common 10494), stdcall 2
    void (*cap)(uint32_t eaxCtx, uint32_t ebxNode, uint32_t owner);   // 0x689F0: EAX = ctx, EBX = node, 1 stack word, ret 4
    void (*fatal)();                                      // 0xBF92/0xBF80/0x3664: the "no list" error exit (never returns)
};

extern "C" void d2_client_6c130(uint32_t ctx, const D2cSeam6C130* s) {
    uint32_t own;
    if (!ctx || !(own = rd(ctx, 0x54))) { s->fatal(); return; }
    const uint32_t now = s->tick();
    for (;;) {
        uint32_t n = rd(own, 0x2c);
        while (n && rd(n, 0x2c) == now) n = rd(n, 0xa4);
        if (!n) return;
        wr(n, 0x2c, now);
        const uint32_t kind = rd(n, 0);
        bool kill = false, step = true;   // kill: 0x6C1F2 (unlink, restart); step: 0x6C1BB (advance +0x18)
        switch (kind) {
            case 0: kill = s->curId(ctx) != rd(n, 8); break;
            case 1: { uint32_t v = rd(n, 8); wr(n, 8, v - 1); kill = v == 0; break; }
            case 2: kill = rd(n, 8) == 0; break;
            case 6:
                if (rd(n, 8) == 0) {
                    if (s->tick() - rd(n, 0x30) < rd(n, 0x28)) { step = false; break; }
                    wr(n, 8, 1);
                }
                break;
            case 8:
                if (!s->o10494(ctx, rd(n, 0x38))) { kill = true; break; }
                if (rd(n, 0x3c)) {
                    int32_t c = (int32_t)rd(n, 8);
                    if (c < 0x32) wr(n, 8, (uint32_t)(c + 1)); else s->cap(ctx, n, own);
                }
                break;
            default: break;
        }
        if (kill) { s->unlink(n, ctx); continue; }
        if (!step) continue;
        const int32_t acc = (int32_t)(rd(n, 0x18) + rd(n, 0x14)), lim = (int32_t)rd(n, 0x1c);
        wr(n, 0x18, (uint32_t)acc);
        if (acc < lim) continue;
        switch (kind) {
            case 1: case 3: case 8: wr(n, 0x18, (uint32_t)(acc - lim)); break;
            case 2: wr(n, 8, 0); break;
            case 4: {
                const uint32_t a = rd(n, 8), b = rd(n, 0xc);
                s->unlink(n, ctx);
                const uint32_t st[7] = { ctx, 3, 0, 0, 0, 0, 0 };
                if (a != 0xFFFFFFFFu) s->queue(a, st);
                if (b != 0xFFFFFFFFu) s->queue(b, st);
                break;
            }
            case 5: wr(n, 0x14, 0); wr(n, 0x18, (uint32_t)(lim - 1)); wr(n, 8, 1); break;
            case 6: wr(n, 0x18, (uint32_t)(acc - lim)); wr(n, 8, 0); wr(n, 0x30, s->tick()); break;
            case 7: {
                s->unlink(n, ctx);
                const uint32_t st[7] = { ctx, 4, 0, 0x8d, 0x8e, 0, 0 };
                s->queue(0x8c, st);
                break;
            }
            case 9: {
                const uint32_t b = rd(n, 0xc), a = rd(n, 8), v = rd(n, 0x10);
                s->unlink(n, ctx);
                if (!s->o10494(ctx, v)) break;
                const uint32_t st[7] = { ctx, 8, 0, v, 0, 0, 0 };
                if (a != 0xFFFFFFFFu) s->queue(a, st);
                if (b != 0xFFFFFFFFu) s->queue(b, st);
                break;
            }
            default: break;
        }
    }
}
