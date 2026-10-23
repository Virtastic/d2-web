// src/core/d2game/d2game_batch_2d500.cpp -- 1.13c D2Game+0x2D500 transcribed from the cached decompile.
// Client record fields are read by retail byte offset; list pointers are host pointers.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wrptr(void* p, uint32_t o, void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

struct D2Game2D500Env {
    void    (*sendUnitUpdates)(void* client);            // 0x2C060 (client in EAX)
    void    (*validatePlayerUnit)(void* client);         // 0x2BE20 (client in ESI)
    void    (*flushHeader)(void* client, void* game);    // 0x2D140 (client EAX, game stack)
    void    (*disconnectClient)(void* client, void* game); // 0x2D090
    int32_t (*send)(uint32_t channel, uint32_t clientId, const void* data, uint32_t len); // D2Net #10002
    void    (*netPostSend)(void);                        // D2Net #10003
};

static uint32_t g_unitUpdatesEnabled;   // 0x6FD31C38

// 1.13c D2Game+0x2D500
// Send-queue pump (client in EAX, game, flag). Null game / client are the 0xEE2 / 0xEE3 fatals.
// Packet node: len +0, data +4, next +0x204. Client: state +4, head +0x1B8, tail +0x1BC,
// free list +0x1C0, failure count +0x1C4.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_2d500(void* client, void* game, int32_t flag, const D2Game2D500Env* env)
{
    if (game == nullptr || client == nullptr) _exit(-1);   // 0xEE2 / 0xEE3 via #10265/#10024
    if (flag != 0 && rd32(client, 4) == 4) {
        if (g_unitUpdatesEnabled != 0) env->sendUnitUpdates(client);
        env->validatePlayerUnit(client);
    }
    const uint8_t st = rd8(game, 0x6A);
    const bool live = st == 1 || st == 2;
    if (live) env->flushHeader(client, game);

    bool sent = false;
    int32_t okCount = 0;
    for (;;) {
        void* node = rdptr(client, 0x1B8);
        if (rdptr(client, 0x1BC) == node) { wrptr(client, 0x1BC, nullptr); wrptr(client, 0x1B8, nullptr); }
        if (node == nullptr) return;
        wrptr(client, 0x1B8, rdptr(node, 0x204));
        if (rd32(node, 0) == 0) return;
        if (env->send(1, rd32(client, 0), (uint8_t*)node + 4, rd32(node, 0)) == 0) {
            env->netPostSend();
            wr32(client, 0x1C4, rd32(client, 0x1C4) + 1);
            wrptr(node, 0x204, nullptr);
            sent = true;
            if (rdptr(client, 0x1B8) != nullptr) wrptr(client, 0x1BC, node);
            wrptr(client, 0x1B8, node);
            if ((int32_t)rd32(client, 0x1C4) > 2) { env->disconnectClient(client, game); return; }
        } else {
            wr32(node, 0, 0);
            wrptr(node, 0x204, rdptr(client, 0x1C0));
            ++okCount;
            wrptr(client, 0x1C0, node);
            wr32(client, 0x1C4, 0);
        }
        if (live && okCount > 2) return;
        if (sent) return;
    }
}
