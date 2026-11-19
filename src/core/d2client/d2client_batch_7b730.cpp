// src/core/d2client/d2client_batch_7b730.cpp -- twelve UNMAPPED 1.13c D2Client.dll rows (alpha lane),
// from build-wasm/ghidra-d2client/decomp/0x<RVA>.c. Base 0x6FAB0000; RVAs are retail's.
// Globals are parameters (caller reads the live value); unrecovered callees (ordinals into D2Win/D2Lang/
// D2Gfx, FUN_ helpers) are host callbacks taking the retail arguments. Node addresses are 32-bit guest values.
#include <cstdint>
#include <emscripten.h>

static inline uint32_t& W(uint32_t a, uint32_t off) { return *(uint32_t*)(uintptr_t)(a + off); }

// 1.13c D2Client+0x7B9C0 (118 B): push a fresh node (0x7B120) on the list at [0x11C3AB0]; at >= 0x80
// entries drop the tail first (0x7B330) and unlink it. EAX at return: the free's EAX on the drop path, else the node
// (the walk leaves the tail's null link in EAX only until the free call overwrites it).
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_7b9c0(uint32_t* head_c3ab0, uint16_t* count_c3ab4,   // count: the WORD at 0x113AB4 (cmp/dec/inc word ptr)
                         uint32_t (*alloc_7b120)(void), uint32_t (*free_7b330)(uint32_t tail_edi)) {
    uint32_t node = alloc_7b120(), eax = node;
    if (*count_c3ab4 >= 0x80 && *head_c3ab0 != 0) {
        uint32_t prev = 0, cur = *head_c3ab0;
        for (uint32_t nx = W(cur, 0x22); nx; nx = W(nx, 0x22)) { prev = cur; cur = nx; }
        eax = free_7b330(cur);
        if (prev) W(prev, 0x22) = 0;
        if (*count_c3ab4) (*count_c3ab4)--;
    }
    W(node, 0x22) = *head_c3ab0;
    (*count_c3ab4)++;
    *head_c3ab0 = node;
    return eax;
}

// retired, see d2client_7c380.cpp: D2Client 0x7C380.

// 1.13c D2Client+0x7C580 (57 B): if Ordinal_10612() != 0 { 0x7B3D0(); free(0x68a); Ordinal_10357(); }
// then Ordinal_10422(0).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_7c580(int32_t (*o10612)(void), void (*sub_7b3d0)(void), void (*free_)(uint32_t, uint32_t),
                     void (*o10357)(void), void (*o10422)(uint32_t)) {
    if (o10612() != 0) { sub_7b3d0(); free_(0x68a, 0); o10357(); }
    o10422(0);
}

// 1.13c D2Client+0x7C5C0 (135 B): current unit lookup. [0x11C3ACA] != 0 -> chain at table 0x11BA808[id&0x7f]
// with id [0x11C3AC2], required type 1; else if [0x11C3A80] != 0 -> table 0x11BAA08[id&0x7f] with id
// [0x11C3AC6], required type 2. Chain next at +0xE4, id at +0xC, type at +0. Type mismatch -> fatal 0x5a.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_7c5c0(uint32_t flag_c3aca, uint32_t flag_c3a80, uint32_t id_c3ac6, uint32_t id_c3ac2,
                         const uint32_t* tab_bba808, const uint32_t* tab_bbaa08, void (*fatal)(uint32_t code)) {
    uint32_t n, id, type;
    if (flag_c3aca == 0) {
        if (flag_c3a80 == 0) return 0;
        n = tab_bbaa08[id_c3ac6 & 0x7f]; id = id_c3ac6; type = 2;
        if (!n) return 0;
        while (W(n, 0xc) != id) { n = W(n, 0xe4); if (!n) return 0; }
    } else {
        id = id_c3ac2; type = 1;
        for (n = tab_bba808[id & 0x7f]; n && W(n, 0xc) != id; n = W(n, 0xe4)) {}
        if (!n) return 0;
    }
    if (W(n, 0) == type) return n;
    fatal(0x5a);
    return 0;
}

// retired, see d2client_7c770.cpp: D2Client 0x7C770.

// retired, see d2client_7c8f0.cpp: D2Client 0x7C8F0.

// 1.13c D2Client+0x7CA40: its own file, d2client_7ca40.cpp.
