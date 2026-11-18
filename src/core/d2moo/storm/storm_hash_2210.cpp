// 1.13c Storm+0x2210: allocates extra+0x60 bytes with tag AURGN, line -2
// and flags|8 through SMemAlloc (0x1C580); on success runs the constructor 0x2410; when location is non-zero it links the
// node with 0x2160 (linktype = location, no anchor) on either path. Stack args list, extra, flags; ESI = location.
#include <cstdint>

extern "C" void* Storm_Port2410(void* e);

struct StormRgnEnv {
    uint8_t* (*alloc)(uint32_t n, const char* tag, int line, uint32_t flags);
    void (*link)(void* list, void* node, uint32_t linktype, void* existing);
};

extern "C" void* Storm_Sub2210(StormRgnEnv& e, void* list, uint32_t extra, uint32_t flags, uint32_t location) {
    uint8_t* node = e.alloc(extra + 0x60, "AURGN", -2, flags | 8);
    void* result = 0;
    if (node) result = Storm_Port2410(node);
    if (location) e.link(list, node, location, 0);
    return result;
}
