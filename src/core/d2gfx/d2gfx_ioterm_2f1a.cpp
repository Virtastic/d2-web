#include <cstddef>
#include <cstdint>
#include <emscripten.h>

// Prior art: MSVC CRT __ioterm and the matching D2Client implementation at 0x862C.
// D2MOO has no implementation of this CRT routine.
struct D2GfxIoInfo2f1a {
    int32_t handle;
    uint8_t flags;
    uint8_t token;
    uint16_t padding;
    int32_t lock_initialized;
    uint8_t critical_section[0x18];
};
static_assert(sizeof(D2GfxIoInfo2f1a) == 0x24);
static_assert(offsetof(D2GfxIoInfo2f1a, lock_initialized) == 8);
static_assert(offsetof(D2GfxIoInfo2f1a, critical_section) == 0xc);

// blocks is retail's 64-pointer _pioinfo table at 0x6FA9E920. Callbacks model
// DeleteCriticalSection and _free; the table retains native wasm64 pointers.
// 1.13c D2Gfx+0x2F1A
extern "C" EMSCRIPTEN_KEEPALIVE void d2gfx_ioterm_2f1a(
    D2GfxIoInfo2f1a** blocks,
    void (*delete_critical_section)(void*), void (*free_block)(void*)) {
    for (uint32_t slot = 0; slot < 64; ++slot) {
        D2GfxIoInfo2f1a* entry = blocks[slot];
        if (entry == nullptr) continue;
        while (reinterpret_cast<uintptr_t>(entry) <
               reinterpret_cast<uintptr_t>(blocks[slot]) + 0x480) {
            if (entry->lock_initialized != 0)
                delete_critical_section(entry->critical_section);
            ++entry;
        }
        free_block(blocks[slot]);
        blocks[slot] = nullptr;
    }
}
