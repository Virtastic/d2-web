// src/core/d2game/d2game_batch_10160.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c), in RVA order. Stage A batch
// lane: implementation + marker only, no spec, no vector, no test runner.
//
// These are the game/client mode-transition and packet-send helpers around GAME_UpdateServerInfo:
// 0x10160 detaches a unit, 0x10240 / 0x10550 / 0x10600 dispatch on the client mode and the packet
// type, 0x11880 / 0x11E20 / 0x11FB0 / 0x12230 build the 0x84-byte client->server packet and hand it
// to FUN_6fc31490, 0x12AF0 / 0x12B90 / 0x12DF0 / 0x12EE0 are the gold/exp grant and per-minute update
// path. Retail is 32-bit: every record field is a byte offset; the register-only arguments Ghidra
// leaves as in_EAX / unaff_ESI / unaff_EBX / unaff_EDI / unaff_EBP are explicit parameters here and
// are named at each definition (the disassembly, not this file, fixes which live register each one is).
//
// Callees outside this batch are seams on D2GameBatch10160Env, in retail argument order. Two seams
// carry the same ordinal twice where the two call sites demonstrably pass different argument lists in
// registers (`_v` = the no-visible-argument form the decompile shows at that site); the callee itself
// is the same retail address. The stack packet frames are the 0x21 zeroed dwords retail builds; the
// offsets below are EBP-relative, taken from the frame variable names in each decompile (e.g.
// stack0xffffff64 is EBP-0x9C). Bytes retail leaves as stack garbage are zeroed here so a replay has a
// fixed starting value.
//
// Base 0x6FC20000. _exit(-1) on the 0x10240 fatal path is the same tail as d2game_batch_03520.cpp.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void     wr64(void* p, uint32_t o, uint64_t v) { std::memcpy((uint8_t*)p + o, &v, 8); }
static inline void     wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }
static inline int32_t  asI32(void* p) { return (int32_t)(intptr_t)p; }
static inline int32_t  asI32n(const void* p) { return (int32_t)(intptr_t)p; }

// 0x6FD19FF9: the module-name literal the 0x10240 fatal path passes to Ordinal_10024.
static const char kModuleName[] = "D2Game.dll";

