// src/core/storm/storm_batch_tiny_1018.cpp -- five 1.13c Storm.dll tiny rows (0x1E90, 0x1F10,
// 0x3A30, 0x3AC0, 0x44B0) transcribed from the cached decompiles
// (build-wasm/ghidra-storm/decomp/0x<RVA>.c). Stage A alpha lane (OBJECTIVE v8.3): marker only,
// no spec, no vector.
//
// Prior art used: the decompiles, the 1.13c disassembly for the register arguments and the tag
// strings, and the sibling rows already recovered in src/core/gfx/storm_batch_1520.cpp (the same
// this+8 SMemFree release) and src/core/storm/storm_batch_alpha.cpp (a register-only argument --
// in_EAX here, in_ECX in 0x44B0 -- becomes an explicit parameter).
//
// 0x1E90/0x1F10/0x3A30/0x3AC0 are one three-instruction destructor body: release the buffer at
// this+8 through 0x1B960 SMemFree with line -2. They differ only in the static tag, which is the
// literal string at the referenced address (0x51C24, 0x51C10, 0x51C80, 0x51C74); the mangled
// ".?AU...@@" tags are the retail bytes, not Ghidra's sanitised symbol name.
// 0x44B0 is a __thiscall wrapper: reset the object through 0x30B70 and return the object in EAX.
//
// Callees whose rows are not mapped yet stay seams, declared in retail argument order.
#include <cstdint>

extern "C" int32_t Storm_Sub1B960(void* p, const char* file, int32_t line);  // 0x1B960 SMemFree
uint32_t Storm_Sub30B70(uint32_t* self, uint32_t param_2);        // 0x30B70 (UNMAPPED)

// 1.13c Storm+0x1E90
extern "C"
void Storm_Sub1E90(uint32_t* self) {
    if (self[2] != 0) Storm_Sub1B960((void*)(uintptr_t)self[2], ".?AU_SOURCE@@", -2);
}

// 1.13c Storm+0x1F10
extern "C"
void Storm_Sub1F10(uint32_t* self) {
    if (self[2] != 0) Storm_Sub1B960((void*)(uintptr_t)self[2], ".?AURECTF@@", -2);
}

// 1.13c Storm+0x3A30
extern "C"
void Storm_Sub3A30(uint32_t* self) {
    if (self[2] != 0) Storm_Sub1B960((void*)(uintptr_t)self[2], ".I", -2);
}

// 1.13c Storm+0x3AC0
extern "C"
void Storm_Sub3AC0(uint32_t* self) {
    if (self[2] != 0) Storm_Sub1B960((void*)(uintptr_t)self[2], ".E", -2);
}

// 1.13c Storm+0x44B0
extern "C"
uint32_t* Storm_Sub44B0(uint32_t* self) {
    Storm_Sub30B70(self, 0);
    return self;
}
