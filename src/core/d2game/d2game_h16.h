// Seams and ports of the D2Game 0xE2600..0xE6660 rows (one file per function, src/core/d2game/d2game_<rva>.cpp).
// Every callee is a function pointer the caller supplies: an FN probe hands in HookEnv recorders, so the vector
// compares the exact call sequence (callee, registers, stack words) as well as the writes.
#pragma once
#include <cstdint>
#include <cstring>
namespace h16 {
inline uint32_t rd32(uint32_t a, uint32_t o = 0) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)(a + o), 4); return v; }
inline uint32_t rd8(uint32_t a, uint32_t o = 0) { return *(const uint8_t*)(uintptr_t)(a + o); }
inline void wr32(uint32_t a, uint32_t o, uint32_t v) { memcpy((void*)(uintptr_t)(a + o), &v, 4); }
inline void wr8(uint32_t a, uint32_t o, uint32_t v) { *(uint8_t*)(uintptr_t)(a + o) = (uint8_t)v; }

struct Env {
    uint32_t (*o10920)(uint32_t unit);                                  // 0xA4C0: unit to player record (unit slots at +0x10)
    uint32_t (*o10174)(uint32_t unit, uint32_t a, uint32_t b);          // 0xA568: stat lookup
    void     (*o10003)(uint32_t unit, uint32_t a, uint32_t b);          // 0xA57A: stat write
    uint32_t (*o10331)(uint32_t unit);                                  // 0xA478
    uint32_t (*o10826)(uint32_t h);                                     // 0xA556
    uint32_t (*o10459)(uint32_t h);                                     // 0xA56E (byte result)
    uint32_t (*findQuest)(uint32_t eax, uint32_t ebx, uint32_t cc);    // 0xE660: EAX = unit, EBX = game, stack = 4CC
    void     (*s8e6f0)(uint32_t node, uint32_t game);                   // 0x8E6F0: EAX = timer node, EDI = game
    void     (*s8ea80)(uint32_t game, uint32_t unit, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e); // ECX, EDX, 5 stack words
    uint32_t (*o10153)(uint32_t obj, uint32_t off, uint32_t* out);      // 0xA5F8: writes *out
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t file, uint32_t line, uint32_t zero); // 0xA2E6: ECX pool, EDX size
    void     (*o10141)(uint32_t unit, uint32_t* pos);                   // 0xA544: writes {x,y}
    void     (*sdfcc0)(uint32_t game, uint32_t cbRva);                  // 0xDFCC0: ECX = game, EBX = callback, EDI = game
    void     (*s34130)(uint32_t h, uint32_t* pair, uint32_t a, uint32_t b, uint32_t* out, uint32_t c); // 0x34130
    void     (*se1d90)(uint32_t x, uint32_t y, uint32_t game, uint32_t out, uint32_t a, uint32_t b, uint32_t c); // 0xE1D90, ECX=2 EDX=0x83
    uint32_t M_54C0, M_5540, M_5450, M_FE38, M_3030, M_3000, M_file; // module addresses E6660 stores (as the probe records them)
};

void e2600(const Env&, uint32_t game, int32_t pct, uint32_t unit);
uint32_t e36a0(const Env&, uint32_t game, uint32_t unit);
void e3b40(const Env&, uint32_t node, uint32_t a);
void e41d0(const Env&, uint32_t game, uint32_t unit);
uint32_t e4280(const Env&, uint32_t game, uint32_t unit);
uint32_t e4500(const Env&, uint32_t a, uint32_t id, uint32_t p3, uint32_t list);
uint32_t e57d0(const Env&, uint32_t game, uint32_t node);
void e6660(const Env&, uint32_t self);
}