// Every callee of the twelve that src/ does not implement, at its retail address.
struct D2GameBatch10160Env {
    // ---- 0x10160 ------------------------------------------------------------------------------
    void     (*ordinal10977_v)(void* unit);           // Ordinal_10977, unit in a register
    void     (*ordinal11166)(void);                   // Ordinal_11166
    int32_t  (*ordinal10695)(uint32_t unitId);        // Ordinal_10695
    void     (*sub55890)(void);                       // 0x55890
    void     (*subAE880)(void* unit);                 // 0xAE880
    // ---- 0x10240 / 0x10550 / 0x10600 ----------------------------------------------------------
    int16_t  (*ordinal10718)(void);                   // Ordinal_10718
    uint32_t (*sub348A0)(void* unit, uint32_t a, uint32_t b, uint32_t c, uint32_t d);  // 0x348A0
    uint32_t (*sub344D0)(void* unit, uint32_t a, uint32_t b, uint32_t c, uint32_t d,
                         int32_t e, uint32_t f);      // 0x344D0
    void     (*ordinal10312)(void);                   // Ordinal_10312
    void     (*ordinal10525_v)(void* unit);           // Ordinal_10525, unit in a register
    void     (*ordinal10802)(void);                   // Ordinal_10802
    uint32_t (*ordinal10265)(uint32_t id);            // Ordinal_10265
    void     (*ordinal10024)(const char* module, uint32_t id);  // Ordinal_10024, tail is _exit(-1)
    int32_t  (*ordinal10744)(void);                   // Ordinal_10744
    int32_t  (*sub332A0)(void* unit);                 // 0x132A0
    void     (*ordinal10536_v)(void* unit);           // Ordinal_10536, unit in a register
    void     (*sub33DE0)(void* unit);                 // 0x13DE0
    uint32_t (*ordinal11088)(void* client);           // Ordinal_11088
    void     (*sub2EC90)(void);                       // 0xEC90
    int32_t* (*sub332A0r)(void* client);              // 0x132A0, the 0x10600 result form
    void     (*ordinal10525)(void* client, int32_t v, uint32_t zero);  // Ordinal_10525
    void     (*ordinal10536)(void* client, int32_t v);                 // Ordinal_10536
    void     (*ordinal10977)(void* pkt, uint32_t a, uint32_t b);       // Ordinal_10977
    // ---- 0x11880 / 0x11E20 / 0x11FB0 / 0x12230 ------------------------------------------------
    void     (*ordinal10141_v)(void);                 // Ordinal_10141
    void     (*ordinal10141)(void* game, void* out8); // Ordinal_10141 (0x12B90's form)
    uint32_t (*ordinal10331_v)(void);                 // Ordinal_10331
    void     (*ordinal10331)(void* p);                // Ordinal_10331 (0x12B90's form)
    int32_t  (*sub2E930)(void* a, void* b);           // 0x2E930 (a handle or a pointer, per site)
    uint32_t (*sub2E5D0)(uint32_t zero);              // 0x2E5D0
    void*    (*sub31490)(void* game, void* pkt);      // 0x31490
    uint32_t (*ordinal10826)(void);                   // Ordinal_10826
    int32_t  (*ordinal10894)(uint32_t v, uint8_t a, uint32_t b);    // Ordinal_10894
    int32_t  (*sub2F8F0)(void* esi, void* p, int32_t n, int32_t arg);  // 0x2F8F0
    int32_t  (*ordinal11064)(int32_t v);              // Ordinal_11064
    int32_t  (*sub21280)(void);                       // 0x21280
    int32_t  (*ordinal10654)(void* a, void* b, void* c, int32_t d, int32_t e, int32_t f, int32_t g); // Ordinal_10654
    int32_t  (*sub2FA30)(void* ebx, void* p, int32_t n, int32_t arg);  // 0x2FA30
    int32_t  (*sub2FFC0)(void* a, uint32_t b, uint32_t c, uint32_t d); // 0x2FFC0
    // ---- 0x12AF0 / 0x12B90 / 0x12DF0 / 0x12EE0 ------------------------------------------------
    int32_t  (*ordinal10450_v)(void);                 // Ordinal_10450
    int32_t  (*ordinal10450)(uint32_t tag, void* out); // Ordinal_10450 (0x12B90's form)
    int32_t  (*ordinal10973)(void);                   // Ordinal_10973
    int32_t  (*sub31980)(int32_t a, int32_t b, void* c, uint32_t d, void* e, int32_t f,
                         int32_t g, void* h, uint32_t i, uint32_t j);  // 0x31980
    void     (*sub2E990)(int32_t p);                  // 0x2E990
    int32_t  (*ordinal10049_v)(void);                 // Ordinal_10049
    int32_t  (*ordinal10049)(void* p);                // Ordinal_10049 (packet form)
    void     (*ordinal10887_v)(void);                 // Ordinal_10887
    void     (*ordinal10887)(void* p, uint32_t a, uint32_t b, uint32_t c);  // Ordinal_10887
    void     (*ordinal10551)(void);                   // Ordinal_10551
    int32_t  (*ordinal10865)(void);                   // Ordinal_10865
    int32_t  (*ordinal10711)(void);                   // Ordinal_10711
    void     (*ordinal11116)(void);                   // Ordinal_11116
    int32_t  (*sub2EA70)(void* v);                    // 0x2EA70
    void     (*ordinal10920)(void);                   // Ordinal_10920
    int32_t  (*subAB010)(void);                       // 0xAB010
    int32_t  (*ordinal10202)(void);                   // Ordinal_10202
    int32_t  (*subF5740)(void* edi, void* esi);       // 0xF5740
    void     (*sub99100)(uint32_t v);                 // 0x99100
    int32_t  (*ordinal10463)(void);                   // Ordinal_10463
};

// 0x10160: detach the unit -- stamp it with EAX, clear flag 0x8 and set bit 1 of +0xC4, run the two
// global updates, and if the owner record's +0x12A is set run 0x55890. Then release the unit's +0xDC
// object and clear the slot.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_10160(void* unit, uint32_t eax, const D2GameBatch10160Env* env) {  // 1.13c D2Game+0x10160
    if (unit != nullptr) {
        wr32(unit, 0x0C, eax);
        wr32(unit, 0xC4, (rd32(unit, 0xC4) & 0xFFFFFFF7u) | 2u);
    }
    env->ordinal10977_v(unit);
    env->ordinal11166();
    const int32_t rec = env->ordinal10695(unit != nullptr ? rd32(unit, 4) : 0xFFFFFFFFu);
    if (rd8((const void*)(intptr_t)rec, 0x12A) != 0) env->sub55890();
    if (unit != nullptr) {
        if (rd32(unit, 0xDC) != 0) env->subAE880(unit);
        wr32(unit, 0xDC, 0);
    }
}

