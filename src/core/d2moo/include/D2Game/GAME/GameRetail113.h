#pragma once
// Retail-layout (1.13c) seam-model routines of D2Game.dll: client/room/packet functions written over opaque 32-bit retail handles and a callee seam
// (`call(id, words)`), so a proof binds the callees to a recorded or scripted stream. One body per marked function; moved here from src/core/d2game.
#include <cstdint>
#include <cstring>
#include <D2Game/GAME/Game.h>

// 1.13c D2Game+0x4B500 (stdcall client, game, x, y; EAX = room or 0, ESI = player unit): move the player into a room, send the room/position updates,
// re-attach the inventory items to the game and finish the iron-golem hand-over. Every callee is a seam call `call(id, words)` (ids and word layouts:);
// handles are opaque 32-bit codes, the game's unit hash lists are walked through bucket/nodeId/nodeNext.
struct D2G4B500Seam
{
    uint32_t (*call)(uint32_t id, const uint32_t* w, uint32_t* out0, uint32_t* out1);
    uint32_t (*act)(uint32_t nAct);                  // game->pAct[nAct]
    uint32_t (*bucket)(int list, uint32_t b);        // game->pUnitList[list][b]
    uint32_t (*nodeId)(uint32_t node);               // node +0xC
    uint32_t (*nodeNext)(uint32_t node);             // node +0xE4
};
struct D2G4B500Client { uint8_t nAct; uint32_t dwFlags; uint32_t nIronGolemItemGUID; };   // client +0x1AC, +0x3D4, +0x484
struct D2G4B500Unit { uint32_t type, id, inventory; };                                  // unit +0, +0xC, +0x60

// 1.13c D2GAME+0x4B500
static void GAME_4B500(const D2G4B500Seam& s, D2G4B500Client* cl, uint32_t hClient, uint32_t hGame, uint32_t hUnit, const D2G4B500Unit& u, uint32_t room, uint32_t x, uint32_t y)
{
    if (!hGame || !hUnit || !hClient) return;
    auto C = [&](uint32_t id, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0, uint32_t e = 0, uint32_t f = 0, uint32_t g = 0, uint32_t* o0 = nullptr, uint32_t* o1 = nullptr) {
        const uint32_t w[7] = { a, b, c, d, e, f, g }; return s.call(id, w, o0, o1);
    };
    auto find = [&](int list, uint32_t guid) { for (uint32_t n = s.bucket(list, guid & 0x7F); n; n = s.nodeNext(n)) if (s.nodeId(n) == guid) return n; return 0u; };
    const uint32_t nAct = cl->nAct;                  // CLIENTS_GetActNo (0x49DB0)
    const uint32_t pAct = s.act(nAct);
    if (!room) {
        const uint32_t r13 = C(1, pAct), r89 = C(2, hUnit);
        room = C(3, pAct, r13, 0, r89, 0, 0, 0, &x, &y);
        uint32_t c10 = 0, c14 = 0;
        C(4, room, 0, 0, 0, 0, 0, 0, &c10, &c14);
        const uint32_t level = C(5, room) & 0xFF;
        C(6, c10, c14, hClient, level);
    }
    C(7, hClient, room);
    C(8, room, hUnit, y, x, hGame, 0);
    cl->dwFlags &= 0xFFFFFFFEu;
    C(9, 0x15, u.type, u.id, hClient, x, y, 1);
    C(10, hClient, 0x7E, 5);
    for (uint32_t item = C(11, u.inventory); item;) {
        const uint32_t guid = C(12, item), t = C(13, item);
        if (!t) {
            const uint32_t w = C(14, item);
            const uint32_t next = C(15, item);
            C(16, u.inventory, guid, 0);
            if (guid && (w & 0xFFFF)) C(17, hGame, hUnit, w, guid, 0xFFFF, 0, 0);
            item = next;
        } else {
            if (const uint32_t node = find(0, guid)) {
                const uint32_t r85 = C(18, nAct & 0xFF), r89 = C(2, node);
                uint32_t ox = 0, oy = 0;
                const uint32_t room2 = C(3, s.act(nAct), r85, 0, r89, 0, 0, 0, &ox, &oy);
                C(19, room2, ox, oy, hGame, guid);
            }
            item = C(15, item);
        }
    }
    C(20, hUnit, hGame);
    const uint32_t g = cl->nIronGolemItemGUID;
    if (g && g != 0xFFFFFFFFu) {
        if (const uint32_t node = find(3, g)) {
            if (const uint32_t v = C(21, hUnit, 0x5A)) {
                const uint32_t res = C(22, hUnit, v, 1);
                const uint32_t a = C(23, hUnit); C(24, node, a);
                const uint32_t b = C(25, hUnit); C(26, node, b);
                C(27, hUnit, node);
                C(28, hUnit, hGame, 0x5A, res, 1, 0, 0);
            }
        }
        cl->nIronGolemItemGUID = 0xFFFFFFFFu;
    }
}

