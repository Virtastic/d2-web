// src/core/d2game/crt_validate_eh3rn_7e6e.cpp -- 1.13c D2Game.dll implementation from
// build-wasm/ghidra-d2game/decomp/0x7E6E.c (alpha lane; no spec/vector yet). Retail base 0x6FC20000.
//
// Reuses the in-tree D2Gfx implementation after comparing all branches with the cached
// D2Game 0x7E6E body: identical CRT logic; only module-local cache addresses differ.
// The MSVC 2003 (VC7.1) Release CRT `__ValidateEH3RN`: validate an EH3 registration node before
// `_except_handler3` walks its scopetable. Prior art: the D2Client and D2MCPClient rows at the same
// 553-byte function implement the same recovered algorithm; reference/ghidra-reconstruct's CRT
// mapping names it `_ValidateEH3RN` with the recovered prototype `uint32_t __ValidateEH3RN(void*)`.
// The TEB stack bounds (Ghidra's `Self`, NT_TIB.StackBase/+4 and StackLimit/+8), the VirtualQuery
// probe and the InterlockedExchange spinlock are host seams; the 16-entry validated-page cache and
// its lock are the module globals at +0x110088/+0x110090/+0x1100D0.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#include "crt_validate_eh3rn_7e6e.h"

// Shift cache[0..index] right one slot and put `page` at slot 0; the tail falls off a full table.
static void eh3rn_move_front(D2GameEh3rnCache& cache, int32_t index, uintptr_t page) {
    uintptr_t carry = page;
    for (int32_t i = 0; i <= index; ++i) {
        uintptr_t old = cache.pages[i];
        cache.pages[i] = carry;
        carry = old;
    }
}

// 1.13c D2Game+0x07E6E (553 B, no callers recorded): the VC7.1 `__ValidateEH3RN`. Returns 1 when the node is
// accepted, 0 when it is corrupt and 0xFFFFFFFF when the page cannot be validated at all. The scopetable
// pointer (retail +0x08) must be 4-byte aligned, outside the thread stack [StackLimit, StackBase), and (when
// a handler in levels 0..trylevel has a nonzero filter) the saved value at node-8 must lie in
// [StackLimit, node). The pointer's page is cached (up to 16 entries, slot 0 is the fast hit); a miss is
// probed with VirtualQuery and must be MEM_IMAGE. A writable image page must carry MZ/PE/PE32 headers,
// have sections, and not fall inside a first section marked IMAGE_SCN_MEM_WRITE.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2game_validate_eh3rn_7e6e(const D2GameEh3Registration* registration, const D2GameEh3rnEnv* env) {
    auto& cache = *env->cache;
    const D2GameEh3Scope* scope = registration->scope;
    if (((uintptr_t)scope & 3u) != 0) return 0;
    const uintptr_t stack_limit = env->stack_limit;                            // [Self+8]
    if (stack_limit <= (uintptr_t)scope && (uintptr_t)scope < env->stack_base) return 0;

    const uint32_t trylevel = registration->try_level; // retail +0x0C
    if (trylevel == 0xffffffffu) return 1;

    uint32_t handlers = 0;
    for (uint32_t level = 0; level <= trylevel; ++level) {
        const uint32_t enclosing = scope[level].enclosing_level;
        if (enclosing != 0xffffffffu && level <= enclosing) return 0;
        if (scope[level].filter != 0) ++handlers;
    }
    if (handlers != 0) {
        const uintptr_t frame = (uintptr_t)registration->saved_frame; // retail node-8
        if (frame < stack_limit || (uintptr_t)registration <= frame) return 0;
    }

    const uintptr_t page = (uintptr_t)scope & ~uintptr_t{0xfff};

    for (int32_t slot = 0; slot < cache.count; ++slot) {
        if (cache.pages[slot] != page) continue;
        if (slot < 1) return 1;                                                // slot 0: no lock needed
        if (env->interlocked_exchange(&cache.lock, 1) != 0) return 1;          // busy: assume valid
        if (cache.pages[slot] == page) {
            eh3rn_move_front(cache, slot, page);
        } else {                                                               // moved under the lock
            int32_t found = cache.count - 1;
            while (found >= 0 && cache.pages[found] != page) --found;
            if (found < 0) {
                if (cache.count < 16) ++cache.count;
                found = cache.count - 1;
            } else if (found == 0) {
                env->interlocked_exchange(&cache.lock, 0);
                return 1;
            }
            eh3rn_move_front(cache, found, page);
        }
        env->interlocked_exchange(&cache.lock, 0);
        return 1;
    }

    D2GameMemoryBasicInformation mbi = {};
    if (env->virtual_query(scope, &mbi) == 0) return 0xffffffffu;
    if (mbi.type != 0x1000000u) return 0xffffffffu;                            // MEM_IMAGE only
    if (((uint8_t)mbi.protect & 0xcc) != 0) {                                  // writable: check the PE
        const uint8_t* image = (const uint8_t*)mbi.allocation_base;
        if (*(const uint16_t*)image != 0x5a4d) return 0xffffffffu;             // 'MZ'
        const uint8_t* nt = image + *(const int32_t*)(image + 0x3c);           // e_lfanew
        if (*(const uint32_t*)nt != 0x4550) return 0xffffffffu;                // 'PE\0\0'
        if (*(const uint16_t*)(nt + 0x18) != 0x10b) return 0xffffffffu;        // PE32 optional header
        const uint16_t opt_size = *(const uint16_t*)(nt + 0x14);               // SizeOfOptionalHeader
        if (*(const uint16_t*)(nt + 6) == 0) return 0xffffffffu;               // NumberOfSections
        const uint8_t* section0 = nt + 0x18 + opt_size;
        const uint32_t virtual_address = *(const uint32_t*)(section0 + 0x0c);
        // PE fields remain serialized 32-bit values; the native image-relative
        // distance must not alias a different mapping above 4 GB.
        const uintptr_t rva = (uintptr_t)scope - (uintptr_t)image;
        if (virtual_address <= rva &&
            rva < virtual_address + *(const uint32_t*)(section0 + 0x08) &&     // VirtualSize
            (*(const uint8_t*)(section0 + 0x27) & 0x80) != 0) {                // MEM_WRITE
            return 0;
        }
    }

    if (env->interlocked_exchange(&cache.lock, 1) != 0) return 1;              // busy: assume valid
    int32_t remaining = cache.count;
    int32_t slot = cache.count - 1;
    while (slot >= 0 && cache.pages[slot] != page) { --slot; --remaining; }
    if (remaining == 0) {                                                      // insert at the front
        const int32_t last = cache.count < 16 ? cache.count : 15;
        eh3rn_move_front(cache, last, page);
        if (cache.count < 16) ++cache.count;
    }
    env->interlocked_exchange(&cache.lock, 0);
    return 1;
}
