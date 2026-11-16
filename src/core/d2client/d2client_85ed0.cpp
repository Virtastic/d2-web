// 1.13c D2Client+0x85ED0 -- item packet: the owner unequips an item. Packet in EAX.
// Locals as retail lays them out (esp after `sub esp,0x20`): L+0 the slot record (C262 result, later the remade item),
// L+4 the self player (0x11BBFC read once, before the unit lookup), L+8..L+0x1F the 24-byte header 0xCA90 decodes:
// +8 act byte, +0xC flag word tested by 0xCA3C, +0x10 dword handed to 0x82980, +0x11 slot byte.
// The branch after 0x6A220 compares the unit with the SELF player (L+4), not with the 0xC6D0 result.
// Seam pattern as d2client_batch_84bb0.cpp (Env::call(id, a..d): id = ordinal, or 0x1000000 + rva for an internal callee).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
typedef uintptr_t W;
struct Env {
    W    (*call)(uint32_t id, W a, W b, W c, W d);
    void (*fatal)(uint32_t code);
    uint8_t** buckets;                        // +0x10AE08 unit hash, 128 chains, next at unit+0xE4
    uint8_t*  player;                         // +0x10BBFC self player
    int32_t   tbl_shift, tbl_mask;
    uint16_t* v5c;                            // +0x10CF58 slot words: [0] at +0x58, [2] at +0x5C
    const char* file;
    uint8_t*  slotOn;
    uint32_t  tick;
    W    (*call8)(uint32_t, W, W, W, W, W, W, W, W);
};
template <class T> T rd(const uint8_t* p, size_t o) { T v; memcpy(&v, p + o, sizeof v); return v; }
constexpr uint32_t V(uint32_t rva) { return 0x1000000u + rva; }
W A(const void* p) { return (W)p; }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_85ed0(const Env* e, const uint8_t* p) {
    auto C = [e](uint32_t id, W a = 0, W b = 0, W c = 0, W d = 0) { return e->call(id, a, b, c, d); };
    uint8_t hdr[0x18]; memset(hdr, 0, sizeof hdr);
    C(11105, A(p + 0xd), (uint32_t)p[2] - 0xd, 0, A(hdr));
    const uint8_t* self = e->player;
    const uint32_t id = rd<uint32_t>(p, 4);
    uint8_t* u = (uint8_t*)C(V(0xa4e20), rd<uint32_t>(p, 9), p[8]);
    if (!u) return;
    const W inv = rd<uint32_t>(u, 0x60);
    if (!inv) { e->fatal(0x65c); return; }
    const uint8_t slot = hdr[0x11];
    W rec = C(11139, inv, slot);
    if (!rec) return;
    C(V(0x693c0), A(u));
    C(10694, inv, rec);
    C(V(0x69a90), A(u));
    C(10605, A(u), inv, A(&rec), slot);
    const W old = C(10646, inv, rec);
    C(10608, inv, slot);
    C(10600, A(u), old, 1, 1);
    if (rd<int32_t>(u, 0) == 0) C(10061, rd<uint32_t>(u, 0x60));
    if (!C(10202, old, 0x100, 0x96, A(e->file))) C(11127, A(u), old);
    C(V(0x82fe0), A(u));
    C(V(0x6a220), A(u));
    if (u != self) {
        // the retail table and the +0xE4 link hold 32-bit words (a host pointer is wider)
        for (uint8_t* n = (uint8_t*)(uintptr_t)rd<uint32_t>((const uint8_t*)e->buckets, 4 * (id & 0x7f)); n;
             n = (uint8_t*)(uintptr_t)rd<uint32_t>(n, 0xE4)) {
            if (rd<uint32_t>(n, 0xC) != id) continue;
            if (rd<int32_t>(n, 0) != 4) { e->fatal(0x5a); return; }
            C(V(0xa6ef0), 0x6fbbae08u, id & 0x7f, id, 4);
            return;
        }
        return;
    }
    C(V(0x47cd0), rec ? rd<uint32_t>((const uint8_t*)rec, 0xc) : 0xffffffffu);
    C(V(0x8bbf0));
    if (slot == 4) e->v5c[0] = 0; else if (slot == 5) e->v5c[2] = 0;
    const uint32_t flags = rd<uint32_t>(hdr, 0xc);
    if (C(10197, flags, 0x20)) { C(V(0xa6ef0), 0x6fbbae08u, id & 0x7f, id, 4); return; }
    if (!C(10197, flags, 1)) {
        C(V(0x82980), rec, hdr[8], rd<uint32_t>(hdr, 0x10));   // EAX = rec; stack 0, act, 0, hdr+0x10
    } else {
        C(V(0xa6ef0), 0x6fbbae08u, id & 0x7f, id, 4);
        rec = C(V(0x833f0), A(p), A(hdr), 0, 0);
        if (!rec) { e->fatal(0x67e); return; }
    }
    C(10058, rec, 0);
    C(10466, inv, rec);
    C(11017, inv);
    C(V(0x162d0));
    C(V(0x82fe0), A(u));
    C(V(0x82fe0), A(u));
    if (rec) {
        C(V(0x81aa0));
        if (C(10202, rec, 0x4000, 0x69c, A(e->file))) C(10977, rec, 0x4000, 0);
    }
}
