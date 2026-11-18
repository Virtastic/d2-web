// Prior art used: D2MOO D2Gfx DllMain.cpp and 's D2Gfx
// entry-point map. These compiler/runtime leaves have no D2MOO game implementation;
// their contracts come from the cached 1.13c bodies. Platform calls are explicit
// seams, retaining retail argument order and widths without truncating wasm pointers.
#include <cstdint>

extern "C" void d2gfx_doexit(uint32_t code, uint32_t quick, uint32_t retcaller);

// 1.13c D2Gfx+0x0287E
extern "C" void d2gfx_sub_287e(void (*unlock)(uint32_t)) {
    unlock(4);
}

// 1.13c D2Gfx+0x02D0B
extern "C" void d2gfx_sub_2d0b(void (*unlock)(uint32_t)) {
    unlock(4);
}

// Storm ordinal 403 is the allocator's free entry. The retail source label is
// "delete", line is UINT32_MAX, flags are zero; NULL suppresses the call entirely.
// 1.13c D2Gfx+0x01000
extern "C" void d2gfx_sub_1000(void* allocation,
    void (*storm_free)(void*, const char*, uint32_t, uint32_t)) {
    if (allocation != nullptr) storm_free(allocation, "delete", 0xffffffffu, 0);
}

// Full CRT cleanup, returning to the caller rather than exiting the process.
// 1.13c D2Gfx+0x0152D
extern "C" void d2gfx_cexit() {
    d2gfx_doexit(0, 0, 1);
}

// The imported TlsAlloc result is left in EAX by this tail wrapper. Preserve it
// for __mtinit, including TLS_OUT_OF_INDEXES (0xffffffff) on allocation failure.
// 1.13c D2Gfx+0x028C5
extern "C" uint32_t d2gfx_sub_28c5(uint32_t (*tls_alloc)()) {
    return tls_alloc();
}
