// src/core/d2game/d2game_batch_71e10.cpp -- 1.13c D2Game.dll sub_71E10 (RE-DONE, alpha lane), from build-wasm/ghidra-d2game/decomp/0x71E10.c.
// PlrTrade.cpp commit step (caller 0x73D90): moves every tradable item of the offering unit's list into the
// other side's inventory, then post-processes both lists. in_EAX = receiving player (B), param_1 = ctx unit A, param_2
// = offering unit; unaff_retaddr = game (hash at +0x1720, 0x80 buckets, chain link +0xE4), unaff_DI = flag, unaff_ESI
// = notify flag. Ghidra shows several operands without a definition; where the decompile is ambiguous the seam
// carries the visible operands ("seam:") and the original value is not reconstructed.
#include <cstdint>
#include <cstring>

namespace {
inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
}

struct D2GameSub71E10Env {
    uint8_t* (*ptr)(uint32_t p);                           // 32-bit blob pointer -> host pointer
    uint8_t* (*o10920)(void* unit);                        // unit -> player data
    uint32_t (*o10460)(uint32_t inv);                      // first node
    uint32_t (*o10464)(uint32_t node);                     // next node
    uint32_t (*o11147)(uint32_t node);                     // node -> unit
    uint8_t  (*o10020)(void* unit);                        // unit/item type
    void*    (*f307E0)(void* item);                        // 0x307E0
    void     (*f90010)();                                  // 0x70010, seam: ambient args
    void     (*o11090)(void* item, int v);
    void     (*o10012)(void* item, int v);
    void     (*fF29F0)(void* playerB);                     // 0xD29F0
    uint32_t (*o10584)();                                  // seam: ambient args
    int      (*o10133)(uint32_t inv, void* item, uint32_t a, int b, uint8_t c, const char* file, int line);
    int      (*o10405)(uint32_t inv, void* item, int v);
    void     (*fF4670)(void* playerB);                     // 0xD4670
    int      (*o11057)(void* item);
    void     (*fF47C0)();                                  // 0xD47C0
    void     (*o10466)(uint32_t inv, int v);
    void     (*o10892)(void* item, int a, int b);
    int      (*fF1BC0)();                                  // 0xD1BC0
    void     (*o10977)(void* item, int a, int b);
    int      (*o10202)(void* item, int a, int line, const char* file);
    void     (*o10201)();
    void     (*o10050)(uint32_t inv, void* item);
    int      (*seamCond)(int32_t v);                       // seam: decompile compares iVar12 (==1) after 0x4000 clear
    void     (*f91B00)(void* thisp, void* playerB);        // 0x71B00
    void     (*fF51A0)(void* game, void* playerB);         // 0xD51A0
    int      (*fF5080)(void* game);                        // 0xD5080, tail check
    int      (*o10757)(void* p);
    int      (*o10508)(uint32_t inv);
    void     (*faa3e0)(int v);                             // 0xA3E0-style cleanup (FUN_6fcaa3e0, 2)
    void     (*o10758)(void* p, int a, int b);
    void     (*o11030)(void* unit, uint32_t v);
    void     (*fAC810)();                                  // 0x8C810
    uint32_t (*fF2D90)(void* game);                        // 0xD2D90
    void     (*fF2BE0)();                                  // 0xD2BE0
};

// 1.13c D2Game+0x71E10
uint32_t sub_71E10(D2GameSub71E10Env& e, uint8_t* playerB, uint8_t* unitA, uint8_t* offer, uint8_t* game, char flagDI, int notify) {
    uint8_t* src = offer;
    uint8_t* pd = (offer && rd32(offer, 0) == 0) ? e.o10920(offer) : nullptr;
    uint8_t flags = pd ? e.ptr(rd32(pd, 0x9c))[10] : 0;
    uint32_t node = e.o10460(rd32(src, 0x60));
    bool fail = false;
    while (node && !fail) {
        uint8_t* item = e.ptr(e.o11147(node));
        uint32_t next = e.o10464(node);
        node = next;
        if (e.o10020(item) != 2) continue;
        void* it = e.f307E0(item);
        if (!it) continue;
        uint8_t* p6 = (uint8_t*)it;
        e.f90010();
        e.o11090(p6, 4);
        e.o10012(p6, 0);
        uint32_t id = rd32(p6, 0xc);
        if (!game || !playerB) { fail = true; break; }
        e.fF29F0(playerB);
        uint8_t* h = e.ptr(rd32(game, 0x1720 + (id & 0x7f) * 4));
        while (h && rd32(h, 0xc) != id) h = e.ptr(rd32(h, 0xe4));
        if (!h) { fail = true; break; }
        p6 = h;
        if (rd32(p6, 0) != 4 || rd32(p6, 0x10) != 4) { fail = true; break; }
        uint8_t t = e.o10020(p6);
        uint32_t inv = rd32(playerB, 0x60);
        if (!inv) { fail = true; break; }
        uint32_t u = e.o10584();
        int nt = notify;
        if (rd32(playerB, 0) == 0 && flagDI == 1) nt = 0;
        if (!e.o10133(inv, p6, u, 0, t, "..\\Source\\D2Game\\PLAYER\\PlrTrade.cpp", 0x9cc) || !e.o10405(inv, p6, 1)) { fail = true; break; }
        if (e.o10020(p6) != 4) e.fF4670(playerB);
        wr32(p6, 0xc4, rd32(p6, 0xc4) & 0xfffffffd);
        if (e.o11057(p6)) e.fF47C0();
        if (nt) e.o10466(inv, 0);
        e.o11090(p6, 0);
        e.o10892(p6, 2, 1);
        if (e.fF1BC0()) e.o10977(p6, 1, 1);
        int r = e.o10202(p6, 0x4000, 0x8ef, "..\\Source\\D2Game\\ITEMS\\ItemMode.cpp");
        if (r) e.o10977(p6, 0x4000, 0);
        e.o10201();
        e.o10050(inv, p6);
        if (e.seamCond(r)) e.f91B00(nullptr, playerB);
        if (e.o11057(p6)) { e.fF51A0(game, playerB); e.o10201(); }
        if ((flags & 4) && e.fF5080(game) == 0) {   // seam: tail test on piVar5 (= flags&4) is ambiguous in the decompile
            e.faa3e0(2);
            e.faa3e0(2);
            return 0;
        }
    }
    if (fail) { e.faa3e0(2); return 0; }
    node = e.o10460(rd32(unitA, 0x60));
    while (node) {
        uint32_t nx = e.o10464(node);
        uint8_t* it = e.ptr(e.o11147(node));
        uint8_t t = e.o10020(it);
        node = nx;
        if (t == 1 || t == 2) {
            if (t == 2) e.fF2BE0();
            uint32_t z = 0;
            e.o10758(&z, 0x20, 1);
            e.o11030(it, 0);
            e.fAC810();
            e.fF2D90(game);
        }
    }
    return 1;
}
