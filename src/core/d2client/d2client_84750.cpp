// src/core/d2client/d2client_84750.cpp -- 1.13c D2Client.dll 0x84750 (569 B), item packet handler (Item.cpp),
// reimplemented from the cached decompile. Same seam pattern as d2client_batch_84bb0.cpp: every callee is
// Env::call(id, a..d), id = ordinal or V(va) = 0x1000000+(va-0x6FAB0000). Base 0x6FAB0000.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>
#include "cl_env.h"

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
// Ordinal_11105 decode result: +0 item code, +4/+6 u16 pair, +8 action byte, +0xC flags, +0x10 byte (0xff = none), +0x11 slot byte
struct Hdr { uint32_t code; uint16_t a, b; uint8_t act, p9, pa, pb; uint32_t flags; uint8_t x10, slot, p12, p13; uint32_t x14; };
static_assert(sizeof(Hdr) == 24, "hdr");
static_assert(offsetof(Hdr, act) == 8 && offsetof(Hdr, flags) == 0xc && offsetof(Hdr, x10) == 0x10, "hdr offsets");

template <class T> static T rd(const uint8_t* p, size_t o) { T v; memcpy(&v, p + o, sizeof v); return v; }
#define V(va) (0x1000000u + ((va) - 0x6FAB0000u))
static W A(const void* p) { return (W)p; }

// 1.13c D2Client+0x84750 (packet in EAX)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_84750(const D2c84750Env* e, const uint8_t* p) {
    auto C = [e](uint32_t id, W a = 0, W b = 0, W c = 0, W d = 0) { return e->call(id, a, b, c, d); };
    Hdr h; memset(&h, 0, sizeof h);
    uint8_t* pl = e->player;
    uint32_t id = rd<uint32_t>(p, 4);
    C(11105, A(p + 8), p[2] - 8, 0, A(&h));
    uint8_t* u = e->buckets[id & 0x7f];
    while (u && rd<uint32_t>(u, 0xC) != id) u = rd<uint8_t*>(u, 0xE4);
    if (u && rd<int32_t>(u, 0) != 4) { e->fatal(0x5a); return; }
    if (!u) {
        u = (uint8_t*)C(V(0x6fb335c0), A(p), A(&h));
    } else if (!C(10197, h.flags, 1)) {
        // 0x32820: eax = hdr+8 byte, stack = hdr+4 u16, hdr+6 u16, 0 (register args first, then stack)
        C(V(0x6fb32820), h.act, h.a, h.b, 0);
        C(10012, A(u), h.x10);
        C(10977, A(u), 0xffffffffu, 0);
        C(10977, A(u), h.flags, 1);
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
    if (h.x10 == 0xff) return;
    // eax = hdr+4 dword, ecx = item, stack: hdr+6 dword, hdr+0x10 dword, player
    e->call8(V(0x6fb32660), rd<uint32_t>((const uint8_t*)&h, 4), A(u), rd<uint32_t>((const uint8_t*)&h, 6), rd<uint32_t>((const uint8_t*)&h, 0x10), A(pl), 0, 0, 0);
    if (C(11057, A(u), A(pl))) C(V(0x6fb31ff0), 0, A(u), A(pl));   // stack 0; esi = item, edi = player
    e->sendMouseMove(C(10048), 0, (W)e->mouseX | (W)e->mouseY << 16);   // SendMessageA(WM_MOUSEMOVE, 0, MAKELONG(DAT_6fbcb828, DAT_6fbcb824))
    C(V(0x6fb31aa0), A(u));   // esi = item
    if (e->hoverOn && !e->hoverBusy && rd<int32_t>(u, 0xC) == e->hoverId) C(V(0x6faf7ab0));
}

