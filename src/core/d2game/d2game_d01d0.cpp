// 1.13c D2Game+0xD01D0
// Interaction dispatch on a target unit by kind (caller 0x79B90). EAX = kind 0..5 (3 and above 5: refused), ECX = game,
// EDX = flag, stack (unit, target id), ret 8. Each kind looks the target up through 0xE03A0, measures the distance through
// 10407 and either runs the interaction or hands the refusal to 0xCE120. Returns 1 for a refused or missing target, 3 when the
// interaction stays open, else 0. Every callee is reached through the env with the registers and stack words retail hands it.
#include <emscripten.h>
#include <cstdint>
#include <cstring>

struct D2GameD01D0Env {
    uint32_t masksSlot;                                                  // address of the bit-mask table pointer (module global)
    uint32_t srcFile;                                                    // the source-file string handed to the fatal report
    uint32_t (*subE03A0)(uint32_t eax, uint32_t ecx, uint32_t edx);      // kind, game, id
    int32_t  (*oA520)(uint32_t);                                         // 10688
    uint32_t (*oA2DA)(uint32_t);                                         // cdecl
    void     (*oA2C2)(uint32_t, uint32_t);                               // cdecl
    void     (*o2A3C)(uint32_t);                                         // exit
    int32_t  (*oA538)(uint32_t, uint32_t);                               // 10407 distance
    int32_t  (*oAAA8)(uint32_t, uint32_t);
    int32_t  (*oAB1A)(uint32_t, uint32_t, uint32_t);
    void     (*oAB50)(uint32_t);
    int32_t  (*sub6A720)(uint32_t eax, uint32_t ecx, uint32_t s0, uint32_t s1);
    void     (*subCE120)(uint32_t eax, uint32_t ecx, uint32_t ebx, uint32_t esi, uint32_t s0);
    void     (*sub70D90)(uint32_t eax, uint32_t edi, uint32_t s0);
    int32_t  (*sub70D60)(uint32_t ecx, uint32_t s0);
    void     (*sub79910)(uint32_t eax, uint32_t ecx, uint32_t s0);
    void     (*subE1C00)(uint32_t eax, uint32_t esi, uint32_t s0);
    void     (*subD7160)(uint32_t eax, uint32_t ebx, uint32_t s0, uint32_t s1);
    void     (*subD6E30)(uint32_t s0, uint32_t s1, uint32_t s2);
    uint32_t (*sub1220)(uint32_t eax);
    void     (*sub8E6F0)(uint32_t eax, uint32_t edi);
    void     (*sub8EA80)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4);
    int32_t  (*sub37680)(uint32_t esi);
    uint32_t (*sub1370)(uint32_t eax);
    void     (*sub487A0)(uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
};

namespace {
inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
inline uint8_t rd8(uint32_t a, uint32_t o) { return ((const uint8_t*)(uintptr_t)a)[o]; }
inline void wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)a + o, &v, 4); }
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_D01D0(uint32_t op, uint32_t game, uint32_t flag, uint32_t unit, uint32_t id, const D2GameD01D0Env* e)
{
    const uint32_t ce = flag ? 0xFFFFFFFEu : 0xFFFFFFFFu;   // the refusal's last word
    switch (op) {
    case 0: {
        uint32_t t = e->subE03A0(0, game, id);
        if (!t) return 1;
        uint32_t d = (uint32_t)e->oA538(unit, t);            // unsigned compares on this kind
        if (d > 0x32) return 1;
        if (d > 8) { e->subCE120(game, unit, 0, id, ce); return 0; }
        if (rd32(t, 0x10) == 0x11 && !e->sub70D60(unit, game) && !e->sub70D60(t, game)) { e->sub79910(unit, game, t); return 0; }
        e->sub70D90(unit, t, game);
        return 0;
    }
    case 1: {
        uint32_t t = e->subE03A0(1, game, id);
        if (!t) return 1;
        int32_t d = e->oA538(unit, t);
        if (d > 0x32) return 1;
        uint32_t r = e->sub1220(rd32(t, 4));
        if (r) {
            uint32_t b = rd8(r, 0xD), m = rd32(e->masksSlot, 0);
            if ((rd32(m, 4) & b) && (rd32(m, 0) & b)) {
                e->oAB50(rd32(t, 0x2C));
                uint32_t p, q;
                if (rd32(t, 0) == 1 && (p = rd32(t, 0x14)) && (q = rd32(p, 0x28))) wr32(q, 0x14, 0x28);
                for (uint32_t n = rd32(t, 0xDC); n; ) {
                    uint32_t next = rd32(n, 0x24);
                    if (rd8(n, 0) == 2) e->sub8E6F0(n, game);
                    n = next;
                }
                e->sub8EA80(game, t, 2, rd32(game, 0xA8) + 1, 0, 0, 0);
            }
        }
        if (d > 6) {
            if (d >= 9) return 0;
            e->subCE120(game, unit, 1, id, ce);
            return 0;
        }
        if (!e->sub37680(unit)) {
            e->oAB50(rd32(unit, 0x2C));
            uint32_t k = e->sub1370(t);
            e->sub487A0(unit, game, t, k, 0);
        }
        return 0;
    }
    case 2: {
        uint32_t t = e->subE03A0(2, game, id);
        if (!t) return 1;
        if (!e->oA520(rd32(t, 4))) { uint32_t v = e->oA2DA(0x290); e->oA2C2(e->srcFile, v); e->o2A3C(0xFFFFFFFFu); }
        if ((int32_t)rd32(t, 0x10) >= 8) return 3;
        if (e->oA538(unit, t) > 0x32) return 1;
        if (e->oAAA8(unit, t) && !e->oAB1A(unit, t, 0x804)) {
            e->oAB50(rd32(unit, 0x2C));
            return e->sub6A720(2, unit, game, id) ? 0 : 3;
        }
        e->subCE120(game, unit, 2, id, ce);
        return 0;
    }
    case 4: {
        uint32_t t = e->subE03A0(4, game, id);
        if (!t || rd32(t, 0x10) != 3) return 1;
        int32_t d = e->oA538(unit, t);
        if (d > 0x32) return 1;
        if (d <= 4 && !e->oAB1A(unit, t, 0x804)) {
            uint32_t slot = id;                              // retail reuses the id argument slot as the out word
            if (flag) e->subD7160(unit, game, id, (uint32_t)(uintptr_t)&slot);
            else e->subD6E30(game, unit, (uint32_t)(uintptr_t)&slot);
            return slot ? 3 : 0;
        }
        e->subCE120(game, unit, 4, id, ce);
        return 0;
    }
    case 5: {
        uint32_t t = e->subE03A0(5, game, id);
        if (!t) return 1;
        int32_t d = e->oA538(unit, t);
        if (d > 0x32) return 1;
        if (d > 4) { e->subCE120(game, unit, 5, id, ce); return 0; }
        e->subE1C00(t, unit, game);
        return 0;
    }
    }
    return 1;
}
