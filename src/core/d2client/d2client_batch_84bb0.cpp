// src/core/d2client/d2client_batch_84bb0.cpp -- 12 UNMAPPED 1.13c D2Client.dll rows (0x84BB0..0x86810, the
// item packet handlers in Item.cpp) reimplemented from the cached decompiles. Alpha batch lane
// (OBJECTIVE v8.3): implementation + marker only, no spec, no vector. Base 0x6FAB0000.
// Every callee (ordinals, other RVAs) is a host seam: Env::call(id, a..d) with id = ordinal, or
// V(va) = 0x1000000+(va-0x6FAB0000), the callee's 0x6FBxxxxx address (the d2client_batch_5d110.cpp seam pattern, one dispatcher instead of 60 members).
// Unit/packet layouts are the retail byte layouts read with rd<>; stack-spilled locals are named by role.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

typedef uintptr_t W;
struct D2cItmEnv {
    W    (*call)(uint32_t id, W a, W b, W c, W d);
    void (*fatal)(uint32_t code);             // Ordinal 10265 + 10024 + _exit(-1); host never returns
    uint8_t** buckets;                        // +0x10AE08 unit hash, 128 chains, next at unit+0xE4
    uint8_t*  player;                         // +0x10BBFC
    int32_t   tbl_shift, tbl_mask;            // sgptDataTables +0xC6C / +0xC70
    uint16_t* v5c;                            // +0x10CF58 (v5c[0]) / +0x10CF5C (v5c[1]) slot words
    const char* file;                         // "..\Source\D2Client\UNIT\Item.cpp"
    uint8_t*  slotOn;                         // +0x10C484 u8[4]
    uint32_t  tick;                           // DAT_6fbc9854
};
struct D2cHdr { uint8_t act, slot, flag, pad; uint32_t a, b; };  // Ordinal_11105 decode result

template <class T> static T rd(const uint8_t* p, size_t o) { T v; memcpy(&v, p + o, sizeof v); return v; }
#define V(va) (0x1000000u + ((va) - 0x6FAB0000u))
#define C(id, ...) call4(e, id, __VA_ARGS__)
#define FATAL(c) do { e->fatal(c); return; } while (0)
#define FATALR(c) do { e->fatal(c); return 0; } while (0)
static W A(const void* p) { return (W)p; }
static W A(W v) { return v; }
static W call4(const D2cItmEnv* e, uint32_t id, W a = 0, W b = 0, W c = 0, W d = 0) { return e->call(id, a, b, c, d); }

static uint8_t* findItem(const D2cItmEnv* e, uint32_t id, bool& bad) {
    bad = false;
    for (uint8_t* u = e->buckets[id & 0x7f]; u; u = rd<uint8_t*>(u, 0xE4))
        if (rd<uint32_t>(u, 0xC) == id) { if (rd<int32_t>(u, 0) != 4) bad = true; return u; }
    return nullptr;
}
static uint8_t* pickPlayer(const D2cItmEnv* e, const uint8_t* p) {   // FUN_6fb54e20
    return (uint8_t*)C(V(0x6fb54e20), rd<uint32_t>(p, 9), p[8], 0, 0);
}
static bool isEquipSlot(uint8_t s) { return s == 0xb || s == 0xc; }    // uVar = 4 else 3


// seams by 0x6FBxxxxx address
#define F_UNPACK_PLAYER V(0x6fb54e20)
#define F_LOOKUP   V(0x6fb335c0)
#define F_MAKE     V(0x6fb333f0)
#define F_CHAIN    V(0x6fb56ef0)
#define F_REFRESH  V(0x6fb31aa0)
#define F_DONE     V(0x6fb6e2b0)
#define F_UNREF    V(0x6fb32fe0)
#define F_SELECT   V(0x6fb3bbf0)
#define F_TRY      V(0x6fb31b40)
#define F_UI1      V(0x6fb31ff0)
#define F_UI2      V(0x6fb31b70)
#define F_UI3      V(0x6fb1a220)
#define F_PRE      V(0x6fb193c0)
#define F_PRE2     V(0x6fb19a90)
#define F_POST     V(0x6fb18cb0)
#define F_KILL     V(0x6fac62d0)

