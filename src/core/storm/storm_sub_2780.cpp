#include <cstdint>
// 1.13c Storm+0x02780: TSList unlink-all (edi=list). While term.next > 0, unlink the
// link at (next + linkoffset): splice prev/next, clear both words.
struct S2780Link { uintptr_t prevlink, next; };
struct S2780List { intptr_t linkoffset; S2780Link term; };
void StormSub2780ListUnlinkAll(S2780List* list) {
    while ((intptr_t)list->term.next > 0) {
        S2780Link* l = (S2780Link*)(list->term.next + (uintptr_t)list->linkoffset);
        uintptr_t prev = l->prevlink;
        if (!prev) continue;  // retail: no-op, same loop (would spin; unreachable on valid lists)
        uintptr_t next = l->next;
        uintptr_t target = (intptr_t)next < 0 ? ~next
                         : next + ((uintptr_t)l - *(uintptr_t*)(prev + sizeof(uintptr_t)));
        *(uintptr_t*)target = prev;
        *(uintptr_t*)(prev + sizeof(uintptr_t)) = next;
        l->prevlink = 0;
        l->next = 0;
    }
}
