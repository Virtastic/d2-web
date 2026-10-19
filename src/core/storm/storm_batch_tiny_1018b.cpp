// src/core/storm/storm_batch_tiny_1018b.cpp -- five 1.13c Storm.dll tiny leaves transcribed from the
// cached decompiles (build-wasm/ghidra-storm/decomp/0x<RVA>.c): an AU_PLAYERNAME free, the CRT
// _atexit/_onexit wrapper, the CRT lock-0xE unlock thunk, type_info's scalar deleting destructor and
// the __ArrayUnwind SEH fragment. Stage A batch lane (OBJECTIVE v8.3): implementation + marker only,
// no spec, no vector. Retail base 0x6FBF0000.
//
// Callees whose rows are not mapped yet are seams declared below in retail argument order; register
// and frame arguments (Ghidra in_EAX in 0x5150, unaff_EBP in 0x60A6) are explicit parameters, as in
// src/core/storm/storm_batch_alpha.cpp. 0x1050 is already ours (src/core/gfx/storm_batch_1000.cpp:
// SMemFree(p, "delete", -1)); it is declared here as a seam because that body has internal linkage.
#include <cstdint>
#include <cstring>

int32_t Storm_Sub1B960(void* block, const char* source, int32_t line);   // 0x1B960 SMemFree
void* Storm_Onexit(void (*func)(void));                               // 0x58FF __onexit
void Storm_Sub902A(int32_t lock);                                     // 0x902A __unlock(lock)
void Storm_TypeInfoDtor(void* self);                                  // 0x595D ~type_info
void Storm_Sub1050(void* p);                                          // 0x1050 SMemFree(p, "delete", -1)
void Storm_ArrayUnwind(void* ptr, uint32_t size, int32_t count, void (*dtor)(void*));   // 0x6000

namespace {

uint32_t rd32(const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }

uint8_t* framePtr(uint8_t* ebp, int32_t off) {
    return ebp + off;
}
void* rdptr(uint8_t* ebp, int32_t off) { return (void*)(uintptr_t)rd32(ebp + off); }

}  // namespace

// 1.13c Storm+0x05150
// EAX is the record; when its +0x08 pointer is set, free it with the RTTI source name and line -2.
extern "C"
void Storm_Sub5150(uint8_t* eax) {
    void* block = rdptr(eax, 8);
    if (block != nullptr) Storm_Sub1B960(block, ".?AU_PLAYERNAME@@", -2);
}

// 1.13c Storm+0x05937
// _atexit(func): register through __onexit; its non-null return means success, reported as 0, and a
// null return as -1 ((p != 0) - 1).
extern "C"
int32_t Storm_Atexit(void (*func)(void)) {
    return (Storm_Onexit(func) != nullptr) - 1;
}

// 1.13c Storm+0x0599A
// __unlock(0xE): the type_info destructor's unlock half.
extern "C"
void Storm_Sub599A(void) { Storm_Sub902A(0xE); }

// 1.13c Storm+0x059A3
// type_info's scalar deleting destructor: run ~type_info(this), free the object when the flag byte is
// odd, return this.
extern "C"
void* Storm_Sub59A3(void* self, uint32_t flags) {
    Storm_TypeInfoDtor(self);
    if ((flags & 1) != 0) Storm_Sub1050(self);
    return self;
}

// 1.13c Storm+0x060A6
// Unwind handler fragment of __ArrayUnwind: the parent frame selector at EBP-0x1C being 0 takes the
// handler, which unwinds the parent's arguments at EBP+8/+0xC/+0x10/+0x14 (ptr, size, count, dtor).
extern "C"
void Storm_Sub60A6(uint8_t* ebp) {
    if ((int32_t)rd32(framePtr(ebp, -0x1C)) != 0) return;
    Storm_ArrayUnwind(rdptr(ebp, 8), rd32(framePtr(ebp, 0xC)),
                      (int32_t)rd32(framePtr(ebp, 0x10)),
                      (void (*)(void*))(uintptr_t)rd32(framePtr(ebp, 0x14)));
}