static void tail(const D2cItmEnv* e, W u) {          // shared item-placed epilogue
    C(F_UNREF, u, 0, 0, 0); C(F_UNREF, u, 0, 0, 0);
}

// 1.13c D2Client+0x84BB0 (packet in EAX)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_84bb0(const D2cItmEnv* e, const uint8_t* p) {
    uint8_t buf[24];
    C(11105, A(p + 8), p[2] - 8, A(buf), 0);
    uint8_t* u = (uint8_t*)C(F_LOOKUP, A(p), A(buf), 0, 0);
    if (!u) FATAL(900);
    C(F_REFRESH, 0, 0, 0, 0);
    if (rd<int32_t>(u, 0x10) == 3 && C(10202, A(u), 0x2000, 0x388, A(e->file))) {
        C(11090, A(u), 5, 0, 0);
        C(V(0x6fb327c0), 0, 0, 0, 0);
    }
}

// 1.13c D2Client+0x84C60 (unit in EDI)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_84c60(const D2cItmEnv* e, uint8_t* unit) {
    if (!unit) return;
    W inv = rd<uint32_t>(unit, 0x60);
    if (!inv) return;
    W cur = C(10460, inv, 0, 0, 0), last = inv;
    while (cur) {
        last = C(10464, cur, 0, 0, 0);              // next node
        W item = C(11147, cur, 0, 0, 0);
        if (!item) FATAL(0xd4f);
        W mode = C(10698, cur, 0, 0, 0);
        if (mode == 3 || (mode == 1 && C(11057, item, 0, 0, 0))) {
            if (!(rd<uint32_t>(unit, 0xC4) & 0x10000) && rd<int32_t>(unit, 0) == 0) {
                int32_t m = rd<int32_t>(unit, 0x10);
                if (m != 0 && m != 0x11) C(V(0x6fac9830), 7, A(unit), 0, 0);
            }
            C(10694, last, item, 0, 0);
            W r = C(10646, last, item, 0, 0);
            if (!r) FATAL(0xd5a);
            if (r != item) FATAL(0xd5b);
            if (C(10698, cur, 0, 0, 0) == 3) C(10608, inv, C(10475, inv, item, 0, 0), 0, 0);
            C(10600, 0, 0, 0, 0);
            if (rd<int32_t>(unit, 0) == 0) C(10061, inv, 0, 0, 0);
            if (!C(10202, r, 0x100, 0x96, A(e->file))) C(11127, 0, 0, 0, 0);
            C(F_UNREF, A(unit), 0, 0, 0);
            cur = C(10460, inv, 0, 0, 0);            // restart from the head
            last = inv;
            continue;
        }
        cur = last;
    }
    C(11162, last, 0, 0, 0);
}

