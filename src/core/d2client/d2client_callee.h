// D2Client ports: one injected callee dispatcher. A port that calls into another DLL (D2Win/D2Gfx/D2Lang/D2Common
// import thunks) or an internal D2Client function it does not own reaches it through this, keyed by the callee's RVA
// in D2Client.dll (1.13c). `r` holds EAX, ECX, EDX, EBX, ESI as the retail caller has them at the call, `s` the first
// six stack words it pushed (s[0] is the word nearest the return address). The result is EAX.
#pragma once
#include <cstdint>
typedef uint32_t (*D2cCallee)(void* ctx, uint32_t rva, const uint32_t* r, const uint32_t* s);
struct D2cSvc { void* ctx; D2cCallee call; };
static inline uint32_t d2c_call(const D2cSvc& v, uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t si,
                                uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0, uint32_t s5 = 0) {
    const uint32_t r[5] = { a, c, d, b, si }, s[6] = { s0, s1, s2, s3, s4, s5 };
    return v.call(v.ctx, rva, r, s);
}