// 1.13c D2Game+0x4B1D0 (ESI = client, stack = new room) and +0x4B3C0 (EAX = client, EDI = room): move a client's room, diffing the old and new rooms' neighbour lists
// (new-only rooms get 9DC10, old-only rooms get 9D980 plus the 0x174 unit hand-over 788B0), then re-flag the unit. Callees are seam calls `call(id, w)` (ids:);
// handles are opaque 32-bit values and the neighbour lists come through list(room, buf) (A55C: up to 4 rooms).
struct D2G4B1D0Seam { uint32_t (*call)(uint32_t id, const uint32_t* w); uint32_t (*list)(uint32_t room, uint32_t* buf); };
struct D2G4B1D0Client { uint32_t game, room, unit, v16c, v170, flags; };   // client +0x1A8, +0x1B4, +0x174, +0x16C, +0x170, +0x3D4
static void GAME_4B1D0(const D2G4B1D0Seam& s, D2G4B1D0Client* cl, uint32_t hClient, uint32_t nw)
{
    if (!hClient || !cl->game) return;
    const uint32_t od = cl->room;
    if (od == nw) return;
    auto C = [&](uint32_t id, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0) { const uint32_t w[7] = { a, b, c, d, 0, 0, 0 }; return s.call(id, w); };
    C(1, od, nw); C(2, od, nw);
    uint32_t A[4], B[4], nA = 0, nB = 0;
    if (nw) { C(3, nw); nA = s.list(nw, A); }
    if (od) { C(3, od); nB = s.list(od, B); }
    auto has = [](const uint32_t* v, uint32_t n, uint32_t x) { for (uint32_t i = 0; i < n; i++) if (v[i] == x) return true; return false; };
    for (uint32_t i = 0; i < nA; i++) if (!has(B, nB, A[i])) C(4, A[i], hClient, cl->game);
    for (uint32_t j = 0; j < nB; j++) {
        if (has(A, nA, B[j])) continue;
        C(5, B[j], cl->game, hClient);
        if (od != B[j]) continue;
        uint32_t u;
        if (cl->flags & 1) u = cl->unit;
        else if (!cl->unit) u = 0;
        else { u = C(6, cl->game, cl->v170, cl->v16c); if (!u) cl->unit = 0; }
        C(7, cl->game, u, hClient, 0);
    }
    cl->room = nw;
    if (nw && cl->unit) { const uint32_t r = C(8, nw), a = C(9, cl->unit), b = C(10, r); C(11, cl->unit, a | b); }
}
static void GAME_4B3C0(const D2G4B1D0Seam& s, D2G4B1D0Client* cl, uint32_t hClient, uint32_t room)
{
    if (!cl) { const uint32_t w[7] = { 0x78B }, v = s.call(2, w), x[7] = { 0xF9FF9, v }, y[7] = { 0xFFFFFFFFu }; s.call(3, x); s.call(4, y); return; }   // assert(client) -> _exit(-1)
    if (room != cl->room) { const uint32_t w[7] = { hClient, room }; s.call(1, w); }   // 4B1D0 itself is a seam call here (the capture stubbed it)
    cl->room = room;
}

