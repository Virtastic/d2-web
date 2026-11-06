// 1.13c Fog+0x1E320 (#10080): signal the event handle held at +0x114 of a queued-socket object.
// Retail: stdcall(p); null p returns, otherwise tail-jumps to SetEvent(*(HANDLE*)(p + 0x114)).
// The object keeps retail's 32-bit field layout; the kernel call is the injected seam.
#include <cstdint>
#include <cstring>

typedef uint32_t (*FogSetEventFn)(uint32_t handle);

extern "C" uint32_t fog_1e320(const uint8_t* p, FogSetEventFn setEvent) {  // 1.13c Fog+0x1E320
    if (!p) return 0;
    uint32_t h;
    memcpy(&h, p + 0x114, 4);
    return setEvent(h);
}
