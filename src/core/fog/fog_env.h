// Callee seam for the faithful Fog ports. A port takes retail's real arguments (registers and stack words in
// retail order) plus a FogEnv: every call retail makes into another function (a direct callee, an import thunk, a function pointer
// read out of a struct) goes through fe_call with the callee's RVA and the registers/stack words retail loads, in the shape of the
// capture's `hook` recorder (eax ecx edx ebx esi, then stack words 0..6). The guest structs are 32-bit retail layouts read
// through the 32-bit guest address (mem is the host base of guest address 0; the probes use flat memory, base 0).
#pragma once
#include <cstdint>
#include <cstring>
struct FogEnv {
    void* ctx;
    uint32_t (*hook)(void* ctx, uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s,
                     uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5);
    uint32_t now;                                       // GetTickCount
    uint32_t lockCount;                                 // module global Fog+0x2FEE4 (entries of the per-bucket lock arrays)
    void (*enter)(void* ctx, uint32_t cs);              // EnterCriticalSection (stubbed, unlogged)
    void (*leave)(void* ctx, uint32_t cs);              // LeaveCriticalSection (stubbed, unlogged)
    void (*destroy)(void* ctx, uint32_t cs);            // DeleteCriticalSection (stubbed, unlogged)
};
inline uint8_t* fe_p(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
inline uint32_t fe_r32(uint32_t a) { uint32_t v; memcpy(&v, fe_p(a), 4); return v; }
inline uint32_t fe_r16(uint32_t a) { uint16_t v; memcpy(&v, fe_p(a), 2); return v; }
inline void fe_w32(uint32_t a, uint32_t v) { memcpy(fe_p(a), &v, 4); }
inline uint32_t fe_call(const FogEnv* e, uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0,
                        uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0, uint32_t s5 = 0) {
    return e->hook(e->ctx, rva, a, c, d, b, s, s0, s1, s2, s3, s4, s5);
}
// A varargs callee reached with more stack words than fe_call carries (a log writer handed a seven- or eight-word format): fe_wide(total, w6, w7)
// right before the fe_call states the word count and words 6 and 7; the recorder reads and clears them. 0 = not stated.
inline uint32_t fe_nw = 0, fe_x6 = 0, fe_x7 = 0;
inline void fe_wide(uint32_t total, uint32_t w6 = 0, uint32_t w7 = 0) { fe_nw = total; fe_x6 = w6; fe_x7 = w7; }
inline void fe_enter(const FogEnv* e, uint32_t cs) { if (e->enter) e->enter(e->ctx, cs); }
inline void fe_leave(const FogEnv* e, uint32_t cs) { if (e->leave) e->leave(e->ctx, cs); }
