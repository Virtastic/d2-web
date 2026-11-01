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
