// 1.13c D2Launch+0xAF00  sub_AF00: game-list row refresh for the alternate (two-column) layout. Every callee is a seam taking the retail register
// layout; literals are image RVAs. g points at the module range [0x24DB0, 0x25EE8); inv and bm are the two bit-mask tables behind the imported Fog data
// pointers (IAT slots 0x1C2BC and 0x1C2C4). Rows alternate between the x offsets 0x25 and 0x135; the y step is taken after the second row of a pair.
#include <cstdint>
#include <cstring>

struct D2lAF00Seams {
    uint32_t (*o9b32)(uint32_t ecx);
    uint32_t (*o996a)(uint32_t ecx);
    uint32_t (*o9b2c)();
    void (*o98f8)(uint32_t ecx, uint32_t edx, uint32_t s0);
    void (*o9a30)(uint32_t s0, uint32_t s1); void (*o99d6)(uint32_t s0, uint32_t s1); uint32_t (*o99e8)(uint32_t s0, uint32_t s1);
    void (*o99be)(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s1, uint32_t s2);
    const char* (*o9a18)(uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
    void (*ctor)(void* p);
    void (*sprintf_)(void* dst, uint32_t fmtRva, uint32_t arg);
    void (*o98ce)(uint32_t ecx, const void* edx, uint32_t s0);
    void (*f9e40)(uint32_t eax, void* esi);
    void (*win2Unicode)(void* dst, const void* src, uint32_t n);
    void (*ustrcpy)(void* dst, uint32_t src); void (*ustrcat)(void* dst, const void* src);
    uint32_t (*time_)(void* p);
    void (*usprintf)(uint32_t n, void* buf, uint32_t fmt, uint32_t arg);
};

// Returns the EAX retail leaves at its single `ret`: W(0x25EB8) on the busy-list exit; with no first node, 0 (that word) when the
// layout word is 1, else 0x996A's answer when the scroll control exists, else the row count; after the walk, 0x25 when the last node
// was a visible row (the column test reloads it) or 0x99E8's answer when it was hidden.
extern "C" uint32_t d2l_af00(const D2lAF00Seams* s, uint8_t* g, const uint32_t* inv, const uint32_t* bm) {
    auto W = [&](uint32_t rva) -> uint32_t& { return *(uint32_t*)(g + (rva - 0x24DB0)); };
    auto U8 = [](uint32_t n, uint32_t o) -> uint32_t { return *(uint8_t*)(uintptr_t)(n + o); };
    auto U16 = [](uint32_t n, uint32_t o) -> uint32_t { return *(uint16_t*)(uintptr_t)(n + o); };
    auto U32 = [](uint32_t n, uint32_t o) -> uint32_t { return *(uint32_t*)(uintptr_t)(n + o); };
    char bb[100];
    uint16_t w1[100], w2[100], w3[10];
    uint32_t tmeta;
    uint32_t node = W(0x25EC8), y = W(0x25710) - 13, x = 0x25, idx = 0, rel, i;
    uint32_t eax = W(0x24DBC);
    if ((int32_t)W(0x24DBC) > 0) {
        i = 0;
        do {
            s->o996a(W(0x25734 + i * 4));
            uint32_t k = s->o9b2c() - 6;
            if (k <= 4 && k != 2) s->o98f8(W(0x25734 + i * 4), 0xFFFFFFFFu, k == 1 ? 0xFFFFFFF9u : 0xFFFFFFFEu);
            i++;
            eax = W(0x24DBC);
        } while ((int32_t)i < (int32_t)W(0x24DBC));
    }
    if (W(0x25EDC)) eax = s->o996a(W(0x25EDC));
    if (W(0x25AD4) == 1) { eax = W(0x25EB8); if (eax != 0) return eax; }
    if (!node) return eax;
    for (;;) {
        uint32_t base = W(0x25ED4), cnt = W(0x24DBC);
        if ((int32_t)idx < (int32_t)base || (int32_t)idx >= (int32_t)(base + cnt)) {
            eax = s->o99e8(U32(node, 0x338), 0);
        } else {
            eax = 0x25;
            rel = idx - base;
            s->o9a30(U32(node, 0x338), x + 0x1E);
            s->o99d6(U32(node, 0x338), y);
            uint32_t fl = U16(node, 0x324), m4 = fl & 4, mask;
            mask = (fl & 8) ? (bm[4] | 1) : (inv[4] & 1);
            mask = m4 ? (mask | bm[5]) : (mask & inv[5]);
            s->o99e8(U32(node, 0x338), mask);
            uint32_t ty = U32(node, 0x334);
            if (ty == 2) {
                s->o99be(W(0x25734 + rel * 4), s->o9b32(0x2B3B), 1, 0, 0);
            } else if (ty == 0) {
                for (i = 0; i < 100; i++) s->ctor(&w2[i]);
                bb[0] = 0;
                const char* nm = s->o9a18(U8(node, 0x320), (fl >> 8) & 0x1F, m4, fl & 0x20);
                if (nm) s->o98ce(W(0x25734 + rel * 4), nm, m4 ? 1 : 4);
                strcpy(bb + strlen(bb), (const char*)(uintptr_t)node);
                if (strlen((const char*)(uintptr_t)(node + 0x32A)) != 0) s->sprintf_(bb + strlen(bb), 0x1EB28, node + 0x32A);
                uint32_t ci = W(0x25734 + rel * 4);
                s->o98ce(ci, bb, m4 ? 1 : 4);
                s->f9e40(U8(node, 0x320), w2);
                for (i = 0; i < 100; i++) s->ctor(&w1[i]);
                for (i = 0; i < 10; i++) s->ctor(&w3[i]);
                s->sprintf_(bb, 0x1EB20, U16(node, 0x322));
                s->win2Unicode(w3, bb, 9);
                s->ustrcpy(w1, s->o9b32(0xFD9));
                s->ustrcat(w1, w3);
                s->ustrcat(w1, w2);
                s->o99be(ci, (uint32_t)(uintptr_t)w1, 0, 0, 0);
                if (fl & 0x20) s->o99be(ci, s->o9b32(0x58CB), 2, 0, 0);
                if (U8(node, 0x32E)) s->o99be(ci, s->o9b32(0x2AAF), 4, 0, 0);
                uint32_t exp = U32(node, 0x348), now = s->time_(&tmeta), days;
                if (exp > now) days = (uint32_t)(((uint64_t)(exp - now) * 0xC22E4507ull) >> 32) >> 16; else days = 0;
                if (W(0x25AD4) == 1 && days <= 0x3C) {
                    uint32_t flag = 1, id;
                    if (days == 0) id = 0x2B7E;
                    else if (days < 2) id = 0x2B7C;
                    else { id = 0x2B7D; if (days >= 0xF) flag = 0; }
                    s->usprintf(0x64, w1, s->o9b32(id), days);
                    s->o99be(ci, (uint32_t)(uintptr_t)w1, flag, 0, 0);
                }
            }
            if (x == 0x25) x = 0x135; else { x = 0x25; y += 0x5D; }
        }
        node = U32(node, 0x34C);
        idx++;
        if (!node) break;
    }
    return eax;
}
