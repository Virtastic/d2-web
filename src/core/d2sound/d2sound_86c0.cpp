// 1.13c D2Sound+0x86C0 -- singly linked list {key, next} (32-bit links): unlink and free the first node whose key == value.
// The free is Ordinal_10043 through the 0x5F68 thunk (ecx = node, line 0xF0): a host seam.
#include <cstdint>
static void (*g_free)(uint32_t node, uint32_t line);
void D2sound_86C0SetFree(void (*f)(uint32_t, uint32_t)) { g_free = f; }
void D2sound_86C0(uint32_t* head, int32_t value) {
    uint32_t* prev = nullptr;
    uint32_t* h = (uint32_t*)(uintptr_t)*head;
    for (uint32_t* n = h; n; prev = n, n = (uint32_t*)(uintptr_t)n[1]) {
        if ((int32_t)n[0] != value) continue;
        if (h == n) *head = n[1];
        if (prev) prev[1] = n[1];
        g_free((uint32_t)(uintptr_t)n, 0xF0);
        return;
    }
}
