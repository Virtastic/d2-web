// src/core/fog/fog_1d870.cpp -- 1.13c Fog+0x1D870 (AsyncData event-slot acquire), from build-wasm/ghidra-fog/decomp/0x1D870.c
// and the disassembly. State and seams: fog_asyncstate.h.
#include <cstdint>
#include <cstring>
#include "fog_asyncstate.h"

static uint32_t fog1d870_ld(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
static void fog1d870_st(uint8_t* p, uint32_t v) { std::memcpy(p, &v, 4); }

// 1.13c Fog+0x1D870
// Disabled table: null. Otherwise the first slot still marked free (busy word +4 non-zero) is taken (busy := 0);
// else the first slot whose event is signalled (WaitForSingleObject(event, 0) == 0) has its request (+8) completed
// through +0x1D740 and is taken; else, under the 0x400 ceiling, a new slot is appended (count first, then
// CreateEventA(0, 1, 0, 0) into it, busy 0). A full table returns null.
uint8_t* fog_sub_1d870(const FogAsyncApi* a, FogAsyncState* s) {
    if (!s->initialized) return nullptr;
    for (uint32_t i = 0; i < s->count; i++) {
        uint8_t* e = s->table + i * 12;
        if (fog1d870_ld(e + 4)) { fog1d870_st(e + 4, 0); return e; }
    }
    for (uint32_t i = 0; i < s->count; i++) {
        uint8_t* e = s->table + i * 12;
        if (a->waitZero(fog1d870_ld(e)) == 0) {
            a->complete(fog1d870_ld(e + 8));
            fog1d870_st(e + 4, 0);
            return e;
        }
    }
    if (s->count < 0x400) {
        uint8_t* e = s->table + s->count * 12;
        s->count++;
        fog1d870_st(e, a->createEvent());
        fog1d870_st(e + 4, 0);
        return e;
    }
    return nullptr;
}
