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

struct D2GameBatch8AD40Env {
    void (*send8A3E0)(void* client, const void* buf, uint32_t len);                 // 0x8A3E0
    // D2Common #10579 QUESTRECORD_CopyRecordToBuffer(record, buf, size, 0)
    void (*copyQuestRecord)(void* record, void* buf, uint32_t size, int32_t zero);
    void (*dfc10)(const void* pkt);                                                 // 0xDFC10 (single packet arg)
    uint16_t (*sub9BEB0)(void* unit);                                               // 0x9BEB0
};

static inline void w32(uint8_t* p, uint32_t o, uint32_t v) { std::memcpy(p + o, &v, 4); }
static inline void w16(uint8_t* p, uint32_t o, uint16_t v) { std::memcpy(p + o, &v, 2); }

// 0x8AD40: 0x67-byte packet: [0]=ecx low, [1..4]=edx, [5]=param5, [6..0x66]=quest record bytes (0x60).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8ad40(uint32_t ecxIn, uint32_t edxIn, void* client, void* record, uint8_t param5, const D2GameBatch8AD40Env* env) { // 1.13c D2Game+0x8AD40
    uint8_t b[0x67] = {};
    b[0] = (uint8_t)ecxIn;
    w32(b, 1, edxIn);
    b[5] = param5;
    env->copyQuestRecord(record, b + 6, 0x60, 0);
    env->send8A3E0(client, b, 0x67);
}

// 0x8B1F0: 6-byte packet 0x79.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8b1f0(void* client, const D2GameBatch8AD40Env* env) { // 1.13c D2Game+0x8B1F0
    uint8_t b[8] = {};
    b[0] = 0x79;
    env->send8A3E0(client, b, 6);
}

// 0x8B2F0: relays a 0x15-byte block (EAX) verbatim.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8b2f0(void* client, const void* src, const D2GameBatch8AD40Env* env) { // 1.13c D2Game+0x8B2F0
    uint8_t b[0x15];
    std::memcpy(b, src, 0x15);
    env->send8A3E0(client, b, 0x15);
}

// 0x8B400: relays a 0x28-byte block (EAX); the NUL-terminated name at +8 longer than 15 is fatal
// (Fog 10265(0x5DA) -> 10024 -> _exit(-1)).
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8b400(void* client, const void* src, const D2GameBatch8AD40Env* env) { // 1.13c D2Game+0x8B400
    const char* name = (const char*)src + 8;
    if (strnlen(name, 32) > 15) _exit(-1);
    uint8_t b[0x28];
    std::memcpy(b, src, 0x28);
    env->send8A3E0(client, b, 0x28);
}

// 0x8C230: 0x10-byte packet: [1]=ecx low, [2..5]=edx, [6]=p4, [7]=p5, [8..11]=p6, [12..13]=p7, [14..15]=p8.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8c230(uint32_t ecxIn, uint32_t edxIn, void* client, uint8_t p4, uint8_t p5, uint32_t p6, uint16_t p7, uint16_t p8, const D2GameBatch8AD40Env* env) { // 1.13c D2Game+0x8C230
    uint8_t b[0x10] = {};
    b[1] = (uint8_t)ecxIn;
    w32(b, 2, edxIn);
    b[6] = p4;
    b[7] = p5;
    w32(b, 8, p6);
    w16(b, 12, p7);
    w16(b, 14, p8);
    env->send8A3E0(client, b, 0x10);
}

// 0x8C3B0: 6-byte packet: [1]=ecx low, [2..5]=edx.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8c3b0(uint32_t ecxIn, uint32_t edxIn, void* client, const D2GameBatch8AD40Env* env) { // 1.13c D2Game+0x8C3B0
    uint8_t b[8] = {};
    b[1] = (uint8_t)ecxIn;
    w32(b, 2, edxIn);
    env->send8A3E0(client, b, 6);
}

// 0x8CFB0: 7-byte packet 0x8D to every unit via 0xDFC10: [1..4]=unit id (-1 if null), [5..6]=0x9BEB0(unit)
// when unit+0x80 is set and *unit==0, else 0xFFFF.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_game_8cfb0(const uint8_t* unit, const D2GameBatch8AD40Env* env) { // 1.13c D2Game+0x8CFB0
    uint8_t b[7];
    uint32_t id = 0xFFFFFFFFu, f0 = 1, f80 = 0;
    b[0] = 0x8D;
    uint16_t v = 0xFFFF;
    if (unit) {
        std::memcpy(&id, unit + 0xC, 4);
        std::memcpy(&f0, unit, 4);
        std::memcpy(&f80, unit + 0x80, 4);
        if (f80 != 0 && f0 == 0) v = env->sub9BEB0((void*)unit);
    }
    w32(b, 1, id);
    w16(b, 5, v);
    env->dfc10(b);
}
