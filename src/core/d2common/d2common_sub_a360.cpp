// 1.13c D2Common+0x0A360 (sub_A360, usercall: context in EBX, no stack args; sole caller +0xAB50).
// Walks the singly linked node list of the context's inner object (+0x20 -> +8 -> list head at +0x38).
// A node whose (x at +8, y at +0x18) lies inside the rect [dx*5, dx*5+w*5) x [dy*5, dy*5+h*5) is
// shifted by (-dx*5, -dy*5), unlinked, and pushed on the context free list (+0x5c). Others stay.
#include <cstdint>

struct A360Node { int32_t pad0[2]; int32_t x; A360Node* next; int32_t pad1[2]; int32_t y; };
struct A360Inner { uint8_t pad[0x38]; A360Node* head; };
struct A360Mid { uint8_t pad[8]; A360Inner* inner; };
struct A360Ctx {
    uint8_t pad0[0x20]; A360Mid* mid; uint8_t pad1[0x34 - 0x20 - sizeof(void*)];
    int32_t dx, dy, w, h; uint8_t pad2[0x5c - 0x44]; A360Node* freeList;
};

// ponytail: 64-bit pointers widen the retail offsets; the int field offsets are kept by layout only on 32-bit.
void D2Common_A360_MoveNodesInRect(A360Ctx* c)
{
    A360Inner* in = c->mid->inner;
    const int32_t x0 = c->dx * 5, y0 = c->dy * 5;
    A360Node* prev = nullptr;
    for (A360Node* n = in->head; n;) {
        A360Node* next = n->next;
        if (n->x < x0 || n->y < y0 || n->x >= x0 + c->w * 5 || n->y >= y0 + c->h * 5) { prev = n; n = next; continue; }
        n->x -= x0; n->y -= y0;
        (prev ? prev->next : in->head) = next;
        n->next = c->freeList; c->freeList = n;
        n = next;
    }
}