// ---- register-exact port (cl::Env seam, one record per retail call) ----
// 1.13c D2Client+0x84750 -- item packet "update or re-create in place". EAX = packet (no stack arguments): byte +2 length, id dword +4,
// payload from +8 decoded by 0xCA90 into a stack block (+4 word, +6 word, +8 byte, +0xC dword, +0x10 dword whose low byte 0xFF means
// "no refresh"). player = *(D2Client+0x11BBFC), read without a null check. The client item hash (D2Client+0x10AE08, chain +0xE4, id +0xC,
// type +0) is walked for the id (a hit whose type is not 4 is fatal).
//  hit, 0xCA3C(block dword +0xC, 1) == 0: in-place update -- 0x82820 (EAX = byte +8, ESI = item, EDI = 0; words +4, +6, 0), 0xCA24 with
//    the block byte +0x10 in the low byte of an uninitialised stack word, 0xCA42(item, -1, 0), 0xCA42(item, dword +0xC, 1).
//  hit, nonzero: when 0xC6D0(inventory, item) holds it, the item drops to the floor -- 0xC92E(item) byte into the residue word, 0xC8A4(player,
//    that word, D2Client+0x119854), 0x1660/0x1630 (ECX = item), 0xC9FA(Item.cpp, 0x456, inventory, x, y, the 0xC8A4 result, 1, the word),
//    0x8BBF0, 0xCA0C(inventory) for player type 0, 0xC1A8(item, 0x100, 0x96, Item.cpp) == 0 -> 0xC544(player, item); in every case it
//    is unhashed (0xA6EF0: ECX = the hash, EDX = id & 0x7F; id, 4) and re-made by 0x835C0 (EBX = 0; packet, block, 0, 0), null returns.
//  miss: 0x835C0 (EBX = 0; packet, block, 0, 1), unchecked.
// Then unless the low byte of block +0x10 is 0xFF: 0x82660 (EAX = dword at block +4, ECX = item; dword at block +6, dword +0x10, player),
// 0xCA30 -> 0x81FF0 (ESI = item, EDI = player, one zero word), WM_MOUSEMOVE to the 0xD0B4 window (lParam = word 0x11B824 << 16 |
// word 0x11B828, IAT 0xCF118), 0x81AA0 (ESI = item, EDI as the path left it) and 0x47AB0 when the hover state (0x11978E != 0, byte
// 0x119781 == 0) names the item's id (-1 for no item) at 0x119786.
// hash/ui/hov: where retail's D2Client+0x10AE08, +0x11B820 and +0x119780 live.
extern "C" void d2c_84750(uint32_t pkt, uint32_t hash, uint32_t ui, uint32_t hov, const cl::Env& e) {
    constexpr uint32_t kItemCpp = 0xB2000000u | 0xD4964u;     // "..\\Source\\D2Client\\UNIT\\Item.cpp"
    uint32_t buf[6] = {};
    const uint32_t bp = (uint32_t)(uintptr_t)buf;
    const uint8_t* b = (const uint8_t*)buf;
    e.H(0xca90, pkt + 8, e.R8(pkt + 2) - 8, 0, bp);
    const uint32_t id = e.R(pkt + 4);
    const uint32_t pl = e.self;
    uint32_t node = 0;
    for (uint32_t n = e.R(hash + (id & 0x7f) * 4); n; n = e.R(n + 0xe4))
        if (e.R(n + 0xc) == id) { if (e.R(n) != 4) return; node = n; break; }   // fatal 0x5A in retail
    uint32_t it, edi = pkt;
    if (node) {
        if (e.H(0xca3c, buf[3], 1)) {
            if (e.H(0xc6d0, e.R(pl + 0x60), node)) {
                const uint32_t w = e.H(0xc92e, node) & 0xff;   // upper bytes: stack residue (masked in the vector)
                const uint32_t q = e.H(0xc8a4, pl, w, e.R(hov + 0xd4));   // 0x119854
                const uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, node);
                const uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, node);
                const uint32_t inv = e.R(pl + 0x60);
                e.H(0xc9fa, kItemCpp, 0x456, inv, x, y);
                const uint32_t blk[8] = { 0, 0x456, inv, x, y, q, 1, 0 };   // the eight stack words; 0 (module string) and 7 (residue) masked
                if (e.blk) e.blk(e.ctx, 0xc9fa, (uint32_t)(uintptr_t)blk, 8);
                e.H(0x8bbf0);
                if (e.R(pl) == 0) e.H(0xca0c, e.R(pl + 0x60));
                if (!e.H(0xc1a8, node, 0x100, 0x96, kItemCpp)) e.H(0xc544, pl, node);
            }
            e.H(0xa6ef0, id, 4, 0, 0, 0, 0, 0xB2000000u | 0x10AE08u, id & 0x7f);
            it = e.H(0x835c0, pkt, bp, 0, 0);
            if (!it) return;
        } else {
            e.H(0x82820, buf[1] & 0xffff, buf[1] >> 16, 0, 0, 0, b[8], 0, 0, 0, node);
            e.Edj(0x82820, 0);
            e.H(0xca24, node, b[0x10]);                        // upper bytes: stack residue (masked in the vector)
            e.H(0xca42, node, 0xffffffffu, 0);
            e.H(0xca42, node, buf[3], 1);
            it = node; edi = 0;
        }
    } else {
        it = e.H(0x835c0, pkt, bp, 0, 1);
    }
    if (b[0x10] == 0xff) return;
    uint32_t w6; __builtin_memcpy(&w6, b + 6, 4);
    e.H(0x82660, w6, buf[4], pl, 0, 0, buf[1], it);
    if (e.H(0xca30, it, pl)) { e.H(0x81ff0, 0, 0, 0, 0, 0, 0, 0, 0, 0, it); e.Edj(0x81ff0, pl); edi = pl; }
    const uint32_t lp = (e.R(ui + 4) & 0xffff) << 16 | (e.R(ui + 8) & 0xffff);   // 0x11B824, 0x11B828
    const uint32_t hw = e.H(0xd0b4);
    e.H(0xcf118, hw, 0x200, 0, lp);
    e.H(0x81aa0, 0, 0, 0, 0, 0, 0, 0, 0, 0, it);
    e.Edj(0x81aa0, edi);
    const uint32_t iid = it ? e.R(it + 0xc) : 0xffffffffu;
    if (e.R(hov + 0xe) && !e.R8(hov + 1) && iid == e.R(hov + 6)) e.H(0x47ab0);
}
