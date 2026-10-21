#include <cstdint>

// Prior art: none in D2MOO (linked MSVC 2003 CRT small-block heap).
// 1.13c D2Common+0x08064 (___sbh_alloc_new_region)
// Grow the region-descriptor array (HeapReAlloc, 0x10 more entries) when full, then fill the next
// 20-byte descriptor: [4]=HeapAlloc(zeroed,0x41c4) header, [3]=VirtualAlloc(1MiB reserve), [2]=-1,
// [0]=[1]=0; count++, header's first dword=-1. Failure frees the header and returns null.
// Heap globals and Win32 calls are seams; pointers are 32-bit retail values.
struct SbhHeap {
    uint32_t count;      // DAT_6fdf14ac
    uint32_t capacity;   // DAT_6fdf14bc
    uint8_t* regions;    // DAT_6fdf14b0
    void* heap;          // DAT_6fdf170c
};
extern "C" SbhHeap g_D2Common_sbh;
extern "C" void* D2Common_HeapReAlloc(void* heap, uint32_t flags, void* p, uint32_t n);
extern "C" void* D2Common_HeapAlloc(void* heap, uint32_t flags, uint32_t n);
extern "C" void D2Common_HeapFree(void* heap, uint32_t flags, void* p);
extern "C" void* D2Common_VirtualAlloc(void* addr, uint32_t n, uint32_t type, uint32_t prot);

extern "C" uint32_t* D2Common_8064_SbhAllocNewRegion()
{
    SbhHeap& h = g_D2Common_sbh;
    if (h.count == h.capacity) {
        void* p = D2Common_HeapReAlloc(h.heap, 0, h.regions, (h.capacity * 5 + 0x50) * 4);
        if (!p) return nullptr;
        h.capacity += 0x10;
        h.regions = (uint8_t*)p;
    }
    uint32_t* r = (uint32_t*)(h.regions + h.count * 0x14);
    void* hdr = D2Common_HeapAlloc(h.heap, 8, 0x41c4);
    r[4] = (uint32_t)(uintptr_t)hdr;
    if (!hdr) return nullptr;
    void* mem = D2Common_VirtualAlloc(nullptr, 0x100000, 0x2000, 4);
    r[3] = (uint32_t)(uintptr_t)mem;
    if (!mem) {
        D2Common_HeapFree(h.heap, 0, hdr);
        return nullptr;
    }
    r[2] = 0xffffffffu;
    r[0] = 0;
    r[1] = 0;
    h.count++;
    *(uint32_t*)hdr = 0xffffffffu;
    return r;
}
