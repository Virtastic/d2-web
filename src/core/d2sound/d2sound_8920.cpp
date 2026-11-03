// 1.13c D2Sound+0x8920 -- free every node of a {key, next} list (32-bit links): each pass removes the first node whose key
// equals the head's key (Ordinal_10043 via 0x5F68, ecx = node, line 0xF0: a host seam).
#include <cstdint>
static void (*g_free)(uint32_t node, uint32_t line);
void D2sound_8920SetFree(void (*f)(uint32_t, uint32_t)) { g_free = f; }
void D2sound_8920(uint32_t* head) {
    while (*head) {
        uint32_t* h = (uint32_t*)(uintptr_t)*head; uint32_t* prev = nullptr;
        for (uint32_t* n = h; n; prev = n, n = (uint32_t*)(uintptr_t)n[1]) {
            if (n[0] != h[0]) continue;
            if (h == n) *head = n[1];
            if (prev) prev[1] = n[1];
            g_free((uint32_t)(uintptr_t)n, 0xF0);
            break;
        }
    }
}
