// src/core/storm/storm_3e20.cpp -- Storm.dll archive-object destructor, one function per file.
// Self-contained on purpose: the copy that lived in storm_batch_b51.cpp pulled that TU's anonymous-namespace seams
// (Storm_Free, Storm_Sub4B80, ...), which have no definition, into every link that referenced the probe.
#include <cstdint>
#include <cstring>
#include <Windows.h>

extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line); // 0x1B960 SMemFree (storm_alloc_1b960.cpp)

namespace {
uint32_t& u32(uint32_t p, uintptr_t off) { return *(uint32_t*)(uintptr_t)(p + off); }
uint32_t rd32(const void* p) { uint32_t v; std::memcpy(&v, p, 4); return v; }
void wr32(void* p, uint32_t v) { std::memcpy(p, &v, 4); }
// 0x4B80 body (the two unlinks of storm_batch_1000.cpp, which are file-local there): unlink a doubly-linked node.
void UnlinkNode(uint8_t* n) {
    uint32_t link = rd32(n);
    if (link == 0) return;
    uint32_t tag = rd32(n + 4);
    uint8_t* prev;
    if ((int32_t)tag < 0) prev = (uint8_t*)(uintptr_t)(uint32_t)~tag;
    else prev = n + (uint32_t)(tag - rd32((void*)((uint8_t*)(uintptr_t)link + 4)));
    wr32(prev, link);
    wr32((void*)((uint8_t*)(uintptr_t)link + 4), tag);
    wr32(n, 0);
    wr32(n + 4, 0);
}
const char* kSfile = "..\\3rdParty\\STORM\\SOURCE\\SFILE.CPP";
}  // namespace

// 1.13c Storm+0x3E20
void Storm_Sub3E20(uint32_t p) {
    if (u32(p, 0x10c) != 0xffffffff) CloseHandle((HANDLE)(uintptr_t)u32(p, 0x10c));
    if (u32(p, 0x124)) Storm_Sub1B960((void*)(uintptr_t)u32(p, 0x124), kSfile, 0xd6);
    if (u32(p, 0x130)) Storm_Sub1B960((void*)(uintptr_t)u32(p, 0x130), kSfile, 0xd7);
    if (u32(p, 0x134)) Storm_Sub1B960((void*)(uintptr_t)u32(p, 0x134), kSfile, 0xde);
    if (u32(p, 0x138)) Storm_Sub1B960((void*)(uintptr_t)u32(p, 0x138), kSfile, 0xdf);
    UnlinkNode((uint8_t*)(uintptr_t)p); UnlinkNode((uint8_t*)(uintptr_t)p);   // 0x4B80
}
