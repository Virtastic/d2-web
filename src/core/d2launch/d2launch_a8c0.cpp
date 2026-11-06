// 1.13c D2Launch+0xA8C0  sub_A8C0: game-list selection refresh (shows or hides the detail widgets for the selected row and rebuilds the detail copy of the
// row node). Every callee is a seam taking the retail register layout; literals are image RVAs. g points at the module range [0x24DB0, 0x25EE8); inv and bm
// are the two bit-mask tables behind the imported Fog data pointers (IAT slots 0x1C2BC and 0x1C2C4). When the exit seam returns, as retail's would not,
// the code falls through to the show calls with a zero handle, as the instruction stream does.
#include <cstdint>
#include <cstring>

struct D2lA8C0Seams {
    uint32_t (*o980e)(); void (*f9bf0)();
    void (*o9862)(uint32_t ecx, uint32_t edx); void (*o987a)(uint32_t ecx, uint32_t edx); void (*o996a)(uint32_t ecx);
    void (*o9a1e)(uint32_t s0, uint32_t s1, uint32_t s2);
    void (*o9a24)(uint32_t s0, uint32_t s1);
    void (*o97fc)(uint32_t ptr, uint32_t fileRva, uint32_t s0, uint32_t s1);
    uint32_t (*o97de)(uint32_t size, uint32_t fileRva, uint32_t s0, uint32_t s1);
    void (*o977e)(uint32_t dst, uint32_t src, uint32_t n);
    void (*o99be)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2);
    uint32_t (*o9b32)(uint32_t ecx);
    uint32_t (*o9e10)(uint32_t eax);
    uint32_t (*o99ca)(uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
    uint32_t (*o9820)(uint32_t id); void (*o97f6)(uint32_t fmtRva, uint32_t a, uint32_t b); void (*exit_)(uint32_t code);
    void (*o9a0c)(uint32_t s0, uint32_t s1); void (*o9a06)(uint32_t s0, uint32_t s1); void (*o9a2a)(uint32_t ecx, uint32_t edx);
    void (*o9a30)(uint32_t s0, uint32_t s1); void (*o99d6)(uint32_t s0, uint32_t s1); void (*o99e8)(uint32_t s0, uint32_t s1);
    void (*ctor)(void* p);
    void (*sprintf4)(void* dst, uint32_t fmtRva, uint32_t a, uint32_t b);
    void (*o98ce)(uint32_t ecx, uint32_t edx, uint32_t s0);
    void (*f9e40)(uint32_t eax, void* esi);
    void (*sprintf3)(void* dst, uint32_t fmtRva, uint32_t a);
    void (*win2Unicode)(void* dst, const void* src, uint32_t n);
    void (*ustrcpy)(void* dst, uint32_t src); void (*ustrcat)(void* dst, const void* src);
};

