// src/core/d2client/crt_validate_eh3rn_9d2d.cpp -- 1.13c D2Client.dll Stage A implementation from
// build-wasm/ghidra-d2client/decomp/0x9D2D.c (alpha lane; no spec/vector yet). Retail base 0x6FAB0000.
//
// The MSVC 2003 (VC7.1) Release CRT `__ValidateEH3RN`: validate an EH3 registration node before
// `_except_handler3` walks its scopetable. Prior art: the existing D2MCPClient VC7.1 validator
// implements the same recovered algorithm; reference/ghidra-reconstruct's CRT mapping names it `_ValidateEH3RN` with the recovered prototype `uint32_t __ValidateEH3RN(void*)`; the body below is
// the decompile's algorithm. The TEB stack bounds (Ghidra's `Self`, NT_TIB.StackBase/+4 and
// StackLimit/+8), the VirtualQuery probe and the InterlockedExchange spinlock are host seams; the
// 16-entry validated-page cache and its lock are the module globals at +0xFAB90/+0xFAB98/+0xFABD8.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

// MEMORY_BASIC_INFORMATION as VirtualQuery fills it on x86 (0x1C bytes); only these fields are read.
struct D2ClientMemoryBasicInformation {
    void*     base;               // +0x00
    void*     allocation_base;    // +0x04
    uint32_t  allocation_protect; // +0x08
    uintptr_t region_size;        // +0x0C
    uint32_t  state;              // +0x10
    uint32_t  protect;            // +0x14
    uint32_t  type;               // +0x18
};

struct D2ClientEh3rnEnv {
    uintptr_t stack_limit;        // [Self+8] NT_TIB.StackLimit
    uintptr_t stack_base;         // [Self+4] NT_TIB.StackBase
    uintptr_t (*virtual_query)(const void* addr, D2ClientMemoryBasicInformation* mbi); // 0 on failure
    int32_t   (*interlocked_exchange)(int32_t* target, int32_t value);              // returns old
};

static int32_t  g_eh3_page_count;      // +0xFAB90
static uint32_t g_eh3_page[16];        // +0xFAB98
static int32_t  g_eh3_lock;            // +0xFABD8

// Shift cache[0..index] right one slot and put `page` at slot 0; the tail falls off a full table.
static void eh3rn_move_front(int32_t index, uint32_t page) {
    uint32_t carry = page;
    for (int32_t i = 0; i <= index; ++i) {
        uint32_t old = g_eh3_page[i];
        g_eh3_page[i] = carry;
        carry = old;
    }
}

// 1.13c D2Client+0x09D2D (553 B, caller 0x0498C): the VC7.1 `__ValidateEH3RN`. Returns 1 when the node is
// accepted, 0 when it is corrupt and 0xFFFFFFFF when the page cannot be validated at all. The scopetable
// pointer at +0x08 must be 4-byte aligned, outside the thread stack [StackLimit, StackBase), and (when
// no handler in levels 0..trylevel has a nonzero filter) the saved value at node-8 must lie in
// [StackLimit, node). The pointer's page is cached (up to 16 entries, slot 0 is the fast hit); a miss is
// probed with VirtualQuery and must be MEM_IMAGE. A writable image page must carry MZ/PE/PE32 headers,
// have sections, and not fall inside a first section marked IMAGE_SCN_MEM_WRITE.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_client_validate_eh3rn_9d2d(void* registration, const D2ClientEh3rnEnv* env) {
    const uint32_t* node = (const uint32_t*)registration;
    const uint32_t* scope = (const uint32_t*)(uintptr_t)node[2];              // +0x08
    if (((uintptr_t)scope & 3u) != 0) return 0;
    const uintptr_t stack_limit = env->stack_limit;                            // [Self+8]
    if (stack_limit <= (uintptr_t)scope && (uintptr_t)scope < env->stack_base) return 0;

    const uint32_t trylevel = node[3];                                         // +0x0C
    if (trylevel == 0xffffffffu) return 1;

    uint32_t handlers = 0;
    for (uint32_t level = 0; level <= trylevel; ++level) {
        const uint32_t enclosing = scope[level * 3 + 0];
        if (enclosing != 0xffffffffu && level <= enclosing) return 0;
        if (scope[level * 3 + 1] != 0) ++handlers;
    }
    if (handlers != 0) {
        const uintptr_t frame = node[-2];                                      // *(uint*)(node-8)
        if (frame < stack_limit || (uintptr_t)registration <= frame) return 0;
    }

    const uint32_t page = (uint32_t)((uintptr_t)scope & 0xfffff000u);

    for (int32_t slot = 0; slot < g_eh3_page_count; ++slot) {
        if (g_eh3_page[slot] != page) continue;
        if (slot < 1) return 1;                                                // slot 0: no lock needed
        if (env->interlocked_exchange(&g_eh3_lock, 1) != 0) return 1;          // busy: assume valid
        if (g_eh3_page[slot] == page) {
            eh3rn_move_front(slot, page);
        } else {                                                               // moved under the lock
            int32_t found = g_eh3_page_count - 1;
            while (found >= 0 && g_eh3_page[found] != page) --found;
            if (found < 0) {
                if (g_eh3_page_count < 16) ++g_eh3_page_count;
                found = g_eh3_page_count - 1;
            } else if (found == 0) {
                env->interlocked_exchange(&g_eh3_lock, 0);
                return 1;
            }
            eh3rn_move_front(found, page);
        }
        env->interlocked_exchange(&g_eh3_lock, 0);
        return 1;
    }

    D2ClientMemoryBasicInformation mbi = {};
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
        const uint32_t rva = (uint32_t)((uintptr_t)scope - (uintptr_t)image);
        if (virtual_address <= rva &&
            rva < virtual_address + *(const uint32_t*)(section0 + 0x08) &&     // VirtualSize
            (*(const uint8_t*)(section0 + 0x27) & 0x80) != 0) {                // MEM_WRITE
            return 0;
        }
    }

    if (env->interlocked_exchange(&g_eh3_lock, 1) != 0) return 1;              // busy: assume valid
    int32_t remaining = g_eh3_page_count;
    int32_t slot = g_eh3_page_count - 1;
    while (slot >= 0 && g_eh3_page[slot] != page) { --slot; --remaining; }
    if (remaining == 0) {                                                      // insert at the front
        const int32_t last = g_eh3_page_count < 16 ? g_eh3_page_count : 15;
        eh3rn_move_front(last, page);
        if (g_eh3_page_count < 16) ++g_eh3_page_count;
    }
    env->interlocked_exchange(&g_eh3_lock, 0);
    return 1;
}
