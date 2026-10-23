// 1.13c D2Common+0x1E920: build the 255-slot (item code, item type) composite table once.
#include <cstdint>
#include <cstring>
#include "d2common_1e920.h"

namespace {
// Type id the slot search compares against, per slot 0..254 (the retail static table's first dword of each pair).
const int32_t kSlotType[255] = {1,1,1,1,37,37,37,37,37,37,37,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,2,16,16,16,15,15,15,19,19,19,37,2,2,25,25,25,29,43,30,30,36,28,30,30,36,31,31,30,30,38,38,38,38,32,32,32,32,33,33,33,30,30,33,33,33,33,27,27,27,27,26,26,26,26,28,34,28,28,34,31,34,34,35,35,43,29,30,36,24,38,38,42,32,30,30,30,33,27,27,27,27,26,28,34,28,35,35,1,3,3,2,3,40,40,19,19,16,16,15,15,30,28,43,29,3,28,28,36,36,30,30,32,32,33,33,33,34,34,32,33,24,24,26,26,36,28,28,28,28,28,28,28,28,28,28,25,25,25,25,36,24,24,24,36,36,36,36,36,36,36,30,30,30,30,30,30,30,30,30,30,30,30,30,30,32,32,32,32,42,43,42,43,33,33,33,33,33,33,33,33,33,33,34,34,34,34,34,34,26,26,26,26,26,27,27,27,27,27,27,27,27,35,35,35,35,37,37,37,37,37,37,37,3,3,3,3,3,3,3,3,3,3,3,3,3};
uint32_t rd(const uint8_t* b, uint32_t o) { uint32_t v; memcpy(&v, b + o, 4); return v; }
bool in(const D2Common1E920Env& e, int32_t t, int32_t minCount) { return t >= 0 && t < e.typeCount && e.typeCount > minCount; }
uint32_t row(const D2Common1E920Env& e, int32_t t, uint32_t off) { return rd(e.typeRows, e.typeStride * 4 * static_cast<uint32_t>(t) + off); }
}

// 1.13c D2Common+0x1E920
void D2Common_1E920_BuildCompositeTable(const D2Common1E920Env& e) {
    if (*e.inited) return;
    memset(e.slots, 0, 0x7f8);
    e.slots[3] = 3; e.slots[5] = 3; e.slots[7] = 3;
    *e.inited = 1;
    e.slots[2] = 0x2074696c; e.slots[4] = 0x2064656d; e.slots[6] = 0x20797668;
    int32_t used = 4;
    for (int32_t n = 0; n < e.itemCount; ++n) {
        const uint8_t* rec = e.items + static_cast<uint32_t>(n) * 0x1a8;
        int32_t code = static_cast<int32_t>(rd(rec, 0x90));
        if (!code) code = static_cast<int32_t>(rd(rec, 0x80));
        int16_t t; memcpy(&t, rec + 0x11e, 2);
        bool ok;
        if (in(e, t, 0x2d) && (row(e, t, 4) & (1u << 13))) ok = true;
        else ok = (in(e, t, 3) && (row(e, t, 0) & (1u << 3))) || (in(e, t, 0x33) && (row(e, t, 4) & (1u << 19)))
                  || (in(e, t, 0x25) && (row(e, t, 4) & (1u << 5)));
        if (ok && in(e, t, 0x4b) && (row(e, t, 8) & (1u << 11))) ok = false;
        bool found = false;
        for (int32_t i = 0; i < used; ++i) if (e.slots[i * 2] == code) { found = true; break; }
        if (found || !ok) continue;
        int32_t s = used;
        for (;; ++s) {
            int32_t u = kSlotType[s];
            if (u >= 0) {
                if (in(e, u, 0x2d) && (row(e, u, 4) & (1u << 13)) && t >= 0 && t < e.typeCount && (row(e, t, 4) & (1u << 13))) continue;
                if (in(e, u, 0x32) && (row(e, u, 4) & (1u << 18)) && t >= 0 && t < e.typeCount && (row(e, t, 4) & (1u << 18))) continue;
            }
            if (e.slots[s * 2] == 0) break;
        }
        if (s > 0xfe) s = used;
        e.slots[s * 2] = code; e.slots[s * 2 + 1] = t;
        if (used == s) ++used;
    }
    *e.last = 0xff;
}
