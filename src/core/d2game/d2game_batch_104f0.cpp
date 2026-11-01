// src/core/d2game/d2game_batch_104f0.cpp -- three 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// The three are the unit-mode transitions 0x10A20 (10A20.cpp in this directory) dispatches to for
// modes 5, 6, 7 and 2. Register only arguments (Ghidra's in_EAX / unaff_ESI / unaff_EBX, the ECX of
// the __fastcall) are explicit parameters and are named at each function; the disassembly fixes which
// live register each one is. Unmapped callees are seams on D2GameBatch104F0Env in retail argument order.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

// Host seams for the callees the cached decompiles name but src/ does not implement.
struct D2GameBatch104F0Env {
    // 0xE210: EDI is the unit-info pointer; resets the unit's three skill slots (10312/10525), then
    // 10611(unit,0), 10883(unit,0) and 10536(unit,-1).
    void     (*subE210)(const void* info);
    void     (*ordinal10414)(uint32_t unit, uint32_t value);   // D2Common #10414
    int32_t  (*ordinal10411)(uint32_t unit);                   // D2Common #10411, result feeds #10614/#10513
    void     (*ordinal10614)(int32_t prev, uint32_t value);    // D2Common #10614, ECX = #10411's result
    void     (*ordinal10875)(uint32_t unit, uint32_t mode);    // D2Common #10875
    int32_t  (*ordinal10513)(int32_t prev);                    // D2Common #10513, ECX = #10411's result
    int32_t  (*ordinal11036)(uint32_t unit);                   // D2Common #11036
    int16_t  (*ordinal10718)(uint32_t unit);                   // D2Common #10718
    // 0xEE90(unit, record): EAX/stack pair; record is also EDI.
    void     (*subEE90)(uint32_t unit, const void* record);
    // 0x103C0(info, record): stack pair.
    uint32_t (*sub103C0)(const void* info, const void* record);
    // 0x10550 / 0x10600: unit and record in registers, the caller's EBX as a discarded stack word.
    void     (*sub10550)(uint32_t unit, const void* record, const void* stackArg);
    void     (*sub10600)(uint32_t unit, const void* record, const void* stackArg);
    // 0x14F20 / 0x15240: ESI is the unit, the caller's EBX is the stack word.
    int32_t  (*sub14F20)(uint32_t unit, const void* record);
    int32_t  (*sub15240)(uint32_t unit, const void* record);
};

// Enter mode 4 from mode 5/6/7: reset the unit, hand the out word to #10414/#10614, run 0xEE90, set mode
// 4 (on the unit and on the record's +0x30), then #10411/#10513 refresh the out word and 0x103C0 runs the
// attack tail. Answers 0x103C0's result. EAX is the info pointer (its first dword is the unit), ESI the
// out word and EBX the record.
// 1.13c D2Game+0x104F0
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_104F0(void* info, uint32_t* out, void* record,
                                                 const D2GameBatch104F0Env* env)
{
    env->subE210(info);
    env->ordinal10414(rd32(info, 0), *out);
    env->ordinal10614(env->ordinal10411(rd32(info, 0)), *out);
    env->subEE90(rd32(info, 0), record);
    env->ordinal10875(rd32(info, 0), 4);
    wr32(record, 0x30, 4);
    *out = (uint32_t)env->ordinal10513(env->ordinal10411(rd32(info, 0)));
    return env->sub103C0(info, record);
}

// The mode 7 variant: the same preamble with mode 6, then #11036 must be non-zero; #10718 picks the tail
// -- non-zero answers 0x15240's result, zero answers 0x14F20's. ECX is the info pointer, EAX the out word
// and EBX the record.
// 1.13c D2Game+0x10730
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_10730(void* info, uint32_t* out, void* record,
                                                const D2GameBatch104F0Env* env)
{
    env->subE210(info);
    env->ordinal10414(rd32(info, 0), *out);
    env->ordinal10614(env->ordinal10411(rd32(info, 0)), *out);
    env->subEE90(rd32(info, 0), record);
    env->ordinal10875(rd32(info, 0), 6);
    wr32(record, 0x30, 6);
    *out = (uint32_t)env->ordinal10513(env->ordinal10411(rd32(info, 0)));

    const uint32_t unit = rd32(info, 0);
    if (env->ordinal11036(unit) == 0) return 0;
    if (env->ordinal10718(unit) != 0) return env->sub15240(unit, record);
    return env->sub14F20(unit, record);
}

// Mode 2 dispatch: the unit's #10718 decides between 0x10600 (zero) and 0x10550 (non-zero). ESI is the
// unit, EAX the record and EBX the stack word both callees discard.
// 1.13c D2Game+0x107B0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_107B0(uint32_t unit, void* record, void* stackArg,
                                             const D2GameBatch104F0Env* env)
{
    if (env->ordinal10718(unit) == 0) {
        env->sub10600(unit, record, stackArg);
        return;
    }
    env->sub10550(unit, record, stackArg);
}
