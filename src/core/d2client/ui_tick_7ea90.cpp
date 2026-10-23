// D2Client rva 0x7EA90: per-frame UI/speech tick. EAX = arg0 (rect source).
// Globals are one flat image `g` indexed by module RVA - G_BASE; callees injected.
#include <cstddef>
#include <cstdint>
#include <cstring>

struct D2Client7EA90Callees {
    void (*clearList)(uint32_t tableRva);                   // 0xA4DA0
    uint32_t (*tick)(uint32_t owner);                       // 0x7E970 (owner -> iVar1)
    void (*c4910)(uint32_t a, uint32_t id);                 // 0x14910 (decomp FUN_6fac4910)
    void (*c46a60)(uint32_t id);                            // 0x46A60 (FUN_6faf6a60)
    void (*c7e930)();                                       // 0x7E930
    uint32_t (*winHandle)();                                // 0xD0B4
    void (*msg3cc60)(uint32_t win, uint32_t str);           // 0x3CC60
    void (*fatal)(uint32_t code);                           // 0xBF92/0xBF80 error text then _exit(-1)
    void (*c7bad0)(uint32_t a);                             // 0x7BAD0
    void (*c7c650)();                                       // 0x7C650
    void (*c7b730)();                                       // 0x7B730
    void (*c7b580)();                                       // 0x7B580
    void (*c3664)();                                        // 0x53840 (gated by 0x113A70)
    uint32_t (*peek)(uint32_t addr);                        // read a dword of module memory
};
enum : uint32_t {
    R_CNT = 0x113ACE, R_RECT = 0x113AD4, R_A80 = 0x113A80, R_Q = 0x113960, R_EC38 = 0x11EC38, R_1384 = 0xE1384,
    R_AE4 = 0x113AE4, R_OWNER = 0x113A68, R_ACA = 0x113ACA, R_AC2 = 0x113AC2, R_TBL = 0x10A808, R_BBFC = 0x11BBFC,
    R_A84 = 0x113A84, R_A88 = 0x113A88, R_A64 = 0x113A64, R_A60 = 0x113A60, R_3448 = 0x123448, R_ABA = 0x113ABA,
    R_A7C = 0x113A7C, R_ADE0 = 0xFADE0, R_A70 = 0x113A70, R_MSG = 0xE1388,
};
static uint32_t gr(const uint8_t* g, uint32_t r) { uint32_t v; memcpy(&v, g + r, 4); return v; }
static void gw(uint8_t* g, uint32_t r, uint32_t v) { memcpy(g + r, &v, 4); }

// 1.13c D2Client+0x7EA90  (a = EAX struct: +4,+8,+0xC,+0x10,+0x24,+0x28 dwords; g sized >= 0x124000)
extern "C" void d2_client_7ea90(const uint8_t* a, uint8_t* g, uint32_t now, const D2Client7EA90Callees& k) {
    gw(g, R_CNT, (gr(g, R_CNT) & 0xFFFF0000u) | ((gr(g, R_CNT) + 1) & 0xFFFF));
    const int32_t l = (int32_t)gr(a, 0x24), t = (int32_t)gr(a, 0x28);
    gw(g, R_RECT, l); gw(g, R_RECT + 4, t);
    gw(g, R_RECT + 8, gr(a, 0xC) - gr(a, 4) + l); gw(g, R_RECT + 12, t - gr(a, 8) + gr(a, 0x10));
    if (gr(g, R_A80) == 1) {
        gw(g, R_Q, gr(g, R_EC38)); gw(g, R_Q + 8, gr(g, R_EC38) + 0x145);
        gw(g, R_Q + 4, gr(g, R_1384)); gw(g, R_Q + 12, gr(g, R_1384) + 0x70);
    }
    gw(g, R_AE4, gr(g, R_A80) == 1);
    k.clearList(0x10A608); k.clearList(0x10A808); k.clearList(0x10AA08);
    uint32_t it = 0, node = 0;
    const uint32_t owner = gr(g, R_OWNER);
    if (gr(g, R_A80) == 1 && owner != 0 && (k.peek(owner + 0x16) == 1 || k.peek(owner + 0x16) == 0)) {
        it = k.tick(owner);
        if (gr(g, R_ACA) != 0) {
            const uint32_t id = gr(g, R_AC2) & 0xFFFF;
            for (node = gr(g, R_TBL + 4 * (id & 0x7f)); node != 0; node = k.peek(node + 0xE4)) {
                if (k.peek(node + 0xC) == id) {
                    if (k.peek(node) != 1) { k.fatal(0x5A); return; }
                    break;
                }
            }
        }
        const uint32_t bb = gr(g, R_BBFC);
        if (bb == 0 || k.peek(bb + 0x10) == 0x11 || (gr(g, R_ACA) != 0 && node != 0 && k.peek(node + 0x10) == 0xC)) {
            if (gr(g, R_ACA) != 0) { k.c4910(1, gr(g, R_AC2) & 0xFFFF); k.c46a60(gr(g, R_AC2) & 0xFFFF); }
            k.c7e930();
            if (gr(g, R_3448) == 1) { gw(g, R_3448, 0); k.msg3cc60(k.winHandle(), R_MSG); }
        }
    }
    if (gr(g, R_A84) != 0) {
        if (gr(g, R_A88) == 0) { k.fatal(0xCFE); return; }
        const bool live = now <= gr(g, R_A64);
        if (live) k.c7bad0(gr(g, R_A60));
        else { k.c7c650(); gw(g, R_A84, 0); gw(g, R_ACA, 0); }
        it += live;
    }
    if (it == 0 && gr(g, R_A80) == 1) {
        gw(g, R_ABA, 0); gw(g, R_ACA, 0); gw(g, R_A7C, 1); gw(g, R_A80, 0);
        if (gr(g, R_3448) == 1) { gw(g, R_3448, 0); k.msg3cc60(k.winHandle(), R_MSG); }
    }
    gw(g, R_A7C, 0);
    if (gr(g, R_ADE0) == 0) { k.c7b730(); k.c7b580(); }
    if (gr(g, R_A70) != 0) k.c3664();
}
