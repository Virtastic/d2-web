// 1.13c D2Client+0xBC430 bestStatBonusTierLookup (EAX = unit): 0 for a null unit. Else D2Common #11139 (thunk 0xC262) is asked for slots 4 and 5 of the object at unit+0x60 (skipped when it is null),
// then #10702 (thunk 0xC286: unit, 0x15C, buffer, 0x20) fills up to 0x20 eight-byte entries (word id, dword value) and returns the count. Per entry: id 0 or the value is taken when
// #10744 (thunk 0xC1CC: slot, id) is nonzero for slot 5, else slot 4 (null slots skipped) and the value is above the running best; an id <= 0 always takes its value. Returns the best value.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2c_bc430(D2cHost h, const uint8_t* u) {
    if (!u) return 0;
    uint32_t ua = (uint32_t)(uintptr_t)u, x = *(const uint32_t*)(u + 0x60), s4 = 0, s5 = 0;
    D2cCall c = {};
    if (x) {
        c = {}; c.rva = 0xC262; c.s[0] = x; c.s[1] = 4; s4 = h(&c);
        c = {}; c.rva = 0xC262; c.s[0] = x; c.s[1] = 5; s5 = h(&c);
    }
    uint32_t ent[0x40];
    for (int i = 0; i < 0x40; i++) ent[i] = 0;
    c = {}; c.rva = 0xC286; c.s[0] = ua; c.s[1] = 0x15c; c.s[2] = (uint32_t)(uintptr_t)ent; c.s[3] = 0x20;
    int32_t n = (int32_t)h(&c), best = 0;
    for (int32_t i = 0; i < n; i++) {
        uint32_t id = ent[i * 2] & 0xffff; int32_t v = (int32_t)ent[i * 2 + 1];
        bool take = false;
        if (id == 0) take = true;
        else {
            bool hit = false;
            if (s5) { c = {}; c.rva = 0xC1CC; c.s[0] = s5; c.s[1] = id; hit = h(&c) != 0; }
            if (!hit && s4) { c = {}; c.rva = 0xC1CC; c.s[0] = s4; c.s[1] = id; hit = h(&c) != 0; }
            take = hit && v > best;
        }
        if (take) best = v;
    }
    return best;
}
