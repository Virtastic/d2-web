#include <cstdint>
#include <cstring>
#include <emscripten.h>

// 1.13c D2Gfx+0x043A8 (43 B, leaf; callers 0x025E4 _msize, 0x02796 _realloc, 0x02BD4 _free):
// the MSVC 2003 Release CRT `___sbh_find_block(void* p)`. The small-block heap keeps a list of
// 0x14-byte region records; the list head is the dword global at VA 0x6FA9D8E8 (RVA 0x5D8E8) and
// the count is at VA 0x6FA9D8E4 (RVA 0x5D8E4). Each record carries its region's base as a dword at
// +0x0C. Walk from the head while `head < head + count*0x14`, and return the address of the first
// record whose base is within 0xFFFFF below `p` (an unsigned 32-bit difference, so a block below the
// base wraps high and never matches); return 0 when the walk ends. `p` is never dereferenced.
// Head and count are arguments here because no D2Gfx sbh state file owns them yet; the walk itself
// is a pure function of (list, count, block).
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2_gfx_sbh_find_block_43a8(uint32_t headerList, uint32_t headerCount,
                                            uint32_t block) {
    const uint32_t end = headerList + headerCount * 0x14u;
    for (uint32_t h = headerList; h < end; h += 0x14u) {
        uint32_t base;
        memcpy(&base, (const void*)(uintptr_t)(h + 0xC), sizeof base);
        if ((uint32_t)(block - base) < 0x100000u) return (void*)(uintptr_t)h;
    }
    return nullptr;
}
