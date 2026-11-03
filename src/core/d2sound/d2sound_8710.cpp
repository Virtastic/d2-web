// 1.13c D2Sound+0x8710 -- push a new {value, next = old head} node on the front of a singly linked list (32-bit links).
// The node comes from Ordinal_10042 via the 0x5F62 thunk (ecx = 8): a host seam returning the address.
#include <cstdint>
static uint32_t (*g_alloc)(uint32_t size);
void D2sound_8710SetAlloc(uint32_t (*f)(uint32_t)) { g_alloc = f; }
void D2sound_8710(uint32_t* head, uint32_t value) {
    uint32_t a = g_alloc(8);
    uint32_t* n = (uint32_t*)(uintptr_t)a;
    n[0] = 0; n[1] = 0;
    uint32_t old = *head;
    n[0] = value; n[1] = old; *head = a;
}
