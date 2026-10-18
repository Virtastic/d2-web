// src/core/storm/storm_batch_alpha.cpp -- five 1.13c Storm.dll leaf/thunk rows transcribed from the
// cached decompiles (build-wasm/ghidra-storm/decomp/0x<RVA>.c): __break, the two CRT unlock thunks,
// the 0x8DA8 forwarder and the unwind leaf. Stage A batch lane (OBJECTIVE v8.3): implementation +
// marker only, no spec, no vector.
//
// Callees whose rows are not mapped yet are seams declared below in retail argument order.
// Register-only arguments (Ghidra unaff_EBX in the unlock thunks, in_EAX in the unwind leaf) are
// explicit parameters. 0x4C20 is already ours (src/core/fn/storm_leaves_b23.cpp).
#include <cstdint>

void Storm_UnlockFHandle(int32_t fileHandle);   // 0xE22A __unlock_fhandle(FileHandle)
void Storm_Sub8DA8(void);                       // 0x8DA8, forwards to 0x902A(8)
void Storm_Sub4C20(uint8_t* node);              // 0x4C20 list unlink, mapped

// 1.13c Storm+0x2F1F0
extern "C"
void Storm_Break_2F1F0(void) { __builtin_trap(); }

// 1.13c Storm+0x5931
extern "C"
void Storm_Sub5931(void) { Storm_Sub8DA8(); }

// 1.13c Storm+0x0C642
extern "C"
void Storm_SubC642(int32_t fileHandle) { Storm_UnlockFHandle(fileHandle); }

// 1.13c Storm+0x0C8A9
extern "C"
void Storm_SubC8A9(int32_t fileHandle) { Storm_UnlockFHandle(fileHandle); }

// 1.13c Storm+0x41970
extern "C"
void Storm_Unwind_41970(uint8_t* node) { Storm_Sub4C20(node); }
