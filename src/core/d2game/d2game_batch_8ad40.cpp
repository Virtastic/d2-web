// src/core/d2game/d2game_batch_8ad40.cpp -- seven 1.13c D2Game.dll packet builders (RE-DONE stage A),
// transcribed from the cached Ghidra decompiles. Each builds a short stack packet and hands it to
// 0x8A3E0 (append to the client's outgoing queue). Register-only inputs (Ghidra in_EAX) are explicit
// parameters. The client handed to 0x8A3E0 is not visible in several decompiles; it is a parameter.
// NOT DONE: 0x8C910 (needs a second 0x9BEB0 argument and the forbidden Ordinal_10045 allocation).
// Known gaps: the decompiles of 0x8C230 and 0x8C3B0 never write packet byte 0 (the opcode); it is
// emitted as 0 here and must be pinned against disassembly. Uninitialised stack tails are zeroed.
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>
#include "GAME/SCmd.h"

struct D2GameBatch8AD40Env {
    void (*send8A3E0)(void* client, const void* buf, uint32_t len);                 // 0x8A3E0
    // D2Common #10579 QUESTRECORD_CopyRecordToBuffer(record, buf, size, 0)
    void (*copyQuestRecord)(void* record, void* buf, uint32_t size, int32_t zero);
    void (*dfc10)(const void* pkt);                                                 // 0xDFC10 (single packet arg)
    uint16_t (*sub9BEB0)(void* unit);                                               // 0x9BEB0
};

static inline void w32(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
static inline void w16(uint8_t* p, uint32_t o, uint16_t v) { std::memcpy(p + o, &v, 2); }

// 0x8AD40: 0x67-byte packet: [0]=AL, [1]=CL, [2..5]=EDX, [6]=flag (stack arg 2), [7..0x66]=quest record bytes (0x60).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8ad40(uint32_t eaxIn, uint32_t ecxIn, uint32_t edxIn, void* client, void* record, uint8_t param5, const D2GameBatch8AD40Env* env) { // 1.13c D2Game+0x8AD40
    uint8_t b[0x67] = {};
    b[0] = (uint8_t)eaxIn;
    b[1] = (uint8_t)ecxIn;
    w32(b, 2, edxIn);
    b[6] = param5;
    env->copyQuestRecord(record, b + 7, 0x60, 0);
    env->send8A3E0(client, b, 0x67);
}

// 0x8B1F0: 6-byte packet 0x79: [1]=CL, [2..5]=EAX.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8b1f0(uint32_t eaxIn, uint32_t ecxIn, D2ClientStrc* client) { // 1.13c D2Game+0x8B1F0
    uint8_t b[8] = {};
    b[0] = 0x79;
    b[1] = (uint8_t)ecxIn;
    w32(b, 2, eaxIn);
    D2GAME_PACKETS_SendPacket_6FC3C710(client, b, 6);
}

// 1.13c D2Game+0x8B2F0 (EAX = source, client on the stack): relays the 0x15-byte block at EAX verbatim.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8b2f0(D2ClientStrc* client, const void* src) { // 1.13c D2Game+0x8B2F0
    uint8_t b[0x15];
    std::memcpy(b, src, 0x15);
    D2GAME_PACKETS_SendPacket_6FC3C710(client, b, 0x15);
}

// 1.13c D2Game+0x8B400 (EAX = source, client on the stack): relays the 0x28-byte block at EAX; a NUL-terminated name at +8
// of 16 bytes or more is fatal (Fog 10265(0x5DA) -> 10024 -> _exit(-1)).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8b400(D2ClientStrc* client, const void* src) { // 1.13c D2Game+0x8B400
    const char* name = (const char*)src + 8;
    if (strnlen(name, 32) > 15) _exit(-1);
    uint8_t b[0x28];
    std::memcpy(b, src, 0x28);
    D2GAME_PACKETS_SendPacket_6FC3C710(client, b, 0x28);
}

// 0x8CFB0: the party broadcast lives in d2game_8cfb0.cpp.
void D2Game_8CFB0_PartyBroadcast(void (*)(const void*, uint32_t), uint16_t (*)(const void*), const uint8_t*, uint32_t);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8cfb0(const uint8_t* unit, const D2GameBatch8AD40Env* env) {
    static const D2GameBatch8AD40Env* s_env;
    s_env = env;
    D2Game_8CFB0_PartyBroadcast([](const void* p, uint32_t) { s_env->dfc10(p); }, [](const void* u) { return s_env->sub9BEB0((void*)u); }, unit, 0);
}