// 1.13c D2Client+0x84F00 (packet in param_1; returns the new/updated item unit or null)
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t* d2_client_84f00(const D2cItmEnv* e, const uint8_t* p) {
    D2cHdr h; memset(&h, 0, sizeof h);
    C(11105, A(p + 0xd), p[2] - 0xd, A(&h), 0);
    uint8_t* pl = e->player;
    uint8_t* u = (uint8_t*)C(F_UNPACK_PLAYER, rd<uint32_t>(p, 9), p[8], 0, 0);
    if (!u) return nullptr;
    W inv = rd<uint32_t>(u, 0x60);
    if (!inv || rd<int32_t>(u, 0) != 0) return nullptr;
    uint32_t id = rd<uint32_t>(p, 4);
    uint8_t* item = nullptr;
    bool bad;
    if (u == pl && h.act == 0) {
        uint8_t* n = findItem(e, id, bad);
        if (n) {
            if (bad || (uint8_t*)C(10646, inv, A(n), 0, 0) != n) FATALR(0x5a);
            C(10061, inv, 0, 0, 0);
            if (!C(10202, A(n), 0x100, 0x96, A(e->file))) C(11127, 0, 0, 0, 0);
            C(V(0x6fb55b00), rd<int32_t>(n, 0), rd<uint32_t>(n, 0xC), 0, 0);
        }
        C(F_SELECT, 0, 0, 0, 0);
        C(10869, A(e->file), 0xbfc, inv, C(10584, A(u), 0, e->tick, 0));
        item = (uint8_t*)C(F_MAKE, A(p), A(&h), 0, 0);
        C(V(0x6fb32660), A(item), 0, 0, 0);
        if (C(11057, A(item), A(u), 0, 0)) C(F_UI1, 0, 0, 0, 0);
    } else if (h.act == 1) {
        W rec = C(11139, inv, h.slot, 0, 0);
        C(F_PRE, A(u), 0, 0, 0);
        if (!rec) return nullptr;
        C(10694, inv, rec, 0, 0);
        C(F_PRE2, A(u), 0, 0, 0);
        C(10605, A(u), inv, A(&h), h.slot);
        if (C(10646, inv, rec, 0, 0)) FATALR(0x5a);
        C(10061, inv, 0, 0, 0);
        if (!C(10202, 0, 0x100, 0x96, A(e->file))) C(11127, A(u), 0, 0, 0);
        C(10600, A(u), 0, 1, 1);
        C(10608, inv, 0, 0, 0);
        C(F_UI3, 0, 0, 0, 0);
        C(V(0x6fb55b00), 6, 0xffffffffu, 0, 0);
        if (u == pl) { C(F_SELECT, 0, 0, 0, 0); if (h.slot == 5) e->v5c[1] = 0; }
        item = (uint8_t*)C(F_MAKE, A(p), A(&h), 0, 0);
        C(10058, A(item), h.slot, 0, 0);
        if (!C(10405, inv, A(item), isEquipSlot(h.slot) ? 4 : 3)) return nullptr;
        if (!C(F_TRY, 0, 0, 0, 0)) return nullptr;
        C(10138, inv, A(item), 0, 0);
        C(F_POST, 0, 0, 0, 0);
        if (rd<uint8_t>((const uint8_t*)rec, 3) <= 1) C(F_PRE2, 0, 0, 0, 0);
        C(F_UI3, 0, 0, 0, 0);
        C(F_UI2, 0, 0, 0, 0);
        C(F_UI1, 0, 0, 0, 0);
        tail(e, A(u));
    } else {
        if (u != pl) return nullptr;
        if (h.act == 2) {
            uint8_t* n = findItem(e, id, bad);
            if (n) {
                if (bad || (uint8_t*)C(10646, inv, A(n), 0, 0) != n) FATALR(0x5a);
                uint32_t k = (uint32_t)C(V(0x6fab1630), A(n), 0, 0, 0);
                C(10267, 0, 0, 0, 0);
                if (k < 4) e->slotOn[k] = 0;
                C(10061, inv, 0, 0, 0);
                if (!C(10202, A(n), 0x100, 0x96, 0)) C(11127, 0, 0, 0, 0);
                C(V(0x6fb55b00), rd<int32_t>(n, 0), rd<uint32_t>(n, 0xC), 0, 0);
            }
            C(V(0x6fad6410), 0, 0, 0, 0);
            item = (uint8_t*)C(F_MAKE, A(p), A(&h), 0, 0);
            if (h.act != 2) FATALR(0x5a);
            C(10882, A(item), 0, 0, 0);
            C(11118, A(item), 0, 0, 0);
            if (!C(10405, inv, A(item), 2)) FATALR(0x5a);
            if (!C(10655, C(V(0x6fab1630), A(item), 0, 0, 0), 0, 0, 0)) C(V(0x6fb31c90), 0, 0, 0, 0);
            if (C(11057, A(item), A(u), 0, 0)) C(F_UI1, 0, 0, 0, 0);
            uint32_t k = (uint32_t)C(V(0x6fab1630), A(item), 0, 0, 0);
            if (k < 4) e->slotOn[k] = 1;
            C(F_DONE, 0, 0, 0, 0);
            if (C(10197, A(u), 0x400, 0, 0)) C(V(0x6fb1b300), A(item), 2, 0, 0);
            if (item) C(F_REFRESH, 0, 0, 0, 0);
            return item;
        }
        if (h.act != 4) return nullptr;
        C(V(0x6fad6410), 0, 0, 0, 0);
        uint8_t* n = findItem(e, id, bad);
        if (n) { if (bad) FATALR(0x5a); C(V(0x6fb55b00), 4, rd<uint32_t>(n, 0xC), 0, 0); }
        item = (uint8_t*)C(F_MAKE, A(p), A(&h), 0, 0);
        C(10882, A(item), 0, 0, 0);
        C(11118, A(item), 0, 0, 0);
        C(10466, inv, A(item), 0, 0);
        C(11017, inv, 0, 0, 0);
        C(F_KILL, 0, 0, 0, 0);
    }
    C(F_DONE, 0, 0, 0, 0);
    if (item) C(F_REFRESH, 0, 0, 0, 0);
    return item;
}

