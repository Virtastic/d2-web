// Replay probe for the D2Launch BNET disconnect routine.
// The port is d2l_e250_core in src/core/d2launch/d2launch_n31.cpp (the port with raw seams).
// No arguments. Blobs: lg r[14] hook log, st r[16] (state block, written in place), vt r[18], iat r[20], eb0 r[22] (gvar, in and out), g r[24] (+0 = 1: interface present).
#include <emscripten.h>
#include <cstdint>
#include <cstring>
struct D2lE250Seams { void (*debugOut)(const char*); void (*o10018)(); void (*vtDisc)(uint32_t); };
extern "C" void d2l_e250_core(int32_t* eb0, uint8_t* ctx, uint32_t hasBn, const D2lE250Seams* sk);
static uint32_t* s_lg;
static void rec(uint32_t rva, uint32_t eax, uint32_t s0) {
    uint32_t* w = s_lg + 1 + s_lg[0]++ * 10;
    const uint32_t v[10] = { rva, eax, 0, 0, 0, 0, s0, 0, 0, 0 };
    for (int i = 0; i < 10; ++i) w[i] = v[i];
}
static void dbg(const char*) { rec(0x9778, 0, 0xB2000000u | 0x1ea30); }   // "Disconnect from BNET\n" in .rdata
static void mcp() { rec(0x9aae, 0, 0); }
static void vt(uint32_t) { rec(0x9754, 0xB1020000u, 0); }                // EAX = the interface blob (index 2, offset 0)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2launch_e250(uint32_t* r) {
    s_lg = (uint32_t*)(uintptr_t)r[14];
    uint8_t* ctx = (uint8_t*)(uintptr_t)r[16];
    uint32_t vtp; std::memcpy(&vtp, (void*)(uintptr_t)r[24], 4);
    int32_t eb0; std::memcpy(&eb0, (void*)(uintptr_t)r[22], 4);
    static const D2lE250Seams S = { dbg, mcp, vt };
    d2l_e250_core(&eb0, ctx, vtp ? 1u : 0u, &S);
    std::memcpy((void*)(uintptr_t)r[22], &eb0, 4);
    r[4] = 0;
}
