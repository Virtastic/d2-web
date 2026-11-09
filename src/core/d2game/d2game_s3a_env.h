// Seams shared by the ports in d2game_<rva>.cpp written with the retail register and stack order of each callee.
// call(): D2Game+rva with EAX, ECX, EDX, EBX, ESI and n stack words (word 0 = the last word pushed). Retail structs are 32-bit images
// (rd32/wr32); a pointer stored in a field goes through h()/p() (host <-> image handle); addr(rva) is the image handle of D2Game+rva.
#pragma once
#include <cstdint>
#include <cstring>
#include <initializer_list>
struct D2GameS3aEnv {
    uintptr_t (*call)(uint32_t rva, uintptr_t eax, uintptr_t ecx, uintptr_t edx, uintptr_t ebx, uintptr_t esi, const uint32_t* st, uint32_t n);
    void      (*iter)(uint32_t cbRva, uint32_t edi, uint32_t s0);   // 0xDFCC0: EAX = 0, EBX = D2Game+cbRva, EDI = edi, one stack word
    uint32_t  (*addr)(uint32_t rva);
    uint32_t  (*h)(uintptr_t host);
    uintptr_t (*p)(uint32_t handle);
    void      (*edi)(uint32_t v);   // EDI for the next call() (a callee that takes an EDI argument); null = not modelled
    void      (*blk)(const void* p, uint32_t n);   // the n-dword block the next call() is handed (logged after it, as `hookblk`); null = not modelled
};
namespace s3a {
inline uint32_t rd32(uintptr_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
inline void     wr32(uintptr_t p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
inline uint8_t  rd8(uintptr_t p, uint32_t o) { return ((const uint8_t*)p)[o]; }
inline void     wr8(uintptr_t p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
inline uint16_t rd16(uintptr_t p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
inline void     wr16(uintptr_t p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
inline uintptr_t clE(const D2GameS3aEnv* e, uint32_t edi, uint32_t rva, uintptr_t a, uintptr_t c, uintptr_t d, uintptr_t b, uintptr_t s, std::initializer_list<uint32_t> st = {}) {
    uint32_t w[8] = {}; uint32_t n = 0; for (uint32_t v : st) if (n < 8) w[n++] = v;
    if (e->edi) e->edi(edi);
    return e->call(rva, a, c, d, b, s, w, n);
}
inline uintptr_t clB(const D2GameS3aEnv* e, const void* blk, uint32_t nblk, uint32_t rva, uintptr_t a, uintptr_t c, uintptr_t d, uintptr_t b, uintptr_t s, std::initializer_list<uint32_t> st = {}) {
    uint32_t w[8] = {}; uint32_t n = 0; for (uint32_t v : st) if (n < 8) w[n++] = v;
    if (e->blk) e->blk(blk, nblk);
    return e->call(rva, a, c, d, b, s, w, n);
}
inline uintptr_t cl(const D2GameS3aEnv* e, uint32_t rva, uintptr_t a, uintptr_t c, uintptr_t d, uintptr_t b, uintptr_t s, std::initializer_list<uint32_t> st = {}) {
    uint32_t w[8] = {}; uint32_t n = 0; for (uint32_t v : st) if (n < 8) w[n++] = v;
    return e->call(rva, a, c, d, b, s, w, n);
}
// the quest-state node of a type in the list at game+0x10f4 (head ptr at [0], next at +0xf4, type dword at +0); 0 if absent
inline uintptr_t quest(const D2GameS3aEnv* e, uintptr_t game, uint32_t type) {
    uint32_t l = rd32(game, 0x10f4); if (!l) return 0;
    for (uint32_t n = rd32(e->p(l), 0); n; n = rd32(e->p(n), 0xf4)) if (rd32(e->p(n), 0) == type) return e->p(n);
    return 0;
}
}
