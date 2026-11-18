// 1.13c D2Client+0x5B120: hireling cel handle lookup/cache.
#include <emscripten.h>
#include <cstdint>
#include <cstring>

struct Hire5b120Node { uint32_t x, id, handle, key, busy; };   // list node dwords [0] [1] [2] [3] [8]
struct Hire5b120Env { void (*fmt)(uint32_t nameOff); uint32_t (*load)(); };

extern "C" uint32_t d2_client_5b120(uint32_t id, int32_t count, const uint8_t* arr, uint32_t* pFlags,
                                    const uint32_t* pKey, uint32_t* pHandle, const Hire5b120Node* nodes, int nn,
                                    uint32_t* cache, const Hire5b120Env* env) {
    if ((int32_t)id < 0 || (int32_t)id >= count || !arr) return 0;
    const uint8_t* rec = arr + (size_t)id * 0xE0;
    if (pFlags) *pFlags = rec[4] & 0x20;   // gdwBitMasks[5]
    uint32_t h = 0xFFFFFFFFu;
    if (pKey) for (int i = 0; i < nn; ++i) if (nodes[i].id == id && nodes[i].key == *pKey && !nodes[i].busy) { h = nodes[i].handle; break; }
    *pHandle = h;
    if (h == 0xFFFFFFFFu) return 0;
    uint32_t x = 0;
    for (int i = 0; i < nn; ++i) if (nodes[i].handle == h) { x = nodes[i].x; break; }
    uint32_t slot = 0;
    for (uint32_t k = 1; k < 5; ++k) {
        uint16_t v; memcpy(&v, rec + 0xB2 + 2 * (k - 1), 2);
        if (x == v && rec[0x2F + 0x20 * (k - 1)]) { slot = k; break; }   // loop ends at 5 -> 0
    }
    uint32_t* c = cache + (id * 5 + slot);
    if (!*c) { env->fmt(id * 0xE0 + slot * 0x20 + 0xF); *c = env->load(); }
    return *c;
}
