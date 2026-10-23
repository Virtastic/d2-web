#include <cstdint>
extern "C" void D2sound_free(void* p, int line); // Ordinal_10043 host seam (Fog free)
struct SNode { int32_t key; SNode* next; };
// 1.13c D2sound+0x8920 free every node of the list (each pass removes the first node with the head's key).
void D2sound_8920(SNode** head) {
    while (*head) {
        SNode* h = *head; SNode* prev = nullptr;
        for (SNode* n = h; n; prev = n, n = n->next) {
            if (n->key != h->key) continue;
            if (h == n) *head = n->next;
            if (prev) prev->next = n->next;
            D2sound_free(n, 0xf0);
            break;
        }
    }
}
