// src/core/d2game/d2game_8D860.cpp -- transcribed from the cached decompile and the 1.13c disassembly
// (the packet bytes are register-built, so the decompile hides them).
// Sibling of 0x8D990/0x8DB00 in d2game_batch_8DB00.cpp; the 16-byte unit-move packet the caller
// 0xF35C0 sends for any header other than 0x68.
// Prior art: D2MOO 1.10f D2GAME_PACKETS_SendPacket0x67_6FC3CDE0 (SCmd.cpp), same layout.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

struct D2Game8D860Env {
    void     (*sendPacket)(uint32_t client, const void* packet, uint32_t length); // 0x8A3E0 (client in EAX)
    uint8_t  (*ordinal10471)(uint32_t path);                                      // #10471 path -> mode byte
    uint16_t (*ordinal10976)(uint32_t path);                                      // #10976 last point X
    uint16_t (*ordinal11001)(uint32_t path);                                      // #11001 last point Y
    int32_t  (*ordinal10973)(uint32_t unit, uint32_t stat, uint32_t layer);       // #10973 stat value
};

static inline uint32_t r32(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)p + o, 4); return v; }

// 1.13c D2Game+0x8D860
// EAX = unit id, EBX = client, DL = packet[0]; stack args a1 (byte), a2 (word), a3 (word),
// a4 (byte), a5 (byte -> packet[0xF]), a6 (byte). Walks the type-1 unit list at game+0x1320.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8d860(uint32_t eaxUnitId, uint32_t ebxClient, uint8_t dl,
                                             uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                                             uint32_t a5, uint32_t a6, const D2Game8D860Env* env)
{
    if (ebxClient == 0) _exit(-1);                 // retail: fatal id 0x996 via #10265/#10024
    const uint32_t game = r32(ebxClient, 0x1A8);
    if (game == 0) _exit(-1);                      // fatal id 0x2D3

    uint8_t pkt[16] = {0};
    pkt[0] = dl;
    std::memcpy(pkt + 1, &eaxUnitId, 4);
    pkt[5] = (uint8_t)a1;
    uint16_t w;
    w = (uint16_t)a2; std::memcpy(pkt + 6, &w, 2);
    w = (uint16_t)a3; std::memcpy(pkt + 8, &w, 2);

    uint32_t node = r32(game, 0x1320 + (eaxUnitId & 0x7F) * 4);
    if (node == 0) return;
    while (r32(node, 0x0C) != eaxUnitId) {
        node = r32(node, 0xE4);
        if (node == 0) return;
    }
    const uint32_t path = r32(node, 0x2C);
    pkt[0x0A] = (uint8_t)a4;
    pkt[0x0B] = (uint8_t)a6;
    const uint8_t mode = env->ordinal10471(path);
    pkt[0x0C] = mode;
    if (mode >= 5) {
        if (mode <= 6) {
            pkt[0x0C] = 1;
            w = env->ordinal10976(path); std::memcpy(pkt + 6, &w, 2);
            w = env->ordinal11001(path); std::memcpy(pkt + 8, &w, 2);
        } else if (mode == 8) {
            pkt[0x0C] = 0x0B;
        }
    }
    int32_t v = env->ordinal10973(node, 0x43, 0);
    if (v < -0x8000) v = -0x8000;
    if (v > 0x7FFF) v = 0x7FFF;
    w = (uint16_t)v; std::memcpy(pkt + 0x0D, &w, 2);
    pkt[0x0F] = (uint8_t)a5;
    env->sendPacket(ebxClient, pkt, 16);
}
