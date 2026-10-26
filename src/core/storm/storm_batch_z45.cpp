// src/core/storm/storm_batch_z45.cpp -- twelve 1.13c Storm.dll SEH unwind funclets (Unwind_6fc31xxx)
// transcribed from the cached decompiles. Stage A batch lane: implementation + marker only.
// unaff_EBP is an explicit parameter (frame bytes); callee rows are seams. Retail base 0x6FBF0000.
// Partial: callee signatures are guessed from the single call site (unmapped callees, void return).
#include <cstdint>
#include <cstring>

namespace {
uint32_t rd32(const uint8_t* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
}  // namespace

extern "C" void Storm_Z45_Callee1250(uint8_t* obj);
extern "C" void Storm_Z45_Callee11A0(uint8_t* obj);
extern "C" void Storm_Sub1050(void* p);
extern "C" void Storm_Z45_Callee22D0(uint32_t arg);
extern "C" void Storm_Z45_Callee4C20(void);
extern "C" void Storm_Z45_Callee4520(uint32_t arg);
extern "C" void Storm_Z45_Callee4910(uint32_t arg);
extern "C" void Storm_Z45_Callee3770(uint32_t arg);

// 1.13c STORM+0x41CE7
extern "C" void Storm_Unwind_41CE7(uint8_t* ebp) {
    Storm_Z45_Callee1250(ebp + (-0x230));
}

// 1.13c STORM+0x41CF4
extern "C" void Storm_Unwind_41CF4(uint8_t* ebp) {
    Storm_Z45_Callee11A0(ebp + (-0x230));
}

// 1.13c STORM+0x41D01
extern "C" void Storm_Unwind_41D01(uint8_t* ebp) {
    Storm_Z45_Callee11A0(ebp + (-0x230));
}

// 1.13c STORM+0x41D0E
extern "C" void Storm_Unwind_41D0E(uint8_t* ebp) {
    Storm_Z45_Callee11A0(ebp + (-0x230));
}

// 1.13c STORM+0x41D25
extern "C" void Storm_Unwind_41D25(uint8_t* ebp) {
    Storm_Sub1050((void*)(uintptr_t)rd32(ebp - 0x40));
}

// 1.13c STORM+0x41D40
extern "C" void Storm_Unwind_41D40(uint8_t* ebp) {
    Storm_Z45_Callee22D0(rd32(ebp + (4)));
}

// 1.13c STORM+0x41D60
extern "C" void Storm_Unwind_41D60(uint8_t* ebp) {
    (void)ebp;
    Storm_Z45_Callee4C20();
}

// 1.13c STORM+0x41D6B
extern "C" void Storm_Unwind_41D6B(uint8_t* ebp) {
    Storm_Z45_Callee4520(rd32(ebp + 4) + 0xc);
}

// 1.13c STORM+0x41D90
extern "C" void Storm_Unwind_41D90(uint8_t* ebp) {
    Storm_Z45_Callee4910(rd32(ebp + (4)));
}

// 1.13c STORM+0x41DF8
extern "C" void Storm_Unwind_41DF8(uint8_t* ebp) {
    Storm_Z45_Callee3770(rd32(ebp + (-16)));
}

// 1.13c STORM+0x41E15
extern "C" void Storm_Unwind_41E15(uint8_t* ebp) {
    (void)ebp;
    Storm_Z45_Callee4C20();
}

// 1.13c STORM+0x41E20
extern "C" void Storm_Unwind_41E20(uint8_t* ebp) {
    Storm_Z45_Callee22D0(rd32(ebp + (-16)));
}