extern "C" uint32_t d2l_a8c0(const D2lA8C0Seams* s, uint8_t* g, const uint32_t* inv, const uint32_t* bm) {
    auto W = [&](uint32_t rva) -> uint32_t& { return *(uint32_t*)(g + (rva - 0x24DB0)); };
    auto U8 = [](uint32_t n, uint32_t o) -> uint32_t { return *(uint8_t*)(uintptr_t)(n + o); };
    auto U16 = [](uint32_t n, uint32_t o) -> uint32_t { return *(uint16_t*)(uintptr_t)(n + o); };
    auto U32 = [](uint32_t n, uint32_t o) -> uint32_t& { return *(uint32_t*)(uintptr_t)(n + o); };
    uint16_t w1[100], w2[100], w3[10];
    char bb[100];
    uint32_t sel, node, idx, cur, h, fl, mask, i;
    int32_t rel;
    if (s->o980e()) { s->f9bf0(); return 1; }
    if (W(0x25ED0) == 0) W(0x24DB0) = 0;
    sel = W(0x24DB0);
    rel = (int32_t)sel - (int32_t)W(0x25ED4);
    s->o9862(W(0x25708), ((int32_t)W(0x25ED0) > 0 && (int32_t)sel > (int32_t)W(0x24DB4) && sel != W(0x24DB8)) ? 1 : 0);
    s->o9862(W(0x25980), 1);
    if (rel >= 0 && rel < (int32_t)W(0x24DBC)) {
        if (sel == W(0x24DB4) && W(0x25EE4) != 0) {
            s->o9a1e(W(0x25990), 0x202, W(0x25710 + rel * 4));
            s->o987a(W(0x25990), 1);
            s->o987a(W(0x25AB0), 0);
            goto after_shows;
        }
        s->o9a1e(W(0x25AB0), 0x202, W(0x25710 + rel * 4));
        s->o987a(W(0x25AB0), 1);
    } else {
        s->o987a(W(0x25AB0), 0);
    }
    s->o987a(W(0x25990), 0);
after_shows:
    s->o987a(W(0x25730), (int32_t)W(0x25ED4) > 0 ? 1 : 0);
    s->o987a(W(0x2587C), ((int32_t)(W(0x24DBC) + W(0x25ED4)) < (int32_t)W(0x25ED0)) ? 1 : 0);
    node = W(0x25EC8);
    if (!node) return 1;
    idx = 0;
    while (W(0x24DB0) != idx) {
        node = U32(node, 0x34C);
        idx++;
        if (!node) return 1;
    }
    if (!node) return 1;
    if (W(0x25ECC)) {
        if (U32(W(0x25ECC), 0x334) == 0) s->o9a24(U32(W(0x25ECC), 0x338), 1);
        s->o97fc(W(0x25ECC), 0x1EB84, 0x5F3, 0);
        W(0x25ECC) = 0;
    }
    W(0x25ECC) = s->o97de(0x350, 0x1EB84, 0x5F7, 0);
    cur = W(0x25ECC);
    s->o977e(cur, node, 0x350);
    U32(cur, 0x34C) = 0;
    if (U32(node, 0x334) == 1) {
        s->o996a(W(0x25878));
        s->o99be(W(0x25878), s->o9b32(0x1419), 1, 0, 0);
        s->o996a(W(0x25704)); s->o996a(W(0x25988));
        return 1;
    }
    if (U32(node, 0x334) == 2) {
        s->o996a(W(0x25878));
        s->o99be(W(0x25878), s->o9b32(0x2B3B), 1, 0, 0);
        s->o996a(W(0x25704)); s->o996a(W(0x25988));
        return 1;
    }
    {
        uint32_t b = U8(cur, 0x320), esi = 5;
        fl = U16(cur, 0x324);
        if (fl & 4) {
            if (fl & 8) b = (s->o9e10(b) != 0) + 8;
            else if (s->o980e() != 0 || U8(W(0x25ECC), 0x320) < 5) esi = 1;
        }
        cur = W(0x25ECC);
        h = s->o99ca(b & 0xFF, esi, cur + 0x300, cur + 0x310);
        U32(cur, 0x338) = h;
        if (h == 0) {
            for (i = 0; i < 16; i++) { *(uint8_t*)(uintptr_t)(cur + 0x300 + i) = 0; *(uint8_t*)(uintptr_t)(cur + 0x310 + i) = 0xFF; }
            *(uint8_t*)(uintptr_t)(cur + 0x301) = 1;
            h = s->o99ca(7, 5, cur + 0x300, cur + 0x310);
            U32(cur, 0x338) = h;
        }
        if (h == 0) {
            uint32_t v = s->o9820(0x62E); s->o97f6(0x1E256, v, 0x62E); s->exit_(0xFFFFFFFFu);
        }
    }
    s->o9a0c(h, 0);
    s->o9a06(U32(W(0x25ECC), 0x338), 0);
    s->o9a2a(U32(W(0x25ECC), 0x338), 5);
    s->o9a30(U32(W(0x25ECC), 0x338), 0x118);
    s->o99d6(U32(W(0x25ECC), 0x338), 0x172);
    cur = W(0x25ECC);
    fl = U16(cur, 0x324);
    mask = (fl & 8) ? (bm[4] | 5) : (inv[4] & 5);
    mask = (fl & 4) ? (mask | bm[5]) : (mask & inv[5]);
    mask = (fl & 0x20) ? (mask | bm[12]) : (mask & inv[12]);
    s->o99e8(U32(cur, 0x338), mask);
    for (i = 0; i < 100; i++) s->ctor(&w2[i]);
    s->o996a(W(0x25878));
    {
        uint32_t edx = cur;
        if (strlen((const char*)(uintptr_t)(cur + 0x32A)) != 0) { s->sprintf4(bb, 0x1EB18, cur, cur + 0x32A); edx = (uint32_t)(uintptr_t)bb; }
        s->o98ce(W(0x25878), edx, 0);
    }
    s->o996a(W(0x25704));
    s->f9e40(U8(W(0x25ECC), 0x320), w2);
    for (i = 0; i < 100; i++) s->ctor(&w1[i]);
    for (i = 0; i < 10; i++) s->ctor(&w3[i]);
    s->sprintf3(bb, 0x1EB20, U16(node, 0x322));
    s->win2Unicode(w3, bb, 9);
    s->ustrcpy(w1, s->o9b32(0xFD9));
    s->ustrcat(w1, w3);
    s->ustrcat(w1, w2);
    s->o99be(W(0x25704), (uint32_t)(uintptr_t)w1, 0, 0, 0);
    s->o996a(W(0x25988));
    {
        uint32_t edx = W(0x25ECC) + 0x100;
        if (U16(edx, 0) == 0) edx = s->o9b32(0x142C);
        s->o99be(W(0x25988), edx, 0, 0, 0);
    }
    if (U8(node, 0x324) & 0x20) s->o9862(W(0x25980), 0);
    return 1;
}
