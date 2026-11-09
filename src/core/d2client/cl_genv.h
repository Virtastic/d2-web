// Module globals, kernel32 file imports and the shared fatal tail for the D2Client ports that touch the DLL's own data
// (the packet queue 0xABC20..0xAC630, Record.dr1 0xAB8D0, the save write 0xAC530, the stat packet 0xADB90).
// A port reaches a global by its retail RVA (G), hands a callee a module address as retail does (M), and calls a
// kernel32 file import by its IAT slot with retail's stack words (F); `buf` is where a read lands when sw[1] is an alias.
#pragma once
#include <cstdint>
#include <cstring>
#include "cl_env.h"
namespace cl {
struct GEnv {
    void* ctx = nullptr;
    uint32_t (*at)(void* ctx, uint32_t rva) = nullptr;    // address of the global at module RVA rva
    uint32_t (*mod)(void* ctx, uint32_t rva) = nullptr;   // the module address retail passes to a callee
    uint32_t (*file)(void* ctx, uint32_t slot, const uint32_t* sw, uint32_t buf) = nullptr;
    uint32_t G(uint32_t rva) const { return at(ctx, rva); }
    uint32_t M(uint32_t rva) const { return mod(ctx, rva); }
    uint32_t F(uint32_t slot, const uint32_t* sw, uint32_t buf = 0) const { return file(ctx, slot, sw, buf); }
};
inline uint32_t Stk(const void* p) { return (uint32_t)(uintptr_t)p; }
inline void Zero(uint32_t a, uint32_t n) { if (n) std::memset((void*)(uintptr_t)a, 0, n); }
inline void Copy(uint32_t d, uint32_t s, uint32_t n) { for (uint32_t i = 0; i < n; i++) ((uint8_t*)(uintptr_t)d)[i] = ((const uint8_t*)(uintptr_t)s)[i]; }   // rep movs: forward
inline uint32_t B(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
inline uint32_t W16(uint32_t a) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
inline void S16(uint32_t a, uint32_t v) { uint16_t x = (uint16_t)v; std::memcpy((void*)(uintptr_t)a, &x, 2); }
// the fatal tail of every D2Client check: 0xBF92(line) -> text, 0xBF80("%s" 0xD359F, text), 0x3664(-1) exit; never returns in retail
inline void Fatal(const Env& e, const GEnv& g, uint32_t line) {
    const uint32_t s = e.H(0xbf92, line);
    e.H(0xbf80, g.M(0xd359f), s);
    e.H(0x3664, 0xffffffffu);
}
}  // namespace cl
