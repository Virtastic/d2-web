// src/core/d2win/d2win_batch_8640.cpp -- five 1.13c D2Win.dll leaf rows transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2win/decomp/0x<RVA>.c). Alpha batch lane: implementation +
// marker only. Every null/type check that retail answers with Fog #10265 -> #10024("") -> _exit(-1)
// is d2win_fatal here. Control field offsets are the retail byte offsets.
#include <cstdint>
#include <unistd.h>
#include <emscripten.h>

[[noreturn]] static void d2win_fatal(uint32_t) { _exit(-1); }

// 1.13c D2Win+0x8640: clamp a list-box scroll position into its scrollbar (type 5) child.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_8640(int32_t* list, int32_t pos) {
    if (!list) d2win_fatal(0x596);
    if (list[0] != 4) d2win_fatal(0x597);
    int32_t* sb = (int32_t*)(uintptr_t)(uint32_t)list[0x24];   // 32-bit retail pointer slot: zero-extend, never sign-extend
    if (sb[0] != 5) d2win_fatal(0x65);
    if (sb[0x12] < pos) pos = sb[0x12];
    sb[0x13] = pos;
    return 1;
}

// 1.13c D2Win+0x7F40: set the colour word (+0x14) of list entry `index`, or of all entries for -1.
// Entries are singly linked through +0x1C from the list head at control+0x48.
struct D2WinListEntry { uint8_t pad[0x14]; uint32_t color; uint8_t pad2[4]; uint32_t next; };   // 32-bit retail pointer slots
struct D2WinListCtl { int32_t type; uint8_t pad[0x44]; uint32_t head; uint32_t tail; uint32_t unk; int32_t count; };
static D2WinListEntry* d2win_ent(uint32_t p) { return (D2WinListEntry*)(uintptr_t)p; }
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2win_sub_7F40(D2WinListCtl* c, int32_t index, uint32_t color) {
    if (!c) d2win_fatal(0x368);
    if (c->type != 4) d2win_fatal(0x369);
    if (index == -1) {
        for (D2WinListEntry* e = d2win_ent(c->head); e; e = d2win_ent(e->next)) e->color = color;
        return 1;
    }
    if (c->count < index) d2win_fatal(0x378);
    int32_t i = 0;
    D2WinListEntry* e = d2win_ent(c->head);
    if (index > 0) {
        while (i < index) {
            D2WinListEntry* n = d2win_ent(e->next);   // e is non-null here: count >= index
            if (!n) break;
            ++i; e = n;
        }
    }
    if (!e) d2win_fatal(0x382);
    if (i != index) d2win_fatal(899);
    e->color = color;
    return 1;
}

// 1.13c D2Win+0xB3E0: store a dword at control+0x14.
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_B3E0(uint8_t* ctl, uint32_t v) {
    if (!ctl) d2win_fatal(0xD76);
    *(uint32_t*)(ctl + 0x14) = v;
}

// 1.13c D2Win+0x178E0: set (on != 0) or clear control+8 bit 2 (gdwBitMasks[2] / gdwInvBitMasks[2]).
extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_178E0(uint8_t* ctl, int32_t on) {
    if (!ctl) d2win_fatal(0x33B);
    uint32_t* w = (uint32_t*)(ctl + 8);
    if (on) *w |= 1u << 2; else *w &= ~(1u << 2);
}

// 1.13c D2Win+0xBB50: honorific prefix. title = param_2 (1..6), class = param_1, hardcore = param_3.
extern "C" EMSCRIPTEN_KEEPALIVE const char* d2win_sub_BB50(int32_t cls, int32_t title, int32_t hc) {
    bool f = cls == 0 || cls == 1 || cls == 6 || cls == 9;
    if (title < 1 || title > 6) return "";
    static const char* const std_m[] = {"", "Sir ", "Lord ", "Baron ", "Count ", "Duke ", "King "};
    static const char* const std_f[] = {"", "Dame ", "Lady ", "Baroness ", "Countess ", "Duchess ", "Queen "};
    static const char* const hc_m[] = {"", "Slayer ", "Champion ", "Patriarch ", "Destroyer ", "Conqueror ", "Guardian "};
    static const char* const hc_f[] = {"", "Slayer ", "Champion ", "Matriarch ", "Destroyer ", "Conqueror ", "Guardian "};
    return hc ? (f ? hc_f : hc_m)[title] : (f ? std_f : std_m)[title];
}
