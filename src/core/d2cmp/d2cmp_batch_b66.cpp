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

// 1.13c D2CMP+0x0bfd0  register-arg: in_EAX = owner, unaff_ESI = node become leading params;
// GetTickCount result becomes a param (no direct Win32). Appends node to list head 0x14/0x18
// (param_1==0) or 0x1c/0x20 (else); node+0x14 = next, +0x18 = prev, +0x10 = tick.
struct Node_b66;
struct Owner_b66 { uint8_t p[0x14]; Node_b66* h14; Node_b66* t18; Node_b66* h1c; Node_b66* t20; };
struct Node_b66 { uint8_t p[0x10]; uint32_t tick; Node_b66* next; Node_b66* prev; };
void sub_BFD0(Owner_b66* owner, Node_b66* node, int param_1, uint32_t tick)
{
    Node_b66*& head = param_1 == 0 ? owner->h14 : owner->h1c;
    Node_b66*& tail = param_1 == 0 ? owner->t18 : owner->t20;
    if (!tail) { head = node; tail = node; }
    else { tail->next = node; node->prev = tail; tail = node; }
    node->tick = tick;
}
