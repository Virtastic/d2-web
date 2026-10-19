#include <cstddef>
#include <cstdint>
#include <emscripten.h>

struct D2GfxSystemInfo {
    uint32_t page_size;
};

// Host-side VirtualQuery result for wasm64. Retail's x86 record is 0x1c bytes, but its pointers
// cannot hold native wasm64 addresses; this adapter uses the host-width Windows layout instead.
struct D2GfxMemoryBasicInformation {
    uintptr_t base;
    uintptr_t allocation_base;
    uint32_t allocation_protect;
    uint16_t partition_id;
    uint16_t reserved0;
    uintptr_t region_size;
    uint32_t state;
    uint32_t protect;
    uint32_t type;
    uint32_t reserved1;
};
static_assert(offsetof(D2GfxMemoryBasicInformation, region_size) == 0x18);
static_assert(offsetof(D2GfxMemoryBasicInformation, state) == 0x20);
static_assert(offsetof(D2GfxMemoryBasicInformation, protect) == 0x24);
static_assert(sizeof(D2GfxMemoryBasicInformation) == 0x30);

struct D2GfxResetStkEnv {
    uintptr_t (*virtual_query)(uintptr_t address, D2GfxMemoryBasicInformation* mbi, uintptr_t size);
    void (*get_system_info)(D2GfxSystemInfo* info);
    uintptr_t (*virtual_alloc)(uintptr_t address, uintptr_t size, uint32_t type, uint32_t protect);
    int32_t (*virtual_protect)(uintptr_t address, uintptr_t size, uint32_t protect, uint32_t* old_protect);
};

// 1.13c D2Gfx+0x0714E (227 B): __resetstkoflw. Retail gets the address of its local stack
// slot and its platform selector from the DLL, then uses these OS calls to restore the guard
// page. The seams make that contract explicit for the wasm port, which has no PE import table;
// their addresses and sizes use native width so wasm64 stack and region pointers are preserved.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2gfx_resetstkoflw(
    uintptr_t stack_slot, uint32_t platform, const D2GfxResetStkEnv* env) {
    if (!env || !env->virtual_query || !env->get_system_info || !env->virtual_protect) return 0;
    D2GfxMemoryBasicInformation mbi{};
    if (env->virtual_query(stack_slot, &mbi, sizeof(mbi)) == 0) return 0;

    D2GfxSystemInfo system_info{};
    env->get_system_info(&system_info);
    if (!system_info.page_size) return 0;
    const uintptr_t page = system_info.page_size;
    const uintptr_t guard = (~(page - 1u) & stack_slot) - page;
    const uintptr_t pages_above_allocation = platform != 1 ? 2u : 17u;
    const uintptr_t limit = pages_above_allocation * page + mbi.allocation_base;
    if (limit > guard) return 0;

    uintptr_t target = guard;
    if (platform != 1) {
        if (!env->virtual_alloc) return 0;
        target = mbi.allocation_base;
        for (;;) {
            if (env->virtual_query(target, &mbi, sizeof(mbi)) == 0) return 0;
            target += mbi.region_size;
            if ((mbi.state & 0x1000u) != 0) break;
        }
        target = mbi.base;
        if (((mbi.protect >> 8) & 1u) != 0) return 1;
        if (guard < mbi.base) return 0;
        if (mbi.base < limit) target = limit;
        env->virtual_alloc(target, page, 0x1000, 4);
    }

    uint32_t old_protect = 0;
    const uint32_t protect = (platform != 1 ? 0x103u : 0u) + 1u;
    return env->virtual_protect(target, page, protect, &old_protect);
}