// 0x10240: emit one of two server->client status packets depending on the client's +0x68 / +0x74
// counters, then refresh the client. When both packets came back empty the module dies with 0x372.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_10240(void* client, void* unit, uint32_t ebx, const D2GameBatch10160Env* env) {  // 1.13c D2Game+0x10240
    int32_t v = (int32_t)rd32(client, 0x68);
    int16_t s3 = 0, s2 = 0;
    if (v < 1) {
        v = (int32_t)rd32(client, 0x74);
        if (v < 1) {
            s3 = env->ordinal10718();
            if (s3 == 0) s3 = (int16_t)env->sub348A0(unit, 1, 0, 1, 1);
            else         s3 = (int16_t)env->sub344D0(unit, 1, 0, 1, 1, v, 0);
            env->ordinal10312();
        }
    } else {
        s2 = env->ordinal10718();
        if (s2 == 0) { s2 = (int16_t)env->sub348A0(unit, 1, 1, 1, 1); env->ordinal10312(); }
        else         { s2 = (int16_t)env->sub344D0(unit, 1, 1, 1, 1, v, 0); env->ordinal10312(); }
    }
    v = (int32_t)rd32(client, 0x74);
    bool refresh = true;
    if (v < 1) {
        v = (int32_t)rd32(client, 0x68);
        if (0 < v) refresh = false;                       // LAB_6fc30373
        else {
            s2 = env->ordinal10718();
            if (s2 == 0) s2 = (int16_t)env->sub348A0(unit, 1, ebx, 1, 0);
            else         s2 = (int16_t)env->sub344D0(unit, 1, ebx, 1, 0, v, 0);
        }
    } else {
        s3 = env->ordinal10718();
        if (s3 == 0) s3 = (int16_t)env->sub348A0(unit, 1, 1, 1, 0);
        else         s3 = (int16_t)env->sub344D0(unit, 1, 1, 1, 0, v, 0);
    }
    if (refresh) env->ordinal10525_v(unit);
    if (s2 == 0 && s3 == 0) {
        env->ordinal10802();
        const uint32_t id = env->ordinal10265(0x372);
        env->ordinal10024(kModuleName, id);
        _exit(-1);
    }
    env->ordinal10977_v(unit);
    return 1;
}

// 0x10550: bring the game up -- mode packet, client reset, two skill passes, then the final flush.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_10550(void* edi, void* esi, uint32_t ebx, const D2GameBatch10160Env* env) {  // 1.13c D2Game+0x10550
    if (env->ordinal10744()) d2_fn_d2game_10240(edi, esi, ebx, env);
    if (env->ordinal10744()) env->ordinal10536_v(edi);
    if (env->ordinal10744()) { env->ordinal10536_v(edi); env->ordinal10977_v(edi); }
    if (env->ordinal10744()) { env->sub332A0(edi); env->ordinal10525_v(edi); }
    if (env->ordinal10744()) { env->sub332A0(edi); env->ordinal10525_v(edi); }
    env->sub33DE0(edi);
}

// 0x10600: dispatch one client packet. 0x0D hands it to 0x10240; 0x12/0x16 send the skill refresh;
// 0x07/0x28 send the client's +0x40 (or the record's +4) word. Everything else tears the client down.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_10600(void* param_1, void* record, const D2GameBatch10160Env* env) {  // 1.13c D2Game+0x10600
    const uint32_t kind = env->ordinal11088(param_1);
    int32_t iVar2;
    switch (kind) {
    case 7:
    case 0x28: {
        iVar2 = (int32_t)rd32(record, 0x40);              // in_EAX[0x10]
        const void* link = rdptr(record, 0);              // *in_EAX
        if (link != nullptr) iVar2 = (int32_t)rd32(link, 4);
        if (kind == 0x28) { env->ordinal10536(param_1, iVar2); return; }
        break;
    }
    case 0x0D: d2_fn_d2game_10240(record, record, 0, env); return;
    case 0x12:
    case 0x16: env->ordinal10525(param_1, asI32n(env->sub332A0r(param_1)), 0); return;
    default:
        env->sub33DE0(record);
        env->sub2EC90();
        return;
    }
    env->ordinal10536(param_1, iVar2);
    env->ordinal10977(param_1, 0x10000, 1);
}

