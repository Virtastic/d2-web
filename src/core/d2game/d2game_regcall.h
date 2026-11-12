// One retail D2Game callee reached in its 1.13c convention: the registers the callee reads (0 for the rest) and its
// stack words in argument order (word 0 is the one pushed last). Guest addresses are 32-bit words.
#pragma once
#include <cstdint>
#include <cstring>
#include <initializer_list>
struct D2GameRegCall {
    uint32_t (*call)(uint32_t rva, uint32_t eax, uint32_t ecx, uint32_t edx, uint32_t ebx, uint32_t esi, uint32_t edi,
                     const uint32_t* stk, uint32_t nstk);
};
namespace d2grc {
inline uint32_t r32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void w32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
inline void w16(uint32_t a, uint32_t v) { uint16_t x = (uint16_t)v; memcpy((void*)(uintptr_t)a, &x, 2); }
inline uint32_t r8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
// a callee that takes stack words only
inline uint32_t stk(const D2GameRegCall* e, uint32_t rva, std::initializer_list<uint32_t> w) { return e->call(rva, 0, 0, 0, 0, 0, 0, w.begin(), (uint32_t)w.size()); }
}
