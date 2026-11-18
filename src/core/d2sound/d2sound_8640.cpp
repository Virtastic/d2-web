// 1.13c D2Sound+0x8640 -- append a new {value, next} node to the tail of a singly linked list (32-bit links).
// The node comes from Ordinal_10042 via the 0x5F62 thunk (ecx = 8, line, zero): a host seam returning the address.
#include <cstdint>
static uint32_t (*g_alloc)(uint32_t size);
void D2sound_8640SetAlloc(uint32_t (*f)(uint32_t)) { g_alloc = f; }
void D2sound_8640(uint32_t* head, uint32_t value) {
    uint32_t a = g_alloc(8);
    uint32_t* n = (uint32_t*)(uintptr_t)a;
    n[0] = 0; n[1] = 0; n[0] = value;
    uint32_t* c = (uint32_t*)(uintptr_t)*head;
    if (!c) { *head = a; return; }
    while (c[1]) c = (uint32_t*)(uintptr_t)c[1];
    c[1] = a;
}
