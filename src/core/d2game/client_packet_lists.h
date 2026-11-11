// 1.13c D2Game+0x2CA20 neighbourhood: the client packet lists the server frame drains (D2Net #10016 list 1,
// read by GAME_ProcessNetworkMessages, D2Game+0x2E460). Retail never applies a client packet where it arrives:
// the packet is appended to a list and the next server frame, before the game update, reads each list until it
// returns -1 and applies every packet. This is that list for the local connection: a FIFO of packets up to
// 0x200 bytes (the buffer 0x2E460 reads into); a longer packet is refused at the append.
#ifndef D2G_CLIENT_PACKET_LISTS_H
#define D2G_CLIENT_PACKET_LISTS_H
#include <cstdint>
#include <cstring>
#include "d2g_2e460.cpp"
namespace d2g_lists {
constexpr int kCapacity = 0x200, kPackets = 256, kBytes = 1 << 16;
struct List {
    uint8_t bytes[kBytes];
    uint16_t sizes[kPackets];
    int head, count, used, headOffset;   // head packet index, packets held, bytes held, offset of the head packet
    void clear() { head = count = used = headOffset = 0; }
    // 1 appended, 0 refused (empty, longer than 0x200, or the list is full).
    int append(const uint8_t* p, int n) {
        if (!p || n <= 0 || n > kCapacity || count >= kPackets) return 0;
        if (!count) headOffset = 0;
        if (headOffset + used + n > kBytes) return 0;
        std::memcpy(bytes + headOffset + used, p, n);
        sizes[(head + count) % kPackets] = (uint16_t)n;
        ++count; used += n;
        return 1;
    }
    // The read the frame loops on: the oldest packet's size copied to buf, or -1 when the list is empty or cap is short.
    int32_t read(void* buf, int32_t cap) {
        if (!count) return -1;
        const int n = sizes[head];
        if (cap < n) return -1;
        std::memcpy(buf, bytes + headOffset, n);
        headOffset += n; used -= n; head = (head + 1) % kPackets; --count;
        return n;
    }
};
// One server frame's drain of list 1 through the ported 0x2E460 loop; the other two lists are empty for the local connection.
template <class Apply>
inline void drainFrame(List& list1, Apply apply) {
    struct Ctx { List* l; Apply* a; };
    static thread_local Ctx ctx;
    ctx = { &list1, &apply };
    d2g_2e460::Env e = {
        [](void*, int32_t) -> int32_t { return -1; },
        [](void* b, int32_t c) -> int32_t { return ctx.l->read(b, c); },
        [](void*, int32_t) -> int32_t { return -1; },
        [](void*) {},
        [](void* b, int32_t n) { (*ctx.a)(static_cast<const uint8_t*>(b), n); },
        [](void*) {},
    };
    d2g_2e460::GAME_ProcessNetworkMessages_113c(e);
}
}
#endif
