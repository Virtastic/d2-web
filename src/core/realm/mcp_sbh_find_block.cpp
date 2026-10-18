#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2MCPClient+0x3544 (size 43, leaf; callers 0x1BB4 _free, 0x29EA _realloc, 0x2B9C __msize):
// the MSVC 2003 Release CRT `___sbh_find_block(void* p)`. The small-block heap keeps a list of
// 0x14-byte region headers at the dword global VA 0x6FA32254 (RVA 0x12254) whose length is the dword
// VA 0x6FA32250 (RVA 0x12250). Each header carries the region base as a dword at +0x0C. Walk the list
// from its head to `head + count*0x14` inclusive-as-limit; return the address of the first header
// whose base is at most 0xFFFFF below `p` (an unsigned 32-bit difference, so a block below the base
// wraps high and never matches), or 0 when none does. The block pointer is never dereferenced here.
// The head and count are passed in (the retail globals) so the walk is a pure function of them.
extern "C" {

EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_sbh_find_block(uint32_t headerList, uint32_t headerCount,
                                                    uint32_t block) {
    const uint32_t end = headerList + headerCount * 0x14u;
    for (uint32_t h = headerList; h < end; h += 0x14u) {
        uint32_t base;
        memcpy(&base, (const void*)(uintptr_t)(h + 0xC), sizeof base);
        if ((uint32_t)(block - base) < 0x100000u) return h;
    }
    return 0;
}

}  // extern "C"
