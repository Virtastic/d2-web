#include <cstddef>
#include <cstdint>
#include <emscripten.h>

// Prior art: MSVC CRT __ioterm and the matching D2Client implementation at 0x862C.
// D2MOO has no implementation of this CRT routine.
struct D2GameIoInfo62ed {
    int32_t handle;
    uint8_t flags;
    uint8_t token;
    uint16_t padding;
    int32_t lock_initialized;
    uint8_t critical_section[0x18];
};
static_assert(sizeof(D2GameIoInfo62ed) == 0x24);
static_assert(offsetof(D2GameIoInfo62ed, lock_initialized) == 8);
static_assert(offsetof(D2GameIoInfo62ed, critical_section) == 0xc);

// blocks is retail's 64-pointer _pioinfo table at 0x6FD3A220. Callbacks model
// DeleteCriticalSection and _free; the table retains native wasm64 pointers.
// 1.13c D2Game+0x062ED (76 B, caller 0x35BB): checked against the decompile; 64 slots, 0x480-byte
// blocks of 0x24-byte entries; lock deleted only when entry+8 != 0; slot cleared after free.
// Test: test/d2game-ioterm-62ed.test.mjs
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_ioterm_62ed(
    D2GameIoInfo62ed** blocks,
    void (*delete_critical_section)(void*), void (*free_block)(void*)) {
    for (uint32_t slot = 0; slot < 64; ++slot) {
        D2GameIoInfo62ed* entry = blocks[slot];
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
