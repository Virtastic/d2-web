// Release a client's unit list: EAX = owner (a null owner is fatal), EDI = the value handed to each release (plain ret). The
// owner's list is fetched through two out-pointers (list, count), then every entry is released with that value in list order.
// A rule over E (E::h = the callee call, E::stk = a local's address as the callee sees it).
#ifndef D2GAME_RULE_9D550
#define D2GAME_RULE_9D550
#include <cstdint>

namespace g9d550 {
template <class E> void ReleaseAll(E& e, uint32_t owner, uint32_t v)
{
    uint32_t count = 0, list = 0;
    e.h(0xA55C, 0, 0, 0, 0, 0, owner, e.stk(&list), e.stk(&count));
    for (uint32_t i = 0; i < count; ++i) e.h(0xAF04, 0, 0, 0, 0, 0, e.rd32(list + 4 * i), v);
}
}
#endif
