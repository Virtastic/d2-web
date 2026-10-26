// src/core/fog/fog_batch_z6.cpp -- three Fog.dll SEH unwind thunks (+0x24A45, +0x24A94, +0x24B0A), from cached decompiles.
// Guessed: callees +0x1390/+0x1750 take the saved object pointer at [EBP+4] (decompile shows no args); host seams here.
#include <cstdint>

namespace {
void (*fog_1390_z6)(uint32_t) = nullptr;
void (*fog_1750_z6)(uint32_t) = nullptr;
}

extern "C" {
// 1.13c Fog+0x24A45
void Unwind_24A45_z6(const uint32_t* ebp) { if (fog_1390_z6) fog_1390_z6(ebp[1]); }
// 1.13c Fog+0x24A94
void Unwind_24A94_z6(const uint32_t* ebp) { if (fog_1750_z6) fog_1750_z6(ebp[1]); }
// 1.13c Fog+0x24B0A
void Unwind_24B0A_z6(const uint32_t* ebp) { if (fog_1390_z6) fog_1390_z6(ebp[1]); }
}
