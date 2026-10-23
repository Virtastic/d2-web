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

// 1.13c D2Game+0x8B750
extern "C"
EMSCRIPTEN_KEEPALIVE void d2game_send_update_96(const D2GameBatchB31Env* env)
{
    // FUN_6fcab750: build and send the 0x96 client-update packet. Writer state is Ghidra's
    // local_14[4]+uStack_10, the packet bytes local_20[4]+uStack_18 (ten bytes); the writer is
    // opened with size 9. Fields, in order: constant 0x96 (8 bits), uStack_10 (15), uStack_18 (16),
    // local_20[0] (16), the low byte of the packet address (8), then 8 (8).
    uint8_t state[6] = {};
    uint8_t packet[10] = {};
    env->bitWriterInit(state, packet, 9);                      // Ordinal_10126
    env->bitWriterWrite(state, 0x96, 8);                       // Ordinal_10128
    env->bitWriterWrite(state, rd16(state, 4), 0x0F);          // uStack_10
    env->bitWriterWrite(state, rd16(packet, 8), 0x10);         // uStack_18
    env->bitWriterWrite(state, rd16(packet, 0), 0x10);         // local_20[0]
    env->bitWriterWrite(state, (uint8_t)(uintptr_t)packet, 8); // puStack_28 & 0xFF
    env->bitWriterWrite(state, 8, 8);                          // uStack_30 & 0xFF
    const uint32_t length = env->bitWriterFlush(state);        // Ordinal_10127
    env->sendPacket(packet, length);                           // 0x8A3E0
}

// 1.13c D2Game+0x8B7F0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8B7F0(const D2GameBatchB31Env* env)
{
    // FUN_6fcab7f0: build and send the 0x95 client-update packet. Writer state is local_24[4]+
    // uStack_28, the packet bytes local_14[4]+uStack_10; a third block auStack_20[4]+uStack_18 is
    // written first and re-read below. Open size 0xD. Fields: 0x95 (8), uStack_10 (15), uStack_18
    // (15), auStack_20[0] (15), uStack_28 (16), the low 16 bits of the packet address (16), then the
    // low bytes of auStack_38[0] = 0x95 (8) and uStack_40 = 0xF (8).
    uint8_t state[6] = {};    // local_24[4] + uStack_28
    uint8_t block[10] = {};   // auStack_20[4] + uStack_18
    uint8_t packet[6] = {};   // local_14[4] + uStack_10
    env->bitWriterInit(state, packet, 0x0D);                   // Ordinal_10126
    env->bitWriterWrite(state, 0x95, 8);                       // Ordinal_10128
    env->bitWriterWrite(state, rd16(packet, 4), 0x0F);         // uStack_10
    env->bitWriterWrite(state, rd16(block, 8), 0x0F);          // uStack_18
    env->bitWriterWrite(state, rd16(block, 0), 0x0F);          // auStack_20[0]
    env->bitWriterWrite(state, rd16(state, 4), 0x10);          // uStack_28
    env->bitWriterWrite(state, (uint16_t)(uintptr_t)packet, 0x10); // puStack_30 & 0xFFFF
    env->bitWriterWrite(state, 0x95, 8);                       // auStack_38[0] & 0xFF
    env->bitWriterWrite(state, 0x0F, 8);                       // uStack_40 & 0xFF
    const uint32_t length = env->bitWriterFlush(state);        // Ordinal_10127
    env->sendPacket(packet, length);                           // 0x8A3E0
}

// 1.13c D2Game+0x8B8C0
extern "C"
EMSCRIPTEN_KEEPALIVE void GAME_SendUpdate18_8B8C0(const D2GameBatchB31Env* env)
{
    // FUN_6fcab8c0: build and send the 0x18 client-update packet. Writer state is local_24[4]+
    // uStack_28, the packet bytes local_14[4]+uStack_10; auStack_20[4]+uStack_18 is written first
    // and re-read. Open size 0xF. Fields: 0x18 (8), uStack_10 (15), uStack_18 (15), auStack_20[0]
    // (15), uStack_28 (7), low byte of the packet address (7), auStack_38[0] = 0x18 (16), uStack_40
    // = 0xF (16), low byte of the writer address (8), low byte of uStack_18 (8).
    uint8_t state[6] = {};    // local_24[4] + uStack_28
    uint8_t block[10] = {};   // auStack_20[4] + uStack_18
    uint8_t packet[6] = {};   // local_14[4] + uStack_10
    env->bitWriterInit(state, packet, 0x0F);                   // Ordinal_10126
    env->bitWriterWrite(state, 0x18, 8);                       // Ordinal_10128
    env->bitWriterWrite(state, rd16(packet, 4), 0x0F);         // uStack_10
    env->bitWriterWrite(state, rd16(block, 8), 0x0F);          // uStack_18
    env->bitWriterWrite(state, rd16(block, 0), 0x0F);          // auStack_20[0]
    env->bitWriterWrite(state, rd16(state, 4) & 0xFF, 7);      // uStack_28 & 0xFF
    env->bitWriterWrite(state, (uint8_t)(uintptr_t)packet, 7); // puStack_30 & 0xFF
    env->bitWriterWrite(state, 0x18, 0x10);                    // auStack_38[0] & 0xFFFF
    env->bitWriterWrite(state, 0x0F, 0x10);                    // uStack_40 & 0xFFFF
    env->bitWriterWrite(state, (uint8_t)(uintptr_t)state, 8);  // ppuStack_48 & 0xFF
    env->bitWriterWrite(state, rd16(block, 8) & 0xFF, 8);      // auStack_50[0] & 0xFF
    const uint32_t length = env->bitWriterFlush(state);        // Ordinal_10127
    env->sendPacket(packet, length);                           // 0x8A3E0
}
