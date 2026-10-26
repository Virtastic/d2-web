// src/core/fog/fog_batch_z18.cpp -- twelve Fog.dll SEH unwind thunks (+0x24A00..+0x24AE0), from cached decompiles.
// Each reads a saved object pointer at [EBP+4] (the unwound frame) and calls a Fog cleanup callee
// (+0x13B0, +0x16B0, +0x1670, +0x1030, +0x1630, +0x1400), which are host seams here.
// Guessed: the frame pointer is passed in as `ebp` (uint32_t link); +0x1030 takes no argument.
#include <cstdint>

namespace {
void (*fog_1030_z18)() = nullptr;
void (*fog_13B0_z18)(uint32_t) = nullptr;
void (*fog_1400_z18)(uint32_t) = nullptr;
void (*fog_1630_z18)(uint32_t) = nullptr;
void (*fog_1670_z18)(uint32_t) = nullptr;
void (*fog_16B0_z18)(uint32_t) = nullptr;
}

extern "C" {
// 1.13c Fog+0x24A00
void Unwind_24A00_z18(const uint32_t* ebp) { if (fog_13B0_z18) fog_13B0_z18(ebp[1]); }
// 1.13c Fog+0x24A4D
void Unwind_24A4D_z18(const uint32_t* ebp) { if (fog_16B0_z18) fog_16B0_z18(ebp[1]); }
// 1.13c Fog+0x24A61
void Unwind_24A61_z18(const uint32_t* ebp) { if (fog_1670_z18) fog_1670_z18(ebp[1]); }
// 1.13c Fog+0x24A6B
void Unwind_24A6B_z18() { if (fog_1030_z18) fog_1030_z18(); }
// 1.13c Fog+0x24A76
void Unwind_24A76_z18(const uint32_t* ebp) { if (fog_1630_z18) fog_1630_z18(ebp[1]); }
// 1.13c Fog+0x24A80
void Unwind_24A80_z18(const uint32_t* ebp) { if (fog_1400_z18) fog_1400_z18(ebp[1]); }
// 1.13c Fog+0x24A9C
void Unwind_24A9C_z18(const uint32_t* ebp) { if (fog_13B0_z18) fog_13B0_z18(ebp[1]); }
// 1.13c Fog+0x24AA6
void Unwind_24AA6_z18(const uint32_t* ebp) { if (fog_1400_z18) fog_1400_z18(ebp[1]); }
// 1.13c Fog+0x24AB0
void Unwind_24AB0_z18(const uint32_t* ebp) { if (fog_1630_z18) fog_1630_z18(ebp[1]); }
// 1.13c Fog+0x24ABA
void Unwind_24ABA_z18(const uint32_t* ebp) { if (fog_1670_z18) fog_1670_z18(ebp[1]); }
// 1.13c Fog+0x24AC4
void Unwind_24AC4_z18() { if (fog_1030_z18) fog_1030_z18(); }
// 1.13c Fog+0x24AE0
void Unwind_24AE0_z18(const uint32_t* ebp) { if (fog_1400_z18) fog_1400_z18(ebp[1]); }
}