// 1.13c D2Client+0x854E0 (packet in EAX)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_854e0(const D2cItmEnv* e, const uint8_t* p) {
    D2cHdr h; memset(&h, 0, sizeof h);
    int len = p[2] - 0xd;
    C(11105, A(p + 0xd), len, 0, A(&h));
    uint8_t* u = (uint8_t*)C(F_UNPACK_PLAYER, rd<uint32_t>(p, 9), p[8], 0, 0);
    if (!u) return;
    W inv = rd<uint32_t>(u, 0x60);
    if (!inv) FATAL(0xb77);
    uint8_t* it = nullptr;
    bool bad;
    if (u == e->player) {
        it = findItem(e, rd<uint32_t>(p, 4), bad);
        if (bad) FATAL(0x5a);
        if (!it) return;
        if ((uint8_t*)C(10646, inv, A(it), 0, 0) != it) FATAL(0xb83);
        C(10869, A(e->file), 0xb85, A(p + 0xd), C(10584, A(u), 0, e->tick, 0));
        C(F_SELECT, 0, 0, 0, 0);
        C(F_DONE, 0, 0, 0, 0);
    }
    if (!C(10197, h.a, 1, 0, 0)) {
        if (!it) { it = (uint8_t*)C(F_MAKE, A(p), A(&h), 0, 0); if (!it) FATAL(0xb93); }
        else C(V(0x6fb32980), 0, h.act, 0, 0);
    } else {
        C(F_CHAIN, 0x6fbbae08, rd<uint32_t>(p, 4) & 0x7f, rd<uint32_t>(p, 4), 4);
        it = (uint8_t*)C(F_MAKE, A(p), A(&h), 0, 0);
        if (!it) FATAL(0xb8d);
    }
    C(10058, A(it), h.slot, 0, 0);
    if (!C(10405, len, A(it), isEquipSlot(h.slot) ? 4 : 3)) return;
    if (!C(F_TRY, 0, 0, 0, 0)) return;
    C(F_PRE, A(u), 0, 0, 0);
    C(10138, len, A(it), 0, 0);
    C(F_UI3, 0, 0, 0, 0);
    C(F_UI1, 0, 0, 0, 0);
    tail(e, A(u));
    C(F_REFRESH, 0, 0, 0, 0);
}

