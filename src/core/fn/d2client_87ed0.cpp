// 1.13c D2Client+0x87ed0 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "../d2client/list_geometry_87d20.h"
using namespace d2client_leaf;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_87ed0(uint32_t* r) {
    EntryNode nodes[8]; Entry ents[8]; uint32_t src[8]; int n = 0;
    uint32_t p; std::memcpy(&p, (const void*)(uintptr_t)r[3], 4);
    while (p && n < 8) {
        uint32_t w[2]; std::memcpy(w, (const void*)(uintptr_t)p, 8);
        src[n] = w[0];
        std::memcpy(&ents[n], (const void*)(uintptr_t)w[0], 32);
        nodes[n].entry = &ents[n]; nodes[n].next = nullptr; if (n) nodes[n - 1].next = &nodes[n];
        ++n; p = w[1];
    }
    d2_client_87ed0(n ? &nodes[0] : nullptr, r[1], r[2]);
    for (int i = 0; i < n; ++i) std::memcpy((void*)(uintptr_t)(src[i] + 0x1c), &ents[i].value, 4);
    r[4] = 0;
}
