// (alt copy; batch_67c0 carries the marker) D2sound 0x08640 -- append a new {value,next} node (8 bytes, zeroed) at the tail of a singly linked list.
#include <cstdint>
struct D2soundNode8640 { void* value; D2soundNode8640* next; };
void D2sound_sub_8640(D2soundNode8640** head, void* value, D2soundNode8640* (*alloc8)()) {
    D2soundNode8640* n = alloc8(); n->value = value; n->next = nullptr;
    if (!*head) { *head = n; return; }
    D2soundNode8640* p = *head;
    while (p->next) p = p->next;
    p->next = n;
}
