// src/core/d2client/d2client_batch_tiny_1019l.cpp -- five UNMAPPED 1.13c D2Client.dll rows
// reimplemented from the decompiles in build-wasm/ghidra-d2client/decomp/0x<RVA>.c (alpha lane).
// Base 0x6FAB0000; every RVA below is retail's. Globals are parameters: the caller reads the live
// value. Unrecovered callees are host callbacks, after the retail arguments.
#include <cstdint>
#include <emscripten.h>

struct Client14660State;   // defined in packet_69.cpp (D2Client+0x14660)

extern "C" void d2_client_14660(Client14660State* g, uint32_t obj,
                                void (*send)(uint32_t, uint32_t, uint32_t), void (*call90)(uint32_t));

// 1.13c D2Client+0x81C90 (31 B): run 0x14660 (its ECX argument is not part of the 0x69 payload), then
// [0xF8C9C] = 0, [0x11C404] = 1, [0x11C3A4] = 1.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_81c90(Client14660State* g, uint32_t obj,
                     void (*send)(uint32_t, uint32_t, uint32_t), void (*call90)(uint32_t),
                     uint32_t* f8c9c, uint32_t* c404, uint32_t* c3a4) {
    d2_client_14660(g, obj, send, call90);
    *f8c9c = 0;
    *c404 = 1;
    *c3a4 = 1;
}

// 1.13c D2Client+0x88D00 (28 B): [0x11BF40] = ([0x11BF40] == 0); when the new value is 0, tail-call
// 0x886F0 (rebuild the frame list).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_88d00(uint32_t* toggle_11bf40, void (*sub_886F0)(void)) {
    *toggle_11bf40 = (*toggle_11bf40 == 0) ? 1u : 0u;
    if (*toggle_11bf40 == 0)
        sub_886F0();
}

// 1.13c D2Client+0x8BEE0 (26 B): return [0x11B9A0] + 0x113 when [0x119854] is 0, else +0x110.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_8bee0(uint32_t base_11b9a0, uint32_t flag_119854) {
    return base_11b9a0 + (flag_119854 == 0 ? 0x113u : 0x110u);
}

// 1.13c D2Client+0xA4D40 (29 B): no local unit -> true; otherwise true only when
// *(uint32_t*)(unit + 0x10) == 0x11.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_a4d40(uint32_t unit_11bbfc) {
    if (unit_11bbfc == 0)
        return 1;
    return *(const uint32_t*)(uintptr_t)(unit_11bbfc + 0x10) == 0x11 ? 1u : 0u;
}

// 1.13c D2Client+0xAD220 (31 B): packed record at ECX: id = [rec+1], a = [rec+5], b = [rec+9]. Calls
// 0x9D630(ECX = rec, EDX = id); on a hit writes [hit+0x28] = a (reloaded after the call) and
// [hit+0x2C] = b (loaded before it).
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_client_ad220(uint32_t rec, int32_t (*lookup_9d630)(uint32_t rec, int32_t id)) {
    uint32_t b = *(const uint32_t*)(uintptr_t)(rec + 9);
    int32_t hit = lookup_9d630(rec, *(const int32_t*)(uintptr_t)(rec + 1));
    if (hit != 0) {
        *(uint32_t*)(uintptr_t)((uint32_t)hit + 0x28) = *(const uint32_t*)(uintptr_t)(rec + 5);
        *(uint32_t*)(uintptr_t)((uint32_t)hit + 0x2C) = b;
    }
}