// 1.13c D2Game+0x4B400 (EAX = client, stack = room): re-home a client onto `room`. hClient/unit handles are opaque.
// cl->room = client +0x1B4, *cnt = client +0x1B0; flagBit = bit 21 of unit +0xC8.
static void GAME_4B400(const D2G4B1D0Seam& s, D2G4B1D0Client* cl, uint32_t* cnt, uint32_t hClient, uint32_t arg, uint32_t flagBit)
{
    if (!arg || !hClient) return;
    const uint32_t od = cl->room;
    auto C = [&](uint32_t id, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0, uint32_t d = 0) { const uint32_t w[7] = { a, b, c, d, 0, 0, 0 }; return s.call(id, w); };
    const uint32_t u = C(1, hClient, 0);
    if (!u) return;
    const uint32_t a = C(2, u);
    C(3, od, hClient, od); C(4, od, hClient, arg); C(5, u, u, 0xCC2F0);
    if (flagBit) C(7, arg, u, 1);
    ++*cnt;
    if (od != a) {
        const uint32_t x = C(8, od), y = C(8, a);
        if (x != y) { C(10, arg, x, y, u); C(11, x, arg, u, y); }
        C(12, hClient, a);
    }
    C(13, hClient, arg);
    if (C(2, u)) C(14, u);
}

// 1.13c D2Game+0x4BCC0 (EAX = client, stdcall game a2 a3 a4 a5): add a player to a game (load the unit via 56430, validate the client/game mode flags, set up the unit and its skill hotkeys,
// notify). id 0 (unlogged) fetches the unit 56430 produced. Handles: client 0xA2, game 0xA1, unit 0xB1 (0 = none).
struct D2G4BCC0St { uint32_t cflags, f3d4, c174, uflags, g70, nSk, utype, uid, w70, w74, w78, w7C, slotA[16], slotD[16]; };   // client +0xA/+0x3D4/+0x174, unit +0xC8, game +0x70
static uint32_t GAME_4BCC0(const D2G4B1D0Seam& s, D2G4BCC0St& c, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
    auto C = [&](uint32_t id, uint32_t a = 0, uint32_t b = 0, uint32_t cc = 0, uint32_t d = 0, uint32_t e = 0, uint32_t f = 0, uint32_t g = 0) { const uint32_t w[7] = { a, b, cc, d, e, f, g }; return s.call(id, w); };
    const uint32_t G = 0xA1, CL = 0xA2;
    c.f3d4 |= 1;
    const uint32_t err = C(1, a4, a5, G, 0xD, a2, a3, CL);
    const uint32_t u = C(0);
    auto fail = [&](uint32_t v) { c.c174 = 0; return v; };
    if (err) { const uint32_t m = C(2, CL); C(3, 0xF9FF9, 0xFC228, m, err, 0xFC270); return fail(err); }
    if (c.cflags & 0x20) { if (!c.g70) return fail(0x18); } else if (c.g70) return fail(0x17);
    if (c.cflags & 4) { if (c.cflags & 8) return fail(0x15); if (!(C(4, G) & 0x800)) return fail(0x14); } else if (C(4, G) & 0x800) return fail(0x13);
    C(5, u, CL); const uint32_t m = C(6, u); C(7, m, 0x5F, CL); C(8, u, u, 0xCC2F0);
    if (u && (c.uflags & 1)) { C(9, 0, u, CL); c.uflags |= 0x200000; }
    for (uint32_t i = 0; i < 16; i++) { const int16_t sk = (int16_t)c.slotA[i]; if (sk >= 0 && sk < (int32_t)c.nSk) C(10, i, c.slotD[i], CL, (c.slotA[i] >> 16) & 0xFF); }
    if (C(11, u)) {
        C(12, u ? c.utype : 6, u ? c.uid : 0xFFFFFFFFu, CL, 1, c.w74 & 0xFFFF, c.w7C);
        C(12, u ? c.utype : 6, u ? c.uid : 0xFFFFFFFFu, CL, 0, c.w70 & 0xFFFF, c.w78);
    }
    C(8, u, u, 0xCC2F0); C(15, u, u, 1); C(16, u, G);
    if (c.g70) C(17, u, G);
    return 0;
}

// 1.13c D2Game+0x8A3E0 (EAX = client, stdcall src, count): append one outgoing packet to the client's queue of 0x208-byte blocks (head +0x1B8, tail +0x1BC, free list +0x1C0).
// New block: the free-list head if any, else the pool allocator (SCmd.cpp line 0xA0, via `alloc`); zeroed, linked behind the tail. A packet over 0x200 is not split (caller contract).
struct D2G8A3E0Blk { uint32_t used; uint8_t data[0x200]; D2G8A3E0Blk* next; };
struct D2G8A3E0Client { D2G8A3E0Blk *head, *tail, *freeList; };
template <class Alloc> static void GAME_8A3E0(D2G8A3E0Client* c, const void* src, uint32_t n, Alloc alloc)
{
    if (!c) return;
    uint32_t used = c->tail ? c->tail->used : 0;
    D2G8A3E0Blk* b = c->tail;
    if (!b || used + n > 0x200) {
        used = 0;
        b = c->freeList;
        if (b) c->freeList = b->next; else b = alloc();
        memset(b, 0, sizeof *b);
        if (c->tail) c->tail->next = b; else c->head = b;
        c->tail = b;
    }
    memcpy(b->data + used, src, n);
    b->used += n;
}