// 1.13c D2Client+0x85710 (packet in EAX)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_85710(const D2cItmEnv* e, const uint8_t* p) {
    D2cHdr h; memset(&h, 0, sizeof h);
    C(11105, 0, 0, A(&h), 0);
    uint8_t* u = (uint8_t*)C(F_UNPACK_PLAYER, rd<uint32_t>(p, 9), p[8], 0, 0);
    if (!u) return;
    C(10584, A(u), 0, e->tick, 0);
    W inv = rd<uint32_t>(u, 0x60);
    W rec = inv ? C(11139, inv, 0, 0, 0) : 0;
    if (!rec) FATAL(0);
    C(F_PRE, A(u), 0, 0, 0);
    C(10694, inv, rec, 0, 0);
    C(F_PRE2, A(u), 0, 0, 0);
    C(10605, A(u), inv, A(&h), h.slot);
    W old = C(10646, inv, rec, 0, 0);
    if (!old) FATAL(0xaaf);
    if (old != (W)&h) FATAL(0xab0);
    C(10061, inv, 0, 0, 0);
    if (!C(10202, old, 0x100, 0x96, A(e->file))) C(11127, A(u), old, 0, 0);
    C(10600, A(u), old, 1, 1);
    C(F_REFRESH, 0, 0, 0, 0);
    C(10608, inv, h.slot, 0, 0);
    C(F_UI3, 0, 0, 0, 0);
    uint8_t* it = nullptr;
    if (!C(10197, A(u), 1, 0, 0)) C(V(0x6fb32980), 0, h.act, 0, 0);
    else {
        C(F_CHAIN, 0x6fbbae08, rd<uint32_t>(p, 4) & 0x7f, rd<uint32_t>(p, 4), 4);
        if (u != e->player) return;
        it = (uint8_t*)C(F_MAKE, A(p), A(&h), 0, 0);
        if (!it) FATAL(0xabd);
    }
    if (u != e->player) return;
    C(F_SELECT, 0, 0, 0, 0);
    if (h.slot == 4) e->v5c[0] = 0; else if (h.slot == 5) e->v5c[1] = 0;
    if (!C(11107, inv, 0, h.a, 1)) FATAL(0xadf);
    if (C(10405, inv, A(it), 1)) {
        C(10058, A(old), 0, 0, 0);
        C(10466, inv, 0, 0, 0);
        C(11017, inv, 0, 0, 0);
        C(F_KILL, 0, 0, 0, 0);
        tail(e, A(u));
        C(F_REFRESH, 0, 0, 0, 0);
    }
}

// 1.13c D2Client+0x85990 (id in EAX, cmd in EBX; this = count)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_85990(const D2cItmEnv* e, int32_t count, char flag, uint16_t arg, uint32_t id, int32_t cmd) {
    bool bad;
    uint8_t* n = findItem(e, id, bad);
    if (!n) return;
    if (bad) FATAL(0x5a);
    if (cmd == 0xcc) {
        if (e->player) C(V(0x6fb32210), A(n), arg >> (e->tbl_shift & 0x1f), e->tbl_mask & arg, 0);
        return;
    }
    if (flag == 1) C(10887, A(n), 0, 0, 0);
    if (!e->player) return;
    if (cmd != 0x46) { C(F_UNREF, A(e->player), 0, 0, 0); return; }
    if (count > 0) {
        if (C(10202, A(n), 4, 0xa38, A(e->file))) C(10977, A(n), 4, 0, 0);
        if (C(10202, A(n), 0x4000, 0xa3c, A(e->file))) C(10977, A(n), 0x4000, 0, 0);
    }
}

// 1.13c D2Client+0x85AA0 (packet in ESI)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_85aa0(const D2cItmEnv* e, const uint8_t* p) {
    W args[2] = { A(p + 8), (W)(p[2] - 8) };
    C(11105, 0, 0, 0, 0);
    bool bad;
    uint8_t* n = findItem(e, rd<uint32_t>(p, 4), bad);
    if (!n) return;
    if (bad) FATAL(0x5a);
    int32_t m = rd<int32_t>(n, 0x10);
    if (m == 0) C(V(0x6fb33d00), A(p), 0, 0, 0);
    else if (m == 4) C(V(0x6fb33ec0), A(args), 0, 0, 0);
}

