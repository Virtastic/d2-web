// D2CMP cache: unlink a node from its LRU list (EAX = node, param_1 = cache in retail).
#include <cstdint>
struct D2CmpUNode { uint32_t _0[3]; uint32_t flags; uint32_t tick; D2CmpUNode* next; D2CmpUNode* prev; };
struct D2CmpUCache { uint32_t _0[5]; D2CmpUNode* head0; D2CmpUNode* tail0; D2CmpUNode* head1; D2CmpUNode* tail1; };

// 1.13c D2CMP+0x0C9F0
void d2cmp_c9f0(D2CmpUCache* c, D2CmpUNode* n, void (*fatal)())
{
    if (!n) { fatal(); return; } // retail: string 0xb44 via Ordinal_10265/10024, _exit(-1)
    D2CmpUNode** head = (n->flags & 1) ? &c->head1 : &c->head0;
    D2CmpUNode** tail = (n->flags & 1) ? &c->tail1 : &c->tail0;
    D2CmpUNode* h = *head;
    if (!h) return;
    if (n == h) {
        if (h->flags & 2) return;
        D2CmpUNode* nx = h->next;
        *head = nx;
        if (!nx) *tail = 0; else nx->prev = 0;
        h->next = 0; h->prev = 0;
        return;
    }
    if (n == *tail) {
        D2CmpUNode* p = (*tail)->prev;
        *tail = p;
        p->next = 0;
        n->next = 0; n->prev = 0;
        return;
    }
    if (n->prev || n->next) {
        n->prev->next = n->next;
        n->next->prev = n->prev;
        n->next = 0; n->prev = 0;
    }
}
