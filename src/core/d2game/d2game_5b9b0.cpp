// src/core/d2game/d2game_5b9b0.cpp -- 1.13c D2Game+0x5B9B0, written from the 1.13c disassembly (the
// decompile's unaff_* slots resolved against it). Seams are function pointers in retail argument order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdp (const void* p, uint32_t o) { return (void*)(uintptr_t)rd32(p, o); }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

struct D2Game5b9b0Env {
    int32_t  (*sub78260)(void* node, void* player);                         // ECX=node, EAX=player; slot index
    void*    (*getRoom)(void* unit);                                        // Common #10331
    void     (*nearRooms)(void* room, void*** rooms, uint32_t* count);      // Common #10383
    int32_t  (*dist)(void* unit, void* other);                              // Common #10522
    void     (*unitPos)(void* unit, void* pos);                             // Common #10141 (writes pos[0])
    void*    (*sub21280)(void* room);                                       // EAX=room
    void*    (*sub1D90)(int32_t x, int32_t y, void* game, void* h, int32_t a, int32_t b, int32_t c); // ECX=2, EDX=0xBD
    void     (*sub54130)(void* h, int32_t* pos, int32_t two, int32_t flags, void** out, int32_t tries);
    void     (*subAEA80)(void* game, void* s, int32_t seven, int32_t v, int32_t a, int32_t b, int32_t c); // ECX=game, EDX=s
    void     (*setRoomState)(void* room, int32_t z);                        // Common #10346
    void*    (*getClientData)(void* player);                                // Common #10920
    uint32_t (*questGet)(void* q, int32_t quest, int32_t flag);             // Common #10174
    void     (*questSet)(void* q, int32_t quest, int32_t flag);             // Common #10003
    void     (*dispatch)(void* game, void* cb, void* a2);                   // 0xDFCC0: EAX=0, EBX=cb, EDI=game, stack a2
    void *cb793f0, *cb79390, *cb7b180, *cb792f0;                            // the four callbacks, by RVA
};

// 1.13c D2Game+0x5B9B0 -- ret 0xC. game=a1 (+0x10f4 node list, +0x6d difficulty), a2 = unit whose room is scanned, a3 = player.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_5b9b0(void* game, void* a2, void* a3, const D2Game5b9b0Env* e) {
    void* head = rdp(game, 0x10f4);
    if (!head) return;
    uint8_t* node = (uint8_t*)rdp(head, 0);
    if (!node) return;
    while (rd32(node, 0) != 0x20) { node = (uint8_t*)rdp(node, 0xf4); if (!node) return; }
    uint8_t* data = (uint8_t*)rdp(node, 0x18);
    int32_t idx = e->sub78260(node, a3);
    if (idx < 0) return;
    uint8_t* flag = data + idx + 0x10e;
    if (*flag == 1) return;
    int32_t pos[2];                                   // X (stack slot orig-8), Y (orig-4)
    pos[0] = (int32_t)rd32(data, 0x8c + idx * 8) + 0xc;
    pos[1] = (int32_t)rd32(data, 0x90 + idx * 8);
    void* room = e->getRoom(a3);
    void* r2 = e->getRoom(a2);
    if (!r2) return;
    void** rooms; uint32_t cnt;
    e->nearRooms(r2, &rooms, &cnt);
    bool found = false;
    for (uint32_t i = 0; i < cnt && cnt; i++) {
        for (uint8_t* u = (uint8_t*)rdp(rooms[i], 0x74); u; u = (uint8_t*)rdp(u, 0xe8)) {
            if (rd32(u, 0) == 1 && rd32(u, 4) == 0x1b2 && rd32(u, 0x10) == 0xc && e->dist(u, a3) < 0xf) {
                room = e->getRoom(u);
                e->unitPos(u, &pos[0]);
                pos[0] += 2;
                found = true;
                break;                                // next room; later matches overwrite
            }
        }
    }
    if (!found) return;
    *flag = 1;
    void* h = e->sub21280(room);
    void* gm = rdp(node, 4);
    void* s = e->sub1D90(pos[0], pos[1], gm, h, 1, 1, 0);
    if (!s) s = e->sub1D90(pos[0] - 2, pos[1], gm, h, 1, 1, 0);
    if (!s) {
        if (h) {
            void* out = a3;                           // retail passes the a3 stack slot as the out cell
            e->sub54130(h, pos, 2, 0x8000, &out, 0x64);
            s = e->sub1D90(pos[0], pos[1], gm, out ? out : h, 1, 1, 0);
        }
    }
    if (s) {
        wr8(data, 0xf8 + idx, 1);
        wr32(data, 0xec + idx * 4, rd32(s, 0xc));
        e->subAEA80(gm, s, 7, (int32_t)rd32(gm, 0xa8) + 0x19, 0, 0, 0);
        e->setRoomState(e->getRoom(s), 0);
    }
    uint8_t* cd = (uint8_t*)e->getClientData(a2);
    void* q = rdp(cd, 0x10 + rd8(game, 0x6d) * 4);
    int32_t ac = (int32_t)rd32(data, 0xac), a8 = (int32_t)rd32(data, 0xa8);
    if ((int32_t)rd32(data, 0xa4) == a8 + ac && rd8(data, 0x86) && rd8(data, 0x87) && rd8(data, 0x88)) {
        if (ac <= 0xb) { e->dispatch(game, e->cb79390, a2); return; }
        wr8(node, 0x14, 0); wr8(node, 0xb, 3);
        e->dispatch(gm, e->cb793f0, nullptr);
        if (!e->questGet(q, 0x24, 0) && !e->questGet(q, 0x24, 1)) {
            e->questSet(q, 0x24, 0xd);
            e->questSet(q, 0x24, 1);
            e->questSet(q, 0x24, ac == 0xf ? 5 : ac == 0xe ? 6 : 7);
        }
        e->dispatch(game, e->cb7b180, a2);
        e->dispatch(game, e->cb79390, a2);
        e->dispatch(game, e->cb792f0, a2);
        return;
    }
    e->questSet(q, 0x24, 4);
    if (a8 < 5) {
        wr8(node, 0x14, 0x20); wr8(node, 0xb, 2);
        e->dispatch(gm, e->cb793f0, nullptr);
    }
    wr8(data, 0x84, 0);
}
