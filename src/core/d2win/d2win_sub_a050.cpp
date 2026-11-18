#include <cstdint>
#include <emscripten.h>

extern "C" [[noreturn]] void d2_mcp_startup_overflow(uint32_t message);
extern "C" __attribute__((weak)) void Fog_Free10043(void* node, int32_t line, int32_t b);

// 1.13c D2Win+0x0A050
// Release a type-12 widget's child chain: p[0] must be 0xC (else fatal, Fog code 0x5D, _exit(-1)).
// Walks the list from the dword at +0x40 via each node's next at +0x34, one Fog #10043(99,0) per node.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2win_sub_A050(uint8_t* p) {
    if (*(int32_t*)p != 0xc) d2_mcp_startup_overflow(0x5d);
    for (uint8_t* n = (uint8_t*)(uintptr_t)*(uint32_t*)(p + 0x40); n; ) {
        uint8_t* cur = n;
        n = (uint8_t*)(uintptr_t)*(uint32_t*)(n + 0x34);
        if (Fog_Free10043) Fog_Free10043(cur, 99, 0); // retail frees the node just left
    }
}
