// Seam table of d2game_batch_D4630.cpp (cut out of the port so the FN probes build the same struct).
#pragma once
#include <cstdint>

// Host seams for the callees and data tables the cached decompiles name but src/ does not implement.
struct D2GameBatchD4630Env {
    // 0xD43E0(param_1): the reset behind 0xD4630's non-null, in_EAX-non-zero path.
    void     (*subD43E0)(void* param1);
    // D2Common #11105(ptr, len, 1, &out): decode one spawn record, out receives the bytes consumed.
    void     (*ordinal11105)(uint32_t ptr, uint32_t len, uint32_t flag, int32_t* out);
    // 0x2EAE0(a, ptr, len, 1, &out, 0x60): create one unit from the record stream, out = consumed.
    void*    (*sub2EAE0)(uint32_t a, uint32_t ptr, uint32_t len, uint32_t flag, int32_t* out,
                         uint32_t mode);
    // D2Common #10387(0, unit): the unit's pet/hireling list head.
    void*    (*ordinal10387)(uint32_t a, void* unit);
    // D2Common #10405(count, unit, 1): append the unit to that list.
    void     (*ordinal10405)(int32_t count, void* unit, uint32_t flag);
    // 0xD47C0(): the per-spawn tail of the 0x1F9F0 loop.
    void     (*subD47C0)();
    // D2Common #10830(param6): non-zero keeps 0xCA410 off its second decrement.
    int32_t  (*ordinal10830)(void* param6);
    // D2Common #11017(edi+0x60): non-zero means the edi unit's list is non-empty.
    int32_t  (*ordinal11017)(uint32_t value);
    // PTR_FUN_6FD1A400: the registered (script) callback 0x37E20 fires before its two sends.
    uint32_t (*fn6FD1A400)(int32_t one, uint32_t stack4, uint32_t stack8);
    // 0xAEA80(game, unit, mode, value, 0, 0, 0): the seven-argument send/action tail.
    void     (*subAEA80)(uint32_t param1, uint32_t unit, uint32_t mode, int32_t value,
                         uint32_t zero1, uint32_t zero2, uint32_t zero3);
    // D2Common #10561(param_2, out): fills the 32-byte payload the action callback receives.
    void     (*ordinal10561)(uint32_t param2, void* out);
    // DAT_6FD2E2FC: 8-byte entries; word 0 a flag, word 1 the action callback (called with the list).
    const uint8_t* actionTable;
    // D2Common #10920: UNITS_GetPlayerData / the owner record behind a unit.
    uint32_t (*ordinal10920)();
    // D2Common #10920(unit): the four-argument form 0xB2C20 uses on a resolved unit.
    uint32_t (*ordinal10920Unit)(uint32_t unit);
    // sgptDataTables: the count at +0xBF0 gates the 0x96920 walk.
    const uint8_t* dataTables;
    // 0xAB220(eight arguments): the index-7 skill dispatch of the 0x96920 walk.
    void     (*subAB220)(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f,
                         uint32_t g, uint32_t h);
    // 0xAB290(player, index, id, value, word): the other indices of that walk.
    void     (*subAB290)(uint32_t player, int32_t index, uint32_t id, uint32_t value, uint32_t word);
    // 0x8A3E0(blob, mode): the tail of 0xB2760 / 0x35B60.
    void     (*sub8A3E0)(void* blob, uint32_t mode);
    // 0x54C70(value, mode): the id lookup that gates 0x35B60's tail.
    int32_t  (*sub34C70)(int32_t value, int32_t mode);
    // 0xE03A0(a, b): resolves a unit's record from its +0x1A8/+0x170 pair.
    uint32_t (*subE03A0)(uint32_t a, uint32_t b);
    // 0xCBB10(unit, flag): the flag-driven update of 0x788B0.
    void     (*subCBB10)(uint32_t unit, uint32_t flag);
    // 0xCBBB10(0, unit), 0xCAEC0(), 0xCAE60(unit, 0), 0xEB8F0(this, unit): the 0x788B0 tail.
    void     (*subCBBB10)(void* zero, void* unit);
    void     (*subCAEC0)();
    void     (*subCAE60)(uint32_t unit, uint32_t zero);
    void     (*subEB8F0)(void* thisp, uint32_t unit);
    // 0x9ADF0(this, unit, player): the common tail of 0x788B0.
    void     (*sub9ADF0)(void* thisp, void* unit, uint32_t player);
    // 0x9AC50(), 0x9ACB0(player), 0xEBBB0(): bit-gated extras of that tail.
    void     (*sub9AC50)();
    void     (*sub9ACB0)(int32_t player);
    void     (*subEBbb0)();
    // D2Common #11149() -> 0x9A5E0(unit, player), #10354() -> 0x9A570().
    int32_t  (*ordinal11149)();
    void     (*sub9A5E0)(void* unit, int32_t player);
    int32_t  (*ordinal10354)();
    void     (*sub9A570)();
    // D2Common #10771: called five times unconditionally.
    void     (*ordinal10771)();
    // 0x9DCD0(param_1): the per-node call of the 0x9DE10 list walk.
    void     (*sub9DCD0)(void* param1);
    // 0x4AD10(0): the client's player unit (ECX is a leaked this the callees ignore).
    void*    (*sub4AD10)(uint32_t zero);
    // 0x7B0A0(param_1, unit): announce a not-yet-seen unit to the client.
    void     (*sub7B0A0)(void* param1, uint32_t unit);
    // 0x788B0(param_1, eax, flag): type 0 handler.
    void     (*sub788B0)(void* param1, uint32_t eax, int32_t flag);
    // 0xF3A40(this, param_1, unit, eax, flag): type 1 handler.
    void     (*subF3A40)(void* param1, uint32_t unit, uint32_t eax, int32_t flag);
    // 0x6B200(this, eax): type 2 handler.
    void     (*sub6B200)(uint32_t eax);
    // 0xD4550(): type 4 handler.
    void     (*subD4550)();
    // 0x8A3E0 as retail calls it from 0xB2760: EAX = client, stack (buffer, length).
    void     (*sub8A3E0c)(uint32_t client, const void* buf, uint32_t len);
    // 0xE03A0 with all three register inputs (EAX = [+0x16C], ECX = [+0x1A8], EDX = [+0x170]).
    uint32_t (*subE03A0x)(uint32_t eax, uint32_t ecx, uint32_t edx);
};
