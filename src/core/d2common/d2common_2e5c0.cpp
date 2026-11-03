// 1.13c D2Common+0x2E5C0 (sub_2E5C0): find or load the room that holds (x, y). Register input: EAX = the current
// room `cur` (0 for none). Stack words: x, y, owner, room (room 0 = look it up). Early exits return the room that
// holds the point (cur, or a neighbour in cur's list). Otherwise the owner's room list is searched by id (the id
// comes from 0x2D5B0), a missing room is created by 0x2D9B0, a room that is not loaded (+0x10 zero) is loaded by
// 0x2E360, and 0x2D600 is then told the room. That last path returns 0 here; retail returns whatever EAX holds
// after the 0x2D600 call, which the descriptor masks off (retmask 0). Callees are seams, see the env below.
#include <cstdint>
#include <cstring>

// Seams: 0x2D5B0 takes EAX = owner, ESI = y (EDI = x) and returns the id; 0x2D9B0 takes ECX = owner, EDX = id and returns
// the new room; 0x2E360 takes one stack word (room); 0x2D600 takes EAX = room, ESI = y (EDI = x).
struct D2Common2E5C0Env {
    int32_t (*lookupId)(uint32_t owner, int32_t x, int32_t y);
    uint32_t (*getOrCreate)(uint32_t owner, int32_t id);
    void (*loadRoom)(uint32_t room);
    void (*touch)(uint32_t room, int32_t x, int32_t y);
};

namespace {
uint32_t rd32(uint32_t a, uint32_t off) {
    uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a + off)), 4); return v;
}
int32_t rdS(uint32_t a, uint32_t off) { return static_cast<int32_t>(rd32(a, off)); }
int32_t add32(int32_t a, int32_t b) { return static_cast<int32_t>(static_cast<uint32_t>(a) + static_cast<uint32_t>(b)); }
// Room box: origin (+0x34, +0x38), size (+0x3C, +0x40); the point is inside when x0 <= x < x0+w and y0 <= y < y0+h.
bool holds(uint32_t r, int32_t x, int32_t y) {
    const int32_t x0 = rdS(r, 0x34), y0 = rdS(r, 0x38);
    return x0 <= x && y0 <= y && x < add32(x0, rdS(r, 0x3c)) && y < add32(y0, rdS(r, 0x40));
}
}

// 1.13c D2Common+0x2E5C0
extern "C" uint32_t D2Common_2E5C0_FindOrLoadRoom(uint32_t cur, int32_t x, int32_t y, uint32_t owner, uint32_t room,
                                                  const D2Common2E5C0Env* env) {
    if (cur) {
        if (holds(cur, x, y)) return cur;
        const uint32_t list = rd32(cur, 0x8);
        const int32_t count = rdS(cur, 0x2c);
        for (int32_t i = 0; i < count; i++) {
            const uint32_t o = rd32(list, 4u * static_cast<uint32_t>(i));
            if (o != cur && holds(o, x, y)) return o;
        }
    }
    if (!room) {
        const int32_t id = env->lookupId(owner, x, y);
        uint32_t n = rd32(owner, 0x47c);
        while (n && rdS(n, 0x1d0) != id) n = rd32(n, 0x1ac);
        room = n ? n : env->getOrCreate(owner, id);
    }
    if (!rd32(room, 0x10)) env->loadRoom(room);
    env->touch(room, x, y);
    return 0;
}
