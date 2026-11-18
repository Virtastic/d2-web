// 1.13c Storm+0x2350: allocates extra+0x18 bytes with tag
// AV_TSHashObjectChunk_URGN_VH, line -2 and flags|8 through SMemAlloc (0x1C580); on success zeroes the six header dwords;
// when location is non-zero it links the chunk through 0x2160 (linktype = location, no anchor) on either path.
// Stack args list, extra, flags; ESI = location. Returns the chunk or 0.
#include <cstdint>

struct StormChunkEnv {
    uint8_t* (*alloc)(uint32_t n, const char* tag, int line, uint32_t flags);
    void (*link)(void* list, void* node, uint32_t linktype, void* existing);
};

extern "C" void* Storm_Sub2350(StormChunkEnv& e, void* list, uint32_t extra, uint32_t flags, uint32_t location) {
    uint32_t* chunk = (uint32_t*)e.alloc(extra + 0x18, "AV_TSHashObjectChunk_URGN_VH", -2, flags | 8);
    if (chunk) {
        for (int i = 0; i < 6; i++) chunk[i] = 0;
    }
    if (location) e.link(list, chunk, location, 0);
    return chunk;
}