// 0x11880: build the type-3 client packet for param_2 (rejected when negative) and send it through
// 0x31490. The record at EBX contributes the 0x187/1 pair to the flags word at +0x80.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_11880(void* game, int32_t param_2, uint32_t param_3, uint32_t param_4, uint32_t flags, void* ebx, const D2GameBatch10160Env* env) {  // 1.13c D2Game+0x11880
    if (param_2 < 0) return 0;
    env->ordinal10141_v();
    const uint32_t id = env->ordinal10331_v();
    const int32_t rec = env->sub2E930((void*)(uintptr_t)id, ebx);
    int32_t result = 0;
    if (rec != 0) {
        uint8_t pkt[0x84] = {};
        wr32(pkt, 0x04, 0);                                    // local_98
        wr32(pkt, 0x08, (uint32_t)(uintptr_t)game);            // iStack_94
        wr32(pkt, 0x0C, env->sub2E5D0(0));                     // iStack_90
        wr32(pkt, 0x14, (uint32_t)param_2);                    // iStack_88
        wr32(pkt, 0x18, 3);                                    // uStack_84
        wrptr(pkt, 0x1C, pkt + 0x04);                          // puStack_80 = &local_98
        wr32(pkt, 0x24, (uint32_t)rec);                        // iStack_78
        wr16(pkt, 0x28, 1);                                    // uStack_74
        wr16(pkt, 0x2A, rd16(game, 0x78));                     // uStack_72
        wr32(pkt, 0x30, param_3);                              // uStack_6c
        wr32(pkt, 0x40, param_4);                              // uStack_5c
        uint32_t f = 0;                                        // uStack_1c
        if (ebx != nullptr && rd32(ebx, 0) == 1 && rd32(ebx, 4) == 0x187) f = 1;
        wr32(pkt, 0x80, f | flags);
        result = asI32n(env->sub31490(game, pkt));
    }
    return result;
}

// 0x11E20: up to six probes of 0x2F8F0 for a usable unit; on success build the type-3 packet holding
// the caller's (param_2[0], param_2[1]) pair and send it. Answers 0x31490's result.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_11E20(void* param_1, const int32_t* param_2, uint32_t param_3, int32_t param_4, int32_t param_5, void* esi, const D2GameBatch10160Env* env) {  // 1.13c D2Game+0x11E20
    const uint32_t id = env->ordinal10826();
    int32_t iVar2 = env->ordinal10894(id, rd8(esi, 0x6D), rd32(esi, 0x70));
    if (1 < iVar2) iVar2 -= 1;
    void* puVar5 = param_1 != nullptr ? (uint8_t*)param_1 + 0x6C : nullptr;
    int32_t iVar3 = -1;
    int32_t tries = 0;
    do {
        iVar3 = env->sub2F8F0(esi, puVar5, iVar2, param_4);
        if (-1 < iVar3 && env->ordinal11064(iVar3) != 0) break;
        tries++;
    } while (tries < 6);
    if (-1 < iVar3) {
        uint8_t pkt[0x84] = {};
        const int32_t ac = param_2[0];                         // iStack_ac
        void* const a8 = rdptr(param_2, 4);                    // puStack_a8 = param_2[1]
        const int32_t bumped = ac + 2;
        const int32_t r = env->sub21280();
        wr32(pkt, 0x00, (uint32_t)(r != 0 ? bumped : ac));     // iStack_bc
        wr32(pkt, 0x0C, (uint32_t)tries);                      // iStack_b0: probe count
        wr32(pkt, 0x10, (uint32_t)ac);                         // iStack_ac
        wrptr(pkt, 0x14, a8);                                  // puStack_a8
        wr32(pkt, 0x18, (uint32_t)iVar3);                      // iStack_a4
        if (env->ordinal10654(param_1, pkt, pkt + 0x10, 1, 0x3E01, 0x801, 1) != 0) {
            std::memset(pkt, 0, sizeof pkt);
            wr32(pkt, 0x00, (uint32_t)param_5);                // iStack_bc
            wr16(pkt, 0x2A, rd16(esi, 0x78));                  // uStack_92
            wr32(pkt, 0x80, rd32(pkt, 0x80) | 0x40u);          // uStack_3c
            wr32(pkt, 0x18, 3);                                // iStack_a4
            wr16(pkt, 0x28, 1);                                // uStack_94
            wr32(pkt, 0x30, 0);                                // uStack_8c
            wr32(pkt, 0x0C, 0x801);                            // iStack_b0
            wrptr(pkt, 0x14, puVar5);                          // puStack_a8
            wr32(pkt, 0x1C, 1);                                // uStack_a0
            wr32(pkt, 0x20, 0x3E01);                           // uStack_9c
            return asI32n(env->sub31490(esi, pkt));
        }
    }
    return 0;
}

