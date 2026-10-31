// 1.13c D2Client+0x87e60 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
#include "../d2client/list_geometry_87d20.h"
using namespace d2client_leaf;
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_87e60(uint32_t* r) {
    EntryNode nodes[8]; Entry ents[8]; int n = 0;
    uint32_t p; std::memcpy(&p, (const void*)(uintptr_t)r[2], 4);
    while (p && n < 8) {
        uint32_t w[2]; std::memcpy(w, (const void*)(uintptr_t)p, 8);
        std::memcpy(&ents[n], (const void*)(uintptr_t)w[0], 32);
        nodes[n].entry = &ents[n]; nodes[n].next = nullptr; if (n) nodes[n - 1].next = &nodes[n];
        ++n; p = w[1];
    }
    r[4] = d2_client_87e60(n ? &nodes[0] : nullptr, r[1]);
}
