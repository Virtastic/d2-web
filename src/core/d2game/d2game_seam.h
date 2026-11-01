// src/core/d2game/d2game_seam.h -- callee seam for ports transcribed from the 1.13c D2Game decompiles whose callees
// take their arguments in registers (EAX/ECX/EDX/EBX/ESI) plus stack words. One call shape for all of them: the
// callee's RVA names it, the registers and the first four stack words are passed as retail has them. The engine
// binds `call` to the real callee; the FN probes bind it to a recorder (the capture's `hook` directive).
#pragma once
#include <cstdint>
struct D2GameSeam {
    void* ctx;
    uint32_t (*call)(void* ctx, uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi,
                     uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
    uint32_t operator()(uint32_t rva, uint32_t eax = 0, uint32_t ecx = 0, uint32_t edx = 0, uint32_t ebx = 0,
                        uint32_t esi = 0, uint32_t s0 = 0, uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0) const
    { return call(ctx, rva, eax, ecx, edx, ebx, esi, s0, s1, s2, s3); }
};
