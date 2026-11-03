// Shared bits of the 1.13c D2Client FN ports: retail 32-bit struct layouts read through
// fixed offsets, the Fog allocator thunks and the fatal-error path as an Env of callbacks the caller supplies.
#pragma once
#include <cstdint>
namespace d2cf {
inline uint32_t& W(uint32_t base, uint32_t off) { return *(uint32_t*)((uintptr_t)base + off); }
inline uint32_t R(uint32_t base, uint32_t off) { return *(const uint32_t*)((uintptr_t)base + off); }
inline uint8_t& B(uint32_t base, uint32_t off) { return *(uint8_t*)((uintptr_t)base + off); }
constexpr uint32_t kFileRva = 0xD4FA4;   // 0x6FB84FA4: the __FILE__ string every Fog_Free call site passes in EDX
struct Env {
    void (*freeMem)(uint32_t ptr, uint32_t line);        // 0xBF86: ECX = ptr, EDX = kFileRva, stack (line, 0), ret 8
    void (*fatal)(uint32_t line);                        // 0xBF92 + 0xBF80 + _exit(-1): never returns in retail
    // any other hooked callee, by module rva; register inputs a c d b s, stack words s0 s1; returns EAX
    uint32_t (*call)(uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s, uint32_t s0, uint32_t s1);
    // the same for a callee that takes four stack words (stdcall 4, ret 0x10)
    uint32_t (*call4)(uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3);
};
// The Gfx.cpp/Engine.h debug prologue shared by 0x6A000..0x6A2F0: #10175 (0xC65E, stdcall 1, ESI = unit) returns the unit kind,
// then #10350 (0xC658, stdcall 6) records (file, line). Returns the kind. Id slot = unit ? unit[1] : -1.
inline uint32_t gfxPro(const Env* e, uint32_t unit, uint32_t line) {
    uint32_t v = e->call(0xC65E, 0, 0, 0, 0, unit, unit, 0);
    e->call(0xC658, 0, 0, 0, 0, unit, line, 0);
    return v;
}
inline uint32_t unitId(uint32_t unit) { return unit ? R(unit, 4) : 0xFFFFFFFFu; }
}
