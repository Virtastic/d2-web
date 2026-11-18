// 1.13c D2Client+0x7AEA0 (no arguments): walk the list headed at D2Client+0x113AB0. A node is six dwords at +0..+0x14, a dword at +0x1A and the next
// pointer at +0x22 (all unaligned). Each node calls vtable slot 0x30 of the object at D2Client+0x113A74 (this = object, stdcall ten words:
// n[0], n[4], n[8], n[0xC], n[0x10], n[0x14], 0, n[0x1A], 0, 0xD); the object global is read again every node.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2c7AEA0Env { void (*slot30)(uint32_t thisPtr, const uint32_t a[10]); };

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_port_7aea0(const uint32_t* g_113ab0, const uint32_t* g_113a74, const D2c7AEA0Env* env) {
    uint32_t node = *g_113ab0;
    while (node) {
        const uint8_t* n = (const uint8_t*)(uintptr_t)node;
        uint32_t a[10];
        for (int i = 0; i < 6; i++) std::memcpy(&a[i], n + 4 * i, 4);
        a[6] = 0;
        std::memcpy(&a[7], n + 0x1a, 4);
        a[8] = 0;
        a[9] = 0xd;
        env->slot30(*g_113a74, a);
        std::memcpy(&node, n + 0x22, 4);
    }
}
