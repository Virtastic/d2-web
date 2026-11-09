// 1.13c D2Game+0xE0D00: per-state unit setup/refresh. EAX = EDI value (passed on to the callees), ECX = ESI value, EDX = unit,
// stack: p1 (record whose +0x1C and +0xBC table are read), p2; ret 8. Addresses are 32-bit retail words.
// Every callee is a seam: call(rva, eax, esi, edi, s0..s5) returns what retail's callee leaves in EAX; eax/esi/edi are the register
// inputs of the callees that take them (zero otherwise), s0..s5 the stack words in push order (top of stack first).
#pragma once
#include <cstdint>
struct D2GameE0D00Env {
    uint32_t (*call)(uint32_t rva, uint32_t eax, uint32_t esi, uint32_t edi,
                     uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5);
    void (*fatal)(uint32_t code);   // Fog 10265/10024 then _exit(-1): never returns
};
uint32_t d2game_e0d00_port(uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t p1, uint32_t p2, const D2GameE0D00Env* e);
