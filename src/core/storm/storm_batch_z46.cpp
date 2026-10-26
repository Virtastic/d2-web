// src/core/storm/storm_batch_z46.cpp -- twelve 1.13c Storm.dll SEH unwind funclets (cached decompiles
// build-wasm/ghidra-storm/decomp/0x<RVA>.c). Stage A batch: implementation + marker only.
// Each funclet reads a frame slot off the unwound function's EBP; here the slot's value is the
// explicit parameter `obj` (32-bit link). Callees are seams in retail argument order.
#include <cstdint>

void Storm_Sub4C20(uint8_t* node);                       // 0x4C20 list unlink, mapped
void Storm_Sub22D0(uint32_t obj);                        // 0x22D0
void Storm_Sub1CF0(uint32_t obj);                        // 0x1CF0
void Storm_Sub1DA0(uint32_t obj);                        // 0x1DA0
void Storm_Sub1DF0(uint32_t obj);                        // 0x1DF0
void Storm_Sub1E40(uint32_t obj);                        // 0x1E40
void Storm_Sub3770(uint32_t obj);                        // 0x3770
void Storm_Sub3820(void* p);                             // 0x3820 element destructor
void Storm_EhVectorDestructorIterator(void* arr, uint32_t size, uint32_t count, void (*dtor)(void*)); // 0x605E

// 1.13c Storm+0x41E40
extern "C" void Storm_Unwind_41E40(uint8_t* node) { Storm_Sub4C20(node); }  // node is a register/global in retail (no args in decompile)
// 1.13c Storm+0x41E4B
extern "C" void Storm_Unwind_41E4B(uint32_t obj) { Storm_Sub22D0(obj); }
// 1.13c Storm+0x41E60
extern "C" void Storm_Unwind_41E60(uint32_t obj) { Storm_Sub1CF0(obj); }
// 1.13c Storm+0x41E6A
extern "C" void Storm_Unwind_41E6A(uint32_t obj) { Storm_Sub1DA0(obj + 0x18); }
// 1.13c Storm+0x41E77
extern "C" void Storm_Unwind_41E77(uint32_t obj) { Storm_Sub1DF0(obj + 0x28); }
// 1.13c Storm+0x41E84
extern "C" void Storm_Unwind_41E84(uint32_t obj) { Storm_Sub1E40(obj + 0x38); }
// 1.13c Storm+0x41EE0 (slot is EBP-0x10)
extern "C" void Storm_Unwind_41EE0(uint32_t obj) { Storm_Sub3770(obj); }
// 1.13c Storm+0x41F00 (0x10 elements of 0x14 bytes)
extern "C" void Storm_Unwind_41F00(void* arr) { Storm_EhVectorDestructorIterator(arr, 0x14, 0x10, Storm_Sub3820); }
// 1.13c Storm+0x41F20
extern "C" void Storm_Unwind_41F20(uint8_t* node) { Storm_Sub4C20(node); }
// 1.13c Storm+0x41F2B
extern "C" void Storm_Unwind_41F2B(uint8_t* node) { Storm_Sub4C20(node); }
// 1.13c Storm+0x41F36
extern "C" void Storm_Unwind_41F36(uint32_t obj) { Storm_Sub1CF0(obj); }
// 1.13c Storm+0x41F48
extern "C" void Storm_Unwind_41F48(uint32_t obj) { Storm_Sub1DA0(obj + 0x18); }