// 1.13c D2Client+0x85B30 (packet in EAX, header words in param_1; byte +0x11 = slot)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_85b30(const D2cItmEnv* e, const uint8_t* p, uint8_t* hdr) {
    uint8_t* u = (uint8_t*)C(F_UNPACK_PLAYER, rd<uint32_t>(p, 9), p[8], 0, 0);
    if (!u) return;
    if (rd<int32_t>(u, 0) != 0) FATAL(0x750);
    W inv = rd<uint32_t>(u, 0x60);
    if (!inv) FATAL(0x753);
    if (!C(10197, rd<uint32_t>(hdr, 0xC), 1, 0, 0)) FATAL(0x755);
    uint8_t slot = hdr[0x11];
    C(F_PRE, A(u), 0, 0, 0);
    bool bad;
    uint8_t* n = findItem(e, rd<uint32_t>(p, 4), bad);
    if (bad) FATAL(0x5a);
    if (n) C(V(0x6fb32490), 0, 0, 0, 0);
    if (C(11139, inv, slot, 0, 0)) C(V(0x6fb32490), 0, 0, 0, 0);
    uint8_t* it = (uint8_t*)C(F_MAKE, A(p), A(hdr), 0, 0);
    if (!it) FATAL(0x765);
    C(10058, A(it), slot, 0, 0);
    if (!C(10405, inv, A(it), isEquipSlot(slot) ? 4 : 3)) return;
    if (!C(11041, inv, A(it), slot)) return;
    if (slot != 4 && slot != 5) { tail(e, A(u)); C(F_DONE, 0, 0, 0, 0); C(F_REFRESH, 0, 0, 0, 0); return; }
    C(10138, inv, A(it), 0, 0);
    C(F_POST, 0, 0, 0, 0);
    C(F_UI1, 0, 0, 0, 0);
    tail(e, A(u));
    C(F_UI2, 0, 0, 0, 0);
    C(F_UI3, 0, 0, 0, 0);
    C(F_DONE, 0, 0, 0, 0);
    C(F_REFRESH, 0, 0, 0, 0);
}

// 1.13c D2Client+0x85CF0 (packet in EAX, header words in param_1)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_85cf0(const D2cItmEnv* e, const uint8_t* p, uint8_t* hdr) {
    uint8_t* u = (uint8_t*)C(F_UNPACK_PLAYER, rd<uint32_t>(p, 9), p[8], 0, 0);
    if (!u) return;
    if (rd<int32_t>(u, 0) != 0) FATAL(0x6b0);
    bool bad;
    uint8_t* n = findItem(e, rd<uint32_t>(p, 4), bad);
    if (bad) FATAL(0x5a);
    uint8_t slot = hdr[0x11];
    if (!C(10197, rd<uint32_t>(hdr, 0xC), 1, 0, 0)) {
        if (!n) FATAL(0xac);
        C(V(0x6fb32820), 0, 0, 0, 0);
        C(10012, A(n), hdr[0x10], 0, 0);
    } else {
        C(F_CHAIN, 0x6fbbae08, rd<uint32_t>(p, 4) & 0x7f, rd<uint32_t>(p, 4), 4);
        n = (uint8_t*)C(F_MAKE, A(p), A(hdr), 0, 0);
        if (!n) FATAL(0x6b7);
    }
    C(10058, A(n), slot, 0, 0);
    W inv = rd<uint32_t>(u, 0x60);
    if (!inv) FATAL(0x6be);
    if (!C(10405, inv, A(n), isEquipSlot(slot) ? 4 : 3)) return;
    if (!C(F_TRY, 0, 0, 0, 0)) return;
    C(F_PRE, A(u), 0, 0, 0);
    C(10138, inv, A(n), 0, 0);
    C(F_POST, 0, 0, 0, 0);
    C(F_UI1, 0, 0, 0, 0);
    tail(e, A(u));
    C(F_UI2, 0, 0, 0, 0);
    C(F_UI3, 0, 0, 0, 0);
    C(F_DONE, 0, 0, 0, 0);
    C(F_REFRESH, 0, 0, 0, 0);
}