// 0x11FB0: one 0x2FA30 probe, and on success the type-3 packet carrying param_3. Answers 0x31490.
// In the decompile the two values copied out of EAX (uStack_a8/uStack_a4) are never read again; they
// are dead stores and are not modelled.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_11FB0(void* self, uint32_t param_1, int32_t param_2, int32_t param_3, void* eax_rec, void* ebx_unit, const D2GameBatch10160Env* env) {  // 1.13c D2Game+0x11FB0
    (void)param_1; (void)eax_rec;
    const uint32_t id = env->ordinal10826();
    int32_t iVar2 = env->ordinal10894(id, rd8(ebx_unit, 0x6D), rd32(ebx_unit, 0x70));
    if (1 < iVar2) iVar2 -= 1;
    void* const puVar4 = self != nullptr ? (uint8_t*)self + 0x6C : nullptr;
    iVar2 = env->sub2FA30(ebx_unit, puVar4, iVar2, param_2);
    if (-1 < iVar2) {
        uint8_t out8[8] = {};                                   // aiStack_b0
        if (env->sub2E930(self, out8) != 0) {
            uint8_t pkt[0x84] = {};
            wr32(pkt, 0x00, (uint32_t)param_3);                 // iStack_a0
            wrptr(pkt, 0x04, puVar4);                           // puStack_9c
            return asI32n(env->sub31490(ebx_unit, pkt));
        }
    }
    return 0;
}

// 0x12230: build the type-3 packet for param_2's client, send it, and copy the frame to param_3.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_12230(void* game, uint32_t param_2, void* param_3, void* eax_rec, const D2GameBatch10160Env* env) {  // 1.13c D2Game+0x12230
    const uint32_t id = env->ordinal10826();
    const uint32_t uVar5 = rd32(game, 0x70);
    env->ordinal10894(id, rd8(game, 0x6D), uVar5);
    uint8_t pkt[0x84] = {};
    wr32(pkt, 0x00, param_2);                                  // iStack_bc
    wr32(pkt, 0x10, rd32(eax_rec, 0));                         // uStack_ac = *in_EAX
    wr32(pkt, 0x14, rd32(eax_rec, 4));                         // uStack_a8 = in_EAX[1]
    wr32(pkt, 0x18, (uint32_t)env->sub2FFC0(game, 0xFFFFFFFF, 0, 0));  // iStack_a4
    env->sub21280();
    if (env->ordinal10654((void*)(intptr_t)param_2, pkt + 0x08, pkt + 0x10, 1, 0x3E01, 0x801, 1) != 0) {
        std::memset(pkt, 0, sizeof pkt);
        wr16(pkt, 0x2A, rd16(game, 0x78));                     // uStack_92
        wr32(pkt, 0x00, 0);                                    // iStack_bc
        wr32(pkt, 0x18, 3);                                    // iStack_a4
        wr16(pkt, 0x28, 1);                                    // uStack_94
        wr32(pkt, 0x30, 0);                                    // uStack_8c
        wr32(pkt, 0x14, uVar5);                                // uStack_a8
        wr32(pkt, 0x1C, 0x801);                                // uStack_a0
        wr32(pkt, 0x20, 1);                                    // uStack_9c
        env->sub31490(game, pkt);
        if (param_3 != nullptr) std::memcpy(param_3, pkt, sizeof pkt);
    }
}

