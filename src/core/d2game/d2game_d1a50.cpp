// 1.13c D2Game+0xD1A50, injectable callees. EAX = record (field +0x60 = linked unit), EBX = out pointer 2, EDI = out pointer 1; plain ret; returns EAX as left at the ret.
// Thunks: 0xA54A (1 stack word), 0xA7AE (2 words), 0xA430 (2 words), each ret n.
#include <cstdint>
#include <cstring>
struct GD1a50Env {
    uint32_t (*o54a)(uint32_t s0);
    uint32_t (*o7ae)(uint32_t s0, uint32_t s1);
    uint32_t (*o430)(uint32_t s0, uint32_t s1);
};
static uint32_t rd32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static void wr32(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }
// 1.13c D2Game+0xD1A50
uint32_t g_d1a50(uint32_t eax, uint32_t ebx, uint32_t edi, const GD1a50Env& env)
{
    wr32(ebx, 0);
    wr32(edi, 0);
    uint32_t u = rd32(eax + 0x60);
    if (u == 0) return eax;
    wr32(edi, env.o54a(u));
    uint32_t a = env.o7ae(u, 4);
    uint32_t b = env.o7ae(u, 5);
    wr32(ebx, rd32(edi) == a ? b : a);
    uint32_t t = rd32(ebx);
    if (t == 0) return 0;
    uint32_t r = env.o430(t, 0x2D);
    if (r == 0) wr32(ebx, 0);
    return r;   // EAX at ret: the record pointer (no unit), 0 (no target), else the 0x2D check's result
}
