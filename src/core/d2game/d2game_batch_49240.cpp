// src/core/d2game/d2game_batch_49240.cpp -- six 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): the per-client update-record
// accessor, the three client-update packet writers (0x96/0x95/0x18) and the two stat-block writers
// (0x19-0x1C delta frames, 0x1D-0x1F absolute frames). Base 0x6FC20000.
//
// Retail module state is opaque host records. The update writers stage a bit-packed payload through
// Ordinal_10126 (open), Ordinal_10128 (append field: writer, value, bit width) and Ordinal_10127
// (close/flush), then hand the buffer and its length to 0x8A3E0 (queue). Those five callees are seams
// in D2GameBatchB31Env, in retail argument order. The fatal path Ordinal_10265(code) ->
// Ordinal_10024(&0x6FD19FF9) -> _exit(-1) is d2game_fatal, as in the other batches.
//
// The 0x10128 call sites in the three packet writers were recovered with register operands; Ghidra
// materialises the shared writer object at each site as the address of a different stack slot. The
// transcription below uses one writer object and keeps the field widths, the leading constants and
// the value expressions exactly as decompiled.
#include <cstdint>
#include <cstddef>
#include <unistd.h>
#include <emscripten.h>

static inline uint16_t rd16(const uint8_t* p, uint32_t o)
{
    return (uint16_t)(p[o] | ((uint16_t)p[o + 1] << 8));
}
static inline void put16(uint8_t* p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}
static inline void put32(uint8_t* p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees the cached decompiles name but src/ does not implement.
struct D2GameBatchB31Env {
    // Ordinal_10126: open the bit writer over (writer state, packet bytes, byte size).
    void     (*bitWriterInit)(void* writer, void* packet, uint32_t size);
    // Ordinal_10128: append one field (value in the low bits, nBits wide).
    void     (*bitWriterWrite)(void* writer, uint32_t value, uint32_t nBits);
    // Ordinal_10127: close the writer and return the packed length.
    uint32_t (*bitWriterFlush)(void* writer);
    // 0x8A3E0: queue the staged packet bytes.
    void     (*sendPacket)(const void* packet, uint32_t length);
};

// 1.13c D2Game+0x49240 (file:function d2game_batch_49240.cpp:D2Game_GetClientUpdateRecord)
extern "C"
EMSCRIPTEN_KEEPALIVE void* D2Game_GetClientUpdateRecord(void* client)
{
    // FUN_6fc69240: the per-client update record used by 0xCC030. The client arrives in EAX; NULL is
    // the 0xCFC fatal through Ordinal_10265/Ordinal_10024. The record lives at client+0x48C.
    if (client == nullptr) d2game_fatal(0xCFC);
    return (uint8_t*)client + 0x48C;
}

// 1.13c D2Game+0x8BDA0
extern "C"
EMSCRIPTEN_KEEPALIVE void D2GAME_PACKETS_SendPacket0x1A_B_C_6FC3D410(uint32_t context, uint32_t value, uint32_t previous,
                                             const D2GameBatchB31Env* env)
{
    // FUN_6fcabda0: send one changed stat as a delta frame when the delta is non-negative and fits in
    // 16 bits. context (ECX) is never read; previous arrives in ESI. Opcode 0x1A carries a 1-byte
    // delta, 0x1B a 2-byte delta, and 0x1C the 4-byte absolute value; the value always follows the
    // opcode byte.
    (void)context;
    uint8_t packet[5];
    const int32_t delta = (int32_t)(value - previous);
    if (delta >= 0 && delta < 0xFFFF) {
        if (delta < 0xFF) {
            packet[0] = 0x1A;
            packet[1] = (uint8_t)delta;
            env->sendPacket(packet, 2);                        // 0x8A3E0
            return;
        }
        packet[0] = 0x1B;
        put16(packet + 1, (uint32_t)delta);
        env->sendPacket(packet, 3);                            // 0x8A3E0
        return;
    }
    packet[0] = 0x1C;
    put32(packet + 1, value);
    env->sendPacket(packet, 5);                                // 0x8A3E0
}

// 1.13c D2Game+0x8BE10 (d2game_batch_49240.cpp:d2game_send_stat_change)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2game_send_stat_change(uint32_t context, uint32_t value, uint32_t previous,
                                             const D2GameBatchB31Env* env)
{
    // FUN_6fcabe10: send one changed stat as an increase frame when the increase is positive and
    // below 0xFF (opcode 0x19, 1-byte delta). Otherwise fall back to an absolute frame with the
    // second byte 0x0E: 0x1D carries a 1-byte value, 0x1E a 2-byte value, 0x1F a 4-byte value.
    // context (ECX) is never read; previous arrives in ESI.
    (void)context;
    uint8_t packet[6];
    const int32_t delta = (int32_t)(value - previous);
    if (delta > 0 && delta < 0xFF) {
        packet[0] = 0x19;
        packet[1] = (uint8_t)delta;
        env->sendPacket(packet, 2);                            // 0x8A3E0
        return;
    }
    if (value < 0xFF) {
        packet[0] = 0x1D;
        packet[1] = 0x0E;
        packet[2] = (uint8_t)value;
        env->sendPacket(packet, 3);                            // 0x8A3E0
        return;
    }
    if (value < 0xFFFF) {
        packet[0] = 0x1E;
        packet[1] = 0x0E;
        put16(packet + 2, value);
        env->sendPacket(packet, 4);                            // 0x8A3E0
        return;
    }
    packet[0] = 0x1F;
    packet[1] = 0x0E;
    put32(packet + 2, value);
    env->sendPacket(packet, 6);                                // 0x8A3E0
}

// 0x8B750 / 0x8B7F0 / 0x8B8C0 (the 0x96 / 0x95 / 0x18 packets) are d2game_8b750.cpp, d2game_8b7f0.cpp, d2game_8b8c0.cpp.