// 1.13c D2Game+0x8C380
// Small server->client packet builders; each fills a stack buffer and hands it to 0x8A3E0 (`send(client, buf, n)`).
// 8C380 (AL = type, ECX = value, stack client): {type, value LE32}, 5 bytes. 4AB70 (EAX = unit or null, stack client): {0x0B, unit type low byte, unit id LE32}, 6 bytes; null unit sends type 6, id -1.
// 8A980 (EAX = word, CL = byte, EDX = dword, stack client, flag): {0x7B, byte, (word & 0xFFF | 0x8000 if flag) LE16, dword LE32}, 8 bytes.
template <class Send> static void GAME_8C380(uint8_t type, uint32_t value, void* client, Send send)
{
    uint8_t b[5]; b[0] = type; memcpy(b + 1, &value, 4); send(client, b, 5);
}
// 1.13c D2GAME+0x4AB70
template <class Send> static void GAME_4AB70(const D2UnitStrc* unit, void* client, Send send)
{
    uint32_t type = unit ? unit->dwUnitType : 6, id = unit ? unit->dwUnitId : 0xFFFFFFFFu;
    uint8_t b[6]; b[0] = 0x0B; b[1] = (uint8_t)type; memcpy(b + 2, &id, 4); send(client, b, 6);
}
// 1.13c D2Game+0x8A980
template <class Send> static void GAME_8A980(uint32_t word, uint8_t byte, uint32_t dword, void* client, int flag, Send send)
{
    uint16_t w = (uint16_t)((word & 0xFFF) | (flag ? 0x8000 : 0));
    uint8_t b[8]; b[0] = 0x7B; b[1] = byte; memcpy(b + 2, &w, 2); memcpy(b + 4, &dword, 4); send(client, b, 8);
}

// 1.13c D2Game+0x2EEA0 (EBX = unit, stack game + one unused word, ret 8): skill-use gate. B2D6(unit) != 0 -> 0; skill = B204(unit), 0 -> 0; B2E2(unit) != 0 -> 0; skill >= cnt (signed, cnt = [[0x6FD1829C]] + 0xC5C) -> 0;
// A808(unit) != 0 -> 0. this = game skill array entry: when this.+8 == 0x7FFFFFFF it becomes B18C(game[0xBC + 4 * this.byte0], skill); this.+4++; when the second stack word's +0x210 is non-zero,
// 2EAD0(this, game, unit, word) == 1 -> 0; else 1. Seam call ids: 1 B2D6 {unit}, 2 B204 {unit}, 3 B2E2 {unit}, 4 A808 {unit}, 5 B18C {game array word, skill}, 6 2EAD0 {thisCode, 0xA1, unit, word}.
struct D2G2EEA0Seam { uint32_t (*call)(uint32_t id, const uint32_t* w); };
static uint32_t GAME_2EEA0(const D2G2EEA0Seam& s, uint32_t unit, uint32_t thisB, uint32_t& thisCnt, uint32_t& thisV8, uint32_t gArr, uint32_t g210, uint32_t cnt, uint32_t skillBase)
{
    (void)thisB; (void)skillBase;
    const uint32_t u[7] = { unit };
    if (s.call(1, u)) return 0;
    const uint32_t skill = s.call(2, u);
    if (!skill) return 0;
    if (s.call(3, u)) return 0;
    if ((int32_t)skill >= (int32_t)cnt) return 0;
    if (s.call(4, u)) return 0;
    if (thisV8 == 0x7FFFFFFFu) { const uint32_t w[7] = { gArr, skill }; thisV8 = s.call(5, w); }
    thisCnt++;
    if (g210) { const uint32_t w[7] = { 0xA3 + skill % 3, 0xA1, unit, g210 }; if (s.call(6, w) == 1) return 0; }
    return 1;
}
