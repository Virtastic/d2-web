// src/core/d2client/d2client_84750.cpp -- 1.13c D2Client.dll 0x84750 (569 B), item packet handler (Item.cpp),
// reimplemented from the cached decompile. Same seam pattern as d2client_batch_84bb0.cpp: every callee is
// Env::call(id, a..d), id = ordinal or V(va) = 0x1000000+(va-0x6FAB0000). Base 0x6FAB0000.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

typedef uintptr_t W;
struct D2c84750Env {
    W    (*call)(uint32_t id, W a, W b, W c, W d);
    void (*fatal)(uint32_t code);        // Ordinal 10265(0x5a) + 10024 + _exit(-1); host never returns
    uint8_t** buckets;                   // +0x10AE08 unit hash, 128 chains, next at unit+0xE4
    uint8_t*  player;                    // +0x10BBFC
    const char* file;                    // "..\Source\D2Client\UNIT\Item.cpp"
    W (*call8)(uint32_t id, W a, W b, W c, W d, W e, W f, W g, W h);
    void (*sendMouseMove)(W hwnd, W wparam, W lparam);
    uint32_t  tick;                      // +0x10 9854
    uint16_t  mouseX, mouseY;            // +0x10B828 / +0x10B824
    uint16_t  hoverOn; uint8_t hoverBusy; // +0x10978E / +0x109781
    int32_t   hoverId;                   // +0x109786
};
struct Hdr { uint8_t act, slot, flag, pad; uint32_t a, b; uint8_t rest[12]; };  // Ordinal_11105 decode result
static_assert(sizeof(Hdr) == 24, "hdr");

template <class T> static T rd(const uint8_t* p, size_t o) { T v; memcpy(&v, p + o, sizeof v); return v; }
#define V(va) (0x1000000u + ((va) - 0x6FAB0000u))
static W A(const void* p) { return (W)p; }

// 1.13c D2Client+0x84750 (packet in EAX)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_84750(const D2c84750Env* e, const uint8_t* p) {
    auto C = [e](uint32_t id, W a = 0, W b = 0, W c = 0, W d = 0) { return e->call(id, a, b, c, d); };
    Hdr h; memset(&h, 0, sizeof h);
    uint8_t* pl = e->player;
    uint32_t id = rd<uint32_t>(p, 4);
    C(11105, A(p + 8), p[2] - 8, A(&h));
    uint8_t* u = e->buckets[id & 0x7f];
    while (u && rd<uint32_t>(u, 0xC) != id) u = rd<uint8_t*>(u, 0xE4);
    if (u && rd<int32_t>(u, 0) != 4) { e->fatal(0x5a); return; }
    if (!u) {
        u = (uint8_t*)C(V(0x6fb335c0), A(p), A(&h));
    } else if (!C(10197, h.a, 1)) {
        // FUN_6fb32820 gets the low/high words of the decoded header's first dword (stack-address artifact in the decompile)
        W w = rd<uint32_t>((const uint8_t*)&h, 0);
        C(V(0x6fb32820), w & 0xffff, w >> 16);
        C(10012, A(u), h.slot);
        C(10977, A(u), 0xffffffffu, 0);
        C(10977, A(u), A(p + 8), 1);
    } else {
        W inv = rd<uint32_t>(pl, 0x60);
        if (C(10646, inv, A(u))) {
            uint8_t k = (uint8_t)C(10020, A(u));
            W s = C(10584, A(pl), k, e->tick);
            e->call8(10869, A(e->file), 0x456, inv, C(V(0x6fab1630), A(u)), C(V(0x6fab1660), A(u)), s, 1, k);
            C(V(0x6fb3bbf0));
            if (rd<int32_t>(pl, 0) == 0) C(10061, inv);
            if (!C(10202, A(u), 0x100, 0x96, A(e->file))) C(11127, A(pl), A(u));
        }
        C(V(0x6fb56ef0), 0x6fbbae08, id & 0x7f, id, 4);
        u = (uint8_t*)C(V(0x6fb335c0), A(p), A(&h));
        if (!u) return;
    }
    if (h.act == 0xff) return;
    C(V(0x6fb32660), A(u));
    if (C(11057, A(u), A(pl))) C(V(0x6fb31ff0));
    e->sendMouseMove(C(10048), 0, (W)e->mouseX | (W)e->mouseY << 16);   // SendMessageA(WM_MOUSEMOVE, 0, MAKELONG(DAT_6fbcb828, DAT_6fbcb824))
    C(V(0x6fb31aa0));
    if (e->hoverOn && !e->hoverBusy && rd<int32_t>(u, 0xC) == e->hoverId) C(V(0x6faf7ab0));
}
