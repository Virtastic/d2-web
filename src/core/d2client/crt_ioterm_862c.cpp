#include <cstddef>
#include <cstdint>
#include <emscripten.h>

// Prior art: RE-map row; D2MOO has no D2Client CRT body.
// Cross-check: src/core/realm/mcp_crt_batch_b56.cpp, d2_mcp_ioterm.
struct D2ClientIoInfo862c {
    int32_t handle;
    uint8_t flags;
    uint8_t token;
    uint16_t padding;
    int32_t lock_initialized;
    uint8_t critical_section[0x18];
};
static_assert(sizeof(D2ClientIoInfo862c) == 0x24);
static_assert(offsetof(D2ClientIoInfo862c, lock_initialized) == 8);
static_assert(offsetof(D2ClientIoInfo862c, critical_section) == 0xc);

// The 64 native pointers represent retail's 32-bit slots at RVA 0x1239A0..0x123A9C.
// Host callbacks model DeleteCriticalSection and _free (0x4891), respectively.
// The caller supplies the same table used by __ioinit, without narrowing wasm64 pointers.
// 1.13c D2Client+0x0862C
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_862c(
    D2ClientIoInfo862c** blocks,
    void (*delete_critical_section)(void*), void (*free_block)(void*)) {
    for (uint32_t slot = 0; slot < 0x40; ++slot) {
        D2ClientIoInfo862c* entry = blocks[slot];
        if (!entry) continue;
        // Retail reloads the slot after every callback before testing the next boundary.
        // Keep that observable ordering rather than caching the block for the whole walk.
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