// 0x12AF0: when the game is in mode 0 and 0x10450 says "in game", look the unit up and hand it to
// 0x31980 with tag 0x20726165 ('ear '); the created unit is released immediately.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_12AF0(void* param_1, void* edi, const D2GameBatch10160Env* env) {  // 1.13c D2Game+0x12AF0
    if (edi != nullptr && rd32(edi, 0) == 0 && env->ordinal10450_v() != 0) {
        env->ordinal10141_v();
        const uint32_t id = env->ordinal10331_v();
        if (env->sub2E930((void*)(uintptr_t)id, edi) != 0) {
            env->ordinal10973();
            const int32_t unit = env->sub31980(1, 1, edi, 0x20726165, param_1, 4, 2,
                                               edi, 0x0C, 0);
            if (unit != 0) env->sub2E990(unit);
        }
    }
}

// 0x12B90: while param_3 (a duration) runs out, send one gold-grant packet per slice of at most
// 2e9 ticks and sleep the client for that slice. 0x20646c67 is the retail tag looked up by 0x10450.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_12B90(void* game, void* param_2, uint32_t param_3, const D2GameBatch10160Env* env) {  // 1.13c D2Game+0x12B90
    uint8_t pkt[0x84] = {};
    void* const piVar5 = pkt + 0x08;                            // aiStack_9c + 2
    if (env->ordinal10450(0x20646C67, piVar5) == 0) return;
    env->ordinal10331(param_2);
    const uint32_t total = param_3;                             // uStack_a4
    if (param_3 == 0) return;
    uint32_t uVar6 = 0;
    do {
        uint32_t slice = param_3 < 0x77359401u ? param_3 : 2000000000u;   // uVar4
        param_3 -= slice;
        uint8_t scratch[4] = {};                                // stack0xffffff44
        uint8_t out4[4] = {};                                   // auStack_b4
        uint8_t ac8[8] = {};                                    // auStack_ac
        uint32_t st_b0 = 0;                                     // uStack_b0
        uint32_t st_a0 = 0;                                     // uStack_a0
        env->ordinal10141(param_2, ac8);
        const uint32_t three = st_b0 + 3;
        uVar6 = env->sub21280() != 0 ? three : st_b0;
        if (env->ordinal10654(piVar5, scratch, out4, 1, 0x3E01, 0x801, 1) != 0) {
            std::memset(pkt, 0, sizeof pkt);
            wr32(pkt, 0x00, (uint32_t)(uintptr_t)param_2);      // aiStack_9c[0]
            wr32(pkt, 0x04, 0);                                 // aiStack_9c[1]
            wr32(pkt, 0x08, (uint32_t)(uintptr_t)game);         // aiStack_9c[2]
            wr32(pkt, 0x0C, 1);                                 // aiStack_9c[3]
            wr32(pkt, 0x14, st_a0);                             // uStack_88
            wr32(pkt, 0x18, 4);                                 // uStack_84
            wr32(pkt, 0x1C, 0);                                 // uStack_80
            wr32(pkt, 0x20, 0);                                 // uStack_7c
            wr32(pkt, 0x24, 0);                                 // uStack_78
            wr16(pkt, 0x28, 1);                                 // uStack_74
            wr16(pkt, 0x2A, rd16(game, 0x78));                  // uStack_72
            wr32(pkt, 0x30, 2);                                 // uStack_6c
            wr32(pkt, 0x48, 0);                                 // uStack_54
            wr32(pkt, 0x4C, 0);                                 // uStack_50
            wr32(pkt, 0x80, rd32(pkt, 0x80) | 10u);             // uStack_1c
            void* const sent = env->sub31490(game, pkt);
            if (sent != nullptr) {
                env->ordinal10977(sent, 0x10, 1);
                if ((int32_t)slice < 0) slice = 0;
                else if (rd32(sent, 0) == 0 && env->ordinal10049(sent) < (int32_t)slice) slice = 0;
                env->ordinal10887(sent, 0x0E, slice, 0);
                env->sub2E990(asI32n(sent));
            }
        }
    } while (uVar6 < total);
}