// 1.13c D2Client+0x85ED0 (packet in EAX)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_85ed0(const D2cItmEnv* e, const uint8_t* p) {
    D2cHdr h; memset(&h, 0, sizeof h);
    C(11105, 0, 0, A(&h), 0);
    uint8_t* u = (uint8_t*)C(F_UNPACK_PLAYER, rd<uint32_t>(p, 9), p[8], 0, 0);
    if (!u) return;
    W inv = rd<uint32_t>(u, 0x60);
    if (!inv) FATAL(0);
    W rec = C(11139, inv, 0, 0, 0);
    if (!rec) return;
    C(F_PRE, A(u), 0, 0, 0);
    C(10694, inv, rec, 0, 0);
    C(F_PRE2, A(u), 0, 0, 0);
    C(10605, A(u), inv, A(&h), h.slot);
    W old = C(10646, inv, rec, 0, 0);
    C(10608, inv, 0, 0, 0);
    C(10600, 1, 1, 0, 0);
    C(10061, inv, 0, 0, 0);
    if (!C(10202, old, 0x100, 0x96, A(e->file))) C(11127, A(u), old, 0, 0);
    C(F_UNREF, A(u), 0, 0, 0);
    C(F_UI3, 0, 0, 0, 0);
    uint32_t id = rd<uint32_t>(p, 4);
    if (A(u) != old) {
        bool bad;
        uint8_t* n = findItem(e, id, bad);
        if (!n) return;
        if (bad) FATAL(0x5a);
        C(F_CHAIN, 0x6fbbae08, id & 0x7f, id, 4);
        return;
    }
    C(V(0x6faf7cd0), rd<uint32_t>(u, 0xC), 0, 0, 0);
    C(F_SELECT, 0, 0, 0, 0);
    if (h.slot == 4) e->v5c[0] = 0; else if (h.slot == 5) e->v5c[1] = 0;
    if (C(10197, h.b, 0x20, 0, 0)) { C(F_CHAIN, 0x6fbbae08, id & 0x7f, id, 4); return; }
    uint8_t* it = u;
    if (!C(10197, h.a, 1, 0, 0)) C(V(0x6fb32980), 0, h.act, 0, 0);
    else {
        C(F_CHAIN, 0x6fbbae08, id & 0x7f, id, 4);
        it = (uint8_t*)C(F_MAKE, A(p), A(&h), 0, 0);
        if (!it) FATAL(0x67e);
    }
    C(10058, A(it), 0, 0, 0);
    C(10466, inv, 0, 0, 0);
    C(11017, inv, 0, 0, 0);
    C(F_KILL, 0, 0, 0, 0);
    tail(e, A(u));
    if (it) {
        C(F_REFRESH, 0, 0, 0, 0);
        if (C(10202, A(it), 0x4000, 0x69c, A(e->file))) C(10977, inv, 0x4000, 0, 0);
    }
}

