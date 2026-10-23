#include <cstdint>
struct StormLogNode346e0 { uint32_t id; StormLogNode346e0* next; };
// 1.13c Storm+0x346E0: unlink node from log bucket list, free it, release bucket lock.
extern "C" void Storm_Sub346E0(uint32_t bucket, StormLogNode346e0* node, StormLogNode346e0** heads,
                               void (*freeNode)(StormLogNode346e0*), void (*leave)(uint32_t)) {
    StormLogNode346e0** link = &heads[bucket];
    for (auto* p = *link; p; link = &p->next, p = *link)
        if (p == node) { *link = p->next; freeNode(p); break; }
    leave(bucket);
}
