#include <cstdint>
#include <cstdlib>

// 1.13c D2Common+0x052CA _calloc(count, size): refuse count*size above 0xFFFFFFE0, else zeroed
// block (size 0 allocates 1 byte). Retail's small-block heap / _callnewh retry loop is the host
// allocator here (not observable); returns null on refusal.
extern "C" void* d2common_calloc_52ca(uint32_t count, uint32_t size)
{
    if (count != 0 && size > 0xffffffe0u / count) return nullptr;
    uint32_t bytes = count * size;
    return std::calloc(bytes ? bytes : 1, 1);
}

// 1.13c D2Common+0x044BC _errno(): address of the calling thread's _terrno (via __getptd, +0x3475).
// Env seam: the thread-data lookup is a host hook.
extern "C" int32_t* d2common_errno_44bc(int32_t* (*getptdTerrno)()) { return getptdTerrno(); }
