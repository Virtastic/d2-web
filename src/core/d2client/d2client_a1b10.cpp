// 1.13c D2Client+0xA1B10: apply skill record a1 to the unit in EAX. EAX = unit, six stack words a0..a5 (ret 0x18); the handler gets ECX = unit, EDX = a1, stack a2.
// Env-seam style: every callee is a callback, called with the arguments the disassembly pushes.
#include <cstdint>
#include <cstring>
struct D2cA1B10Env {
    void* c;
    const uint8_t* dt;                    // *sgptDataTables (D2Client+0xCE474): +0xB98 record array, +0xBA0 count
    const uint32_t* htab; int32_t hcount; // D2Client+0xDEA48 handler table (dwords), +0xDEC50 count
    uint32_t (*a1340)(void* c, uint32_t unit, uint32_t* outW, uint32_t* outH);
    void (*f2a0b0)(void* c, uint32_t unit);
    uint32_t (*c2da)(void* c, uint32_t id);                                     // Ordinal 10392
    void (*c62e)(void* c, uint32_t unit, uint32_t v);                           // Ordinal 11129
    void (*c3ee)(void* c, uint32_t id, uint32_t x, uint32_t y);                 // Ordinal 10286
    uint32_t (*handler)(void* c, uint32_t fn, uint32_t unit, uint32_t edx, uint32_t stk);   // ecx = unit, edx = skill, stack a2
    void (*f88a70)(void* c, uint32_t ebx, uint32_t unit);                       // ebx register + (unit,0,0,0)
    uint32_t (*fa5f70)(void* c);
    void (*f6b300)(void* c, uint32_t eax, uint32_t unit);                       // eax register + (unit,2,0,0,0,0,0)
    void (*fatal)(void* c);                                                     // Ordinal 10265(0x946) + 10024 + _exit(-1)
};
static inline uint32_t rd32(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
static inline void wr32(uint32_t p, uint32_t o, uint32_t v) { std::memcpy((void*)(uintptr_t)(p + o), &v, 4); }
// 1.13c D2Client+0xA1B10
extern "C" void d2c_a1b10(const D2cA1B10Env& e, uint32_t unit, uint32_t a0, int32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
    if (!unit || a1 < 0) return;
    uint32_t cnt; std::memcpy(&cnt, e.dt + 0xBA0, 4);
    if (a1 >= (int32_t)cnt) return;
    uint32_t base; std::memcpy(&base, e.dt + 0xB98, 4);
    const uint32_t rec = (uint32_t)a1 * 0x23C + base;
    if (!rec) return;
    const uint32_t saved = rd32(unit, 0x28);
    uint32_t outW, outH;
    const uint32_t r1 = e.a1340(e.c, unit, &outW, &outH);
    e.f2a0b0(e.c, unit);
    if (!rd32(unit, 0x2C)) { e.fatal(e.c); return; }
    const uint32_t u = e.c2da(e.c, rd32(unit, 0x2C));
    uint32_t ebx = a3, edi = a4;
    bool skip = false;
    if (a0) { e.c62e(e.c, unit, a0); wr32(unit, 0x28, a5 & 0xFF); }
    else if (!ebx) skip = true;
    else if (edi) { e.c3ee(e.c, rd32(unit, 0x2C), ebx, edi); wr32(unit, 0x28, a5 & 0xFF); }
    if (!skip && ebx && !edi) wr32(unit, 0x28, ebx);
    auto s16 = [&](uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)(rec + o), 2); return (int32_t)(int16_t)v; };
    const int32_t hs = s16(0xF6);
    if (hs >= 0 && hs < e.hcount && e.htab[hs] && e.handler(e.c, e.htab[hs], unit, (uint32_t)a1, a2)) {
        if (s16(0x100) > 0) e.f88a70(e.c, (uint32_t)s16(0x100), unit);
        if (s16(0x10A) > 0) { const uint32_t t = e.fa5f70(e.c); if (t) e.f88a70(e.c, (uint32_t)s16(0x10A), t); }
        if (s16(0x108) > 0) { const uint32_t t = e.fa5f70(e.c); if (t) e.f6b300(e.c, (uint32_t)s16(0x108), t); }
    }
    e.c62e(e.c, unit, u);
    if (r1 && !u) e.c3ee(e.c, rd32(unit, 0x2C), outW, outH);
    wr32(unit, 0x28, saved);
}
