#include <cstdint>
extern "C" void D2sound_free(void* p, int line); // Ordinal_10043 host seam (Fog free)
struct SNode { int32_t key; SNode* next; };
// 1.13c D2sound+0x86C0 singly linked list: unlink and free the first node whose key == value.
void D2sound_86C0(SNode** head, int32_t value) {
    SNode* prev = nullptr;
    for (SNode* n = *head; n; prev = n, n = n->next) {
        if (n->key != value) continue;
        if (*head == n) *head = n->next;
        if (prev) prev->next = n->next;
        D2sound_free(n, 0xf0);
        return;
    }
}
