// src/core/d2game/d2game_batch_8D990.cpp -- 1.13c D2Game+0x8D990 (sub_8D990), transcribed from the
// cached decompile plus the disassembly (stack layout). Alpha batch lane: implementation + marker only.
// Opaque host model as in d2game_batch_8DB00.cpp: offsets, register args explicit, callees as seams.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

struct D2GameBatch8D990Env {
    void    (*sendPacket)(uint32_t client, const void* packet, uint32_t length); // 0x8A3E0 (client in EAX)
    uint8_t (*ordinal10471)(uint32_t path);                                      // #10471 path -> mode byte
    int32_t (*ordinal10867)(uint32_t path);                                      // #10867 (0xA454)
    int32_t (*ordinal10750)(uint32_t path);                                      // #10750 (0xA3B8)
    int32_t (*ordinal10973)(uint32_t unit, uint32_t stat, uint32_t layer);       // #10973 (0xA42A)
};

// 1.13c D2Game+0x8D990
// 21-byte unit packet. EAX = unit id, EBX = client, DL = packet[0]; six stack args a1..a6 (ret 0x18):
// a1 -> [5], a2 -> [0xA], a3 (dword) -> [0xB], a4 -> [0xF], a5 -> [0x14], a6 -> [0x10].
// Same list walk as 0x8D860; mode byte at [0x11] (8 -> 0x0B only), clamped layer-0x43 stat word at [0x12].
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8d990(uint32_t eaxUnitId, uint32_t ebxClient, uint8_t dl,
                                             uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                                             uint32_t a5, uint32_t a6, const D2GameBatch8D990Env* env)
{
    if (ebxClient == 0) _exit(-1);  // retail: #10265(0x996) -> #10024 -> _exit(-1)
    const uint32_t game = rd32((void*)(uintptr_t)ebxClient, 0x1A8);
    if (game == 0) _exit(-1);       // 0x2D3

    uint8_t pkt[21] = {0};
    pkt[0] = dl;
    wr32(pkt, 1, eaxUnitId);
    pkt[5] = (uint8_t)a1;
    pkt[0x0A] = (uint8_t)a2;
    wr32(pkt, 0x0B, a3);

    uint32_t node = rd32((void*)(uintptr_t)game, 0x1320 + (eaxUnitId & 0x7F) * 4);
    if (node == 0) return;
    while (rd32((void*)(uintptr_t)node, 0x0C) != eaxUnitId) {
        node = rd32((void*)(uintptr_t)node, 0xE4);
        if (node == 0) return;
    }

    const uint32_t type = rd32((void*)(uintptr_t)node, 0);
    const uint32_t path = rd32((void*)(uintptr_t)node, 0x2C);
    const bool direct = type == 2 || (type > 3 && type <= 5);
    wr16(pkt, 6, (uint16_t)(direct ? rd32((void*)(uintptr_t)path, 0x0C) : path ? (uint32_t)env->ordinal10867(path) : 0));
    wr16(pkt, 8, (uint16_t)(direct ? rd32((void*)(uintptr_t)path, 0x10) : path ? (uint32_t)env->ordinal10750(path) : 0));
    pkt[0x0F] = (uint8_t)a4;
    pkt[0x10] = (uint8_t)a6;

    uint8_t mode = env->ordinal10471(path);
    if (mode == 8) mode = 0x0B;
    pkt[0x11] = mode;
    int32_t v = env->ordinal10973(node, 0x43, 0);
    if (v < -0x8000) v = -0x8000;
    if (v > 0x7FFF) v = 0x7FFF;
    wr16(pkt, 0x12, (uint16_t)v);
    pkt[0x14] = (uint8_t)a5;
    env->sendPacket(ebxClient, pkt, 21);
}
