// 1.13c Storm+0x37D50 -- load an image file through the loader at 0x33EA0 and hand the pixels to the converter at 0x37C10.
#include "storm_stransload_37d50.h"
extern "C" uint32_t Storm_StransLoad_37d50(const StormStransEnv& e, const char* name, uint32_t nameAddr, uint32_t p2, uint32_t p3, uint32_t* out) {
    if (out) *out = 0;
    if (!name || !*name || !out) { *e.lastError = 0x57; e.setLastError(0x57); return 0; }
    uint32_t w = 0, h = 0;
    if (!e.load(nameAddr, 0, 0, &w, &h)) return 0;
    const uint32_t size = h * w;
    const uint32_t buf = e.alloc(size, 0x46a04, 0x4d9);
    if (!e.load(nameAddr, buf, size, 0, 0)) return 0;
    e.convert(buf, w, h, 8, p2, p3, out);
    e.heapFree(buf, 0x46a04, 0x4e7);
    return 1;
}
