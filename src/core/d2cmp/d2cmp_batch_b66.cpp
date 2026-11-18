// D2CMP batch b66. Skipped (SEH/CRT/Win32): 2F30 2F6B 4DD2 67FE 673D 4ABC 7875 4EF1 1F30 (raw-pointer heap).
#include <cstdint>
#include <initializer_list>

// 1.13c D2CMP+0x04ec6  ___sbh_find_block: the _DAT_6ff14e6c table (n entries of 0x14 bytes,
// base address at +0xc) is passed as (table, count); returns the region holding addr, else null.
struct SbhRegion_b66 { uint32_t pad[3]; uint32_t base; uint32_t pad2; };
SbhRegion_b66* sub_4EC6(SbhRegion_b66* table, uint32_t count, uint32_t addr)
{
    for (SbhRegion_b66* r = table; r < table + count; ++r)
        if (addr - r->base < 0x100000u) return r;
    return nullptr;
}

// 1.13c D2CMP+0x4E66  register-arg: eax and ebp become leading params; stores the three
// unwind-state globals (DAT_6fe32450 = [ebp+8], DAT_6fe3244c = eax, DAT_6fe32454 = ebp).
struct UnwindState_b66 { uint32_t g2450, g244c, g2454; };
UnwindState_b66 g_unwind_b66;
void sub_4E66(uint32_t in_eax, uint32_t ebp, uint32_t ebp_plus8)
{
    g_unwind_b66.g2450 = ebp_plus8;
    g_unwind_b66.g244c = in_eax;
    g_unwind_b66.g2454 = ebp;
}

// 1.13c D2CMP+0x0bfd0  register-arg: EAX = owner, ECX = list select, ESI = node, as 32-bit
// retail addresses of the SpriteCache structs (flat guest image). GetTickCount is the tick seam.
// Appends node to list head/tail 0x14/0x18 (select == 0) or 0x1c/0x20 (else); node+0x14 = next,
// +0x18 = prev, +0x10 = tick. The node's own next is left as it is.
static inline uint32_t rdg_b66(uint32_t a) { uint32_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline void wrg_b66(uint32_t a, uint32_t v) { __builtin_memcpy((void*)(uintptr_t)a, &v, 4); }
void sub_BFD0(uint32_t owner, uint32_t node, int param_1, uint32_t (*tick)())
{
    const uint32_t ho = param_1 == 0 ? 0x14 : 0x1c, to = ho + 4;
    const uint32_t tail = rdg_b66(owner + to);
    if (!tail) { wrg_b66(owner + ho, node); wrg_b66(owner + to, node); }
    else { wrg_b66(tail + 0x14, node); wrg_b66(node + 0x18, rdg_b66(owner + to)); wrg_b66(owner + to, node); }
    wrg_b66(node + 0x10, tick());
}
