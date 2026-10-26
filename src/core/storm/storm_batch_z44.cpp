// src/core/storm/storm_batch_z44.cpp -- UNMAPPED 1.13c Storm.dll rows 0x41652, 0x41C28..0x41CDA
// (alpha lane: marker only). Compiler-generated SEH unwind thunks: each runs a destructor on a
// stack object at a fixed offset from the unwound frame's EBP. ebp is passed explicitly; callees
// are seams on StormBatchZ44Env.
#include <cstdint>

struct StormBatchZ44Env {
    void (*sub_4910)(void* obj);   // 0x4910
    void (*sub_4B40)(void* obj);   // 0x4B40
    void (*sub_11A0)(void* obj);   // 0x11A0
    void (*sub_1250)(void* obj);   // 0x1250
    void (*sub_4C20)(void);        // 0x4C20
    void (*sub_4B80)(void* obj);   // 0x4B80
};

namespace {
template <typename T> T* P(uint32_t v) { return (T*)(uintptr_t)v; }
inline void* at(uint32_t ebp, int32_t off) { return P<void>(ebp + (uint32_t)off); }
}

// 1.13c STORM+0x41652: empty (returns).
extern "C" void Storm_Sub41652(void) {}

// 1.13c STORM+0x41C28
extern "C" void Storm_Sub41C28(uint32_t ebp, const StormBatchZ44Env* e) { e->sub_4910(at(ebp, -0x3a80)); }
// 1.13c STORM+0x41C3F
extern "C" void Storm_Sub41C3F(uint32_t ebp, const StormBatchZ44Env* e) { e->sub_4910(at(ebp, -0x3480)); }
// 1.13c STORM+0x41C4C
extern "C" void Storm_Sub41C4C(uint32_t ebp, const StormBatchZ44Env* e) { e->sub_4B40(at(ebp, -0x3480)); }
// 1.13c STORM+0x41C59
extern "C" void Storm_Sub41C59(uint32_t ebp, const StormBatchZ44Env* e) { e->sub_4910(at(ebp, -0x3480)); }
// 1.13c STORM+0x41C7F
extern "C" void Storm_Sub41C7F(uint32_t ebp, const StormBatchZ44Env* e) { e->sub_11A0(at(ebp, -300)); }
// 1.13c STORM+0x41C8C
extern "C" void Storm_Sub41C8C(uint32_t ebp, const StormBatchZ44Env* e) { e->sub_1250(at(ebp, -300)); }
// 1.13c STORM+0x41C99
extern "C" void Storm_Sub41C99(uint32_t ebp, const StormBatchZ44Env* e) { e->sub_11A0(at(ebp, -300)); }
// 1.13c STORM+0x41CA6
extern "C" void Storm_Sub41CA6(uint32_t ebp, const StormBatchZ44Env* e) { e->sub_11A0(at(ebp, -300)); }
// 1.13c STORM+0x41CC2
extern "C" void Storm_Sub41CC2(const StormBatchZ44Env* e) { e->sub_4C20(); }
// 1.13c STORM+0x41CCD: arg is the pointer stored at [ebp-0x210].
extern "C" void Storm_Sub41CCD(uint32_t ebp, const StormBatchZ44Env* e) { e->sub_4B80(*P<void*>(ebp - 0x210)); }
// 1.13c STORM+0x41CDA
extern "C" void Storm_Sub41CDA(uint32_t ebp, const StormBatchZ44Env* e) { e->sub_11A0(at(ebp, -0x230)); }