// 0x12DF0: pace the caller's wait against the client clock. 0x10049 is now, 0x10973 the client's own
// clock; equal clocks or a wait that fits short-circuit, an overrun re-schedules the remainder.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_12DF0(void* edi, void* esi, uint32_t ebp, const D2GameBatch10160Env* env) {  // 1.13c D2Game+0x12DF0
    const uint32_t now = (uint32_t)env->ordinal10049_v();
    const uint32_t clock = (uint32_t)env->ordinal10973();
    if (clock == now) { d2_fn_d2game_12B90(edi, esi, ebp, env); return; }
    if (clock + ebp <= now) {
        if (esi != nullptr) {
            const int32_t v = env->ordinal10973();
            if (v < 0) { env->ordinal10887_v(); return; }
            if (rd32(esi, 0) == 0) {
                const int32_t cap = env->ordinal10049_v();
                if (cap < v) { env->ordinal10887_v(); return; }
            }
            env->ordinal10551();
        }
        return;
    }
    if (esi != nullptr) {
        const int32_t v = env->ordinal10973() + (int32_t)(now - clock);
        if (v < 0) env->ordinal10887_v();
        else {
            if (rd32(esi, 0) == 0) {
                const int32_t cap = env->ordinal10049_v();
                if (cap < v) { env->ordinal10887_v(); }
                else env->ordinal10551();
            } else {
                env->ordinal10551();
            }
        }
    }
    d2_fn_d2game_12B90(edi, esi, (clock - now) + ebp, env);
}

// 0x12EE0: the per-tick client update. When both 0x10744 predicates are clear, or 0x10865 is, do
// nothing. Otherwise advance the caller's 64-bit LCG at EDI+0x20 and, when `lo % 100` is under the
// rate (4, or 10 when the 0x10711/0x70 path runs), pick the update branch by 0x10973 / 0x10202 /
// 0x10718 and end with the 0x10463 sweep.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_12EE0(void* edi, void* esi, void* ebx, const D2GameBatch10160Env* env) {  // 1.13c D2Game+0x12EE0
    int32_t iVar3 = env->ordinal10744();
    const int32_t iVar4 = env->ordinal10744();
    if (iVar3 == 0 && iVar4 == 0) return 0;
    if (env->ordinal10865() == 0) return 0;
    uint32_t rate = 4;
    if (iVar3 == 0) {
        iVar3 = env->ordinal10711();
        if (iVar3 != 0) {
            if (rd32(esi, 0x70) == 0) return asI32n(esi);
            rate = 10;
        }
    }
    const uint64_t next = (uint64_t)(uint32_t)rd32(edi, 0x20) * 0x6AC690C5ull +
                          (uint64_t)(uint32_t)rd32(edi, 0x24);
    wr64(edi, 0x20, next);
    const uint32_t lo = (uint32_t)next;
    iVar3 = (int32_t)(lo / 100);
    if ((lo % 100) < rate) {
        iVar3 = env->ordinal10973();
        if (0 < iVar3 - 1) {
            env->ordinal11116();
            env->sub2EA70(ebx);
            env->ordinal10887_v();
            if (edi != nullptr && rd32(edi, 0) == 0) env->ordinal10920();
            return env->subAB010();
        }
        iVar3 = env->ordinal10202();
        if (iVar3 == 0) return env->subF5740(edi, esi);
        const int16_t sVar2 = env->ordinal10718();
        if (sVar2 == 0) {
            env->ordinal10887_v();
            if (edi != nullptr && rd32(edi, 0) == 0) env->ordinal10920();
            iVar3 = env->subAB010();
        } else {
            iVar3 = env->ordinal10973();
            if (iVar3 < 2) {
                env->ordinal10887_v();
                if (edi != nullptr && rd32(edi, 0) == 0) env->ordinal10920();
                env->subAB010();
                const uint32_t zero = 0;                          // uVar6
                env->ordinal10887_v();
                if (edi != nullptr && rd32(edi, 0) == 0) env->ordinal10920();
                env->subAB010();
                env->sub99100(zero);
            } else {
                env->ordinal10887_v();
                if (edi != nullptr && rd32(edi, 0) == 0) env->ordinal10920();
                env->subAB010();
                env->ordinal11116();
                env->ordinal10887_v();
                if (edi == nullptr || rd32(edi, 0) != 0) env->subAB010();
                else { env->ordinal10920(); env->subAB010(); }
            }
            iVar3 = env->ordinal10463();
            if (0 < iVar3) return env->sub2EA70(nullptr);
        }
    }
    return iVar3;
}