// 1.13c D2Client+0x861A0 (packet in param_1)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_861a0(const D2cItmEnv* e, const uint8_t* p) {
    D2cHdr h; memset(&h, 0, sizeof h);
    C(11105, 0, 0, A(&h), 0);
    uint8_t* u = (uint8_t*)C(F_UNPACK_PLAYER, rd<uint32_t>(p, 9), p[8], 0, 0);
    if (!u) return;
    if (!(rd<uint32_t>(u, 0xC4) & 0x10000) && rd<int32_t>(u, 0) == 0) {
        int32_t m = rd<int32_t>(u, 0x10);
        if (m != 0 && m != 0x11) C(V(0x6fac9830), 7, A(u), 0, 0);
    }
    uint8_t slot = h.slot, to;
    switch (slot) { case 4: case 0xb: to = 5; break; case 5: case 0xc: to = 4; break; default: return; }
    W inv = rd<uint32_t>(u, 0x60);
    W rec = C(11139, inv, 0, 0, 0);
    if (!rec) FATAL(0);
    C(10694, inv, rec, 0, 0);
    C(10921, A(u), to, 0, 0);
    C(F_PRE2, A(u), 0, 0, 0);
    C(10605, A(u), inv, A(&h), slot);
    W old = C(10646, inv, rec, 0, 0);
    if (!old || old != (W)&h) FATAL(0x5f7);
    C(10061, inv, 0, 0, 0);
    if (!C(10202, old, 0x100, 0x96, A(e->file))) C(11127, A(u), old, 0, 0);
    C(10608, inv, to, 0, 0);
    C(F_UI3, 0, 0, 0, 0);
    if (u == e->player) {
        C(F_SELECT, 0, 0, 0, 0);
        if (slot == 4) e->v5c[0] = 0; else if (slot == 5) e->v5c[1] = 0;
        C(V(0x6fb32980), 0, 4, 0, 0);
        C(10058, A(&h), 0, 0, 0);
        C(10466, inv, A(u), 0, 0);
        C(11017, inv, 0, 0, 0);
        C(F_KILL, 0, 0, 0, 0);
    }
    C(F_REFRESH, 0, 0, 0, 0);
    bool bad;
    uint8_t* n = findItem(e, rd<uint32_t>(p, 4), bad);
    if (bad) FATAL(0x5a);
    if (!n) n = (uint8_t*)C(F_MAKE, A(p), A(&h), 0, 0);
    else if (!C(10197, rec, 1, 0, 0)) C(V(0x6fb32980), 0, h.act, 0, 0);
    else {
        C(F_CHAIN, 0x6fbbae08, rd<uint32_t>(p, 4) & 0x7f, rd<uint32_t>(p, 4), 4);
        n = (uint8_t*)C(F_MAKE, A(p), A(&h), 0, 0);
        if (!n) FATAL(0x618);
    }
    C(10058, A(n), slot, 0, 0);
    if (!C(10405, inv, A(n), isEquipSlot(slot) ? 4 : 3)) return;
    if (!C(F_TRY, 0, 0, 0, 0)) return;
    C(10138, inv, rec, 0, 0);
    C(F_POST, 0, 0, 0, 0);
    if (rd<int32_t>(u, 0) < 0 || rd<int32_t>(u, 0) > 1) FATAL(0x641);
    if (p[3] <= 1) C(F_PRE2, A(u), 0, 0, 0);
    C(F_UI3, 0, 0, 0, 0);
    C(F_UI2, 0, 0, 0, 0);
    C(F_UI1, 0, 0, 0, 0);
    tail(e, A(u));
    C(F_DONE, 0, 0, 0, 0);
    C(F_REFRESH, 0, 0, 0, 0);
}

// 1.13c D2Client+0x86810 (packet in param_1)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_86810(const D2cItmEnv* e, const uint8_t* p) {
    uint8_t buf[24];
    C(11105, 0, 0, A(buf), 0);
    uint8_t* pl = e->player;
    uint32_t id = rd<uint32_t>(p, 4);
    bool bad;
    uint8_t* n = findItem(e, id, bad);
    if (bad) FATAL(0);
    if (n) {
        int32_t m = rd<int32_t>(n, 0x10);
        if (m == 0) FATAL(0);
        if (m == 1) {
            if (!pl) FATAL(0);
            W inv = rd<uint32_t>(pl, 0x60);
            C(F_PRE, A(pl), 0, 0, 0);
            C(10694, inv, 0, 0, 0);
            C(10921, A(pl), 0, 0, 0);
            C(F_PRE2, A(pl), 0, 0, 0);
            W kind = C(10417, 0, 0, 0, 0);
            C(10605, A(pl), inv, A(buf), kind);
            W old = C(10646, inv, 0, 0, 0);
            if (old) {
                C(10608, inv, 0, 0, 0);
                C(10061, rd<uint32_t>(pl, 0x60), 0, 0, 0);
                if (!C(10202, old, 0x100, 0x96, A(e->file))) C(11127, A(pl), 0, 0, 0);
            }
            C(F_UNREF, A(pl), 0, 0, 0);
            C(F_UI3, 0, 0, 0, 0);
        }
        C(F_CHAIN, 0x6fbbae08, id & 0x7f, id, 4);
    }
    uint8_t* it = (uint8_t*)C(F_LOOKUP, A(p), 0, 0, 0);
    if (!it) FATAL(0);
    C(F_REFRESH, 0, 0, 0, 0);
}
