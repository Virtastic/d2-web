// 1.13c D2Game+0x4B7F0 CLIENTS_RemoveClientFromGame: drop client `id` from `game`.
// Convention: EAX = client id, stack game, reason; ret 8. The client records keep their retail layout (32-bit words):
// +0 id, +0xA word, +0xD name[16], +0x1D, +0x60, +0x68, +0x6C byte, +0x178, +0x17C buffer, +0x190, +0x1A8 owner (+0x1C pool),
// +0x1B4, +0x1C0 list (next +0x204), +0x4A8 next in the game list, +0x4AC next by id, +0x4B0 next by name.
// Game: +0x28 word, +0x88 client list, +0x8C client count. Unit: +0 type, +4 class, +0xC id.
// The module globals (the two 256-bucket client hashes, the initialised flag, the callback table) come through the env.
#include <cstdint>
#include <cstring>

struct D2Game4B7F0Env {
    uint32_t* byId;            // 0x1107B8: 256 heads, bucket id & 0xFF, chain +0x4AC
    uint32_t* byName;          // 0x1103B8: 256 heads, bucket hash(name) & 0xFF, chain +0x4B0
    const uint32_t* inited;    // 0x111C18
    const uint32_t* callbacks; // 0x111C3C: table pointer, function at +4
    uint32_t (*clientById)(uint32_t game, uint32_t id);                // 0x49FE0 (ECX id, EAX game)
    void     (*resetSlot)(uint32_t slot, uint32_t client);             // 0x4AE90 (EAX client+0x6C, ECX client)
    void     (*onDrop)(uint32_t client, uint32_t game);                // 0x2C910 (stdcall 2)
    void     (*lock)(uint32_t lockRva, uint32_t line);                 // 0xA328 Fog lock (ECX lock, EDX line)
    void     (*unlock)(uint32_t lockRva);                              // LeaveCriticalSection
    uint32_t (*hashName)(const char* name);                            // 0xA334 (stdcall 1)
    int32_t  (*strncmpN)(uint32_t a, const char* b, uint32_t n);       // 0xA286 Storm strncmp (stdcall 3)
    void     (*detach)(uint32_t client, uint32_t game);                // 0x2BED0 (EAX client, stack game)
    uint32_t (*playerUnit)(uint32_t client, uint32_t z);               // 0x4AD10 (ESI client, stack 0)
    int32_t  (*getStat)(uint32_t unit, uint32_t stat, uint32_t layer); // 0xA3E2 (stdcall 3)
    void     (*unitA)(uint32_t unit, uint32_t game);                   // 0x9C840 (EAX unit, stack game)
    void     (*unitB)(uint32_t game, uint32_t unit);                   // 0x97640 (ECX game, EDX unit)
    void     (*unitC)(uint32_t game, uint32_t unit);                   // 0x90560 (EAX game, stack unit)
    void     (*unitD)(uint32_t unit, uint32_t game);                   // 0x34760 (EAX unit, stack game)
    void     (*unitE)(uint32_t game, uint32_t unit);                   // 0x73AA0 (EAX game, EDI unit)
    uint32_t (*unitRoom)(uint32_t unit);                               // 0xA478 (stdcall 1)
    void     (*roomRemove)(uint32_t room, uint32_t type, uint32_t id); // 0xA44E (stdcall 3)
    void     (*ownerRemove)(uint32_t owner, uint32_t unit);            // 0xE20A0 (ECX owner, EDX unit)
    void     (*roomF)(uint32_t room, uint32_t z);                      // 0xB1E6 (stdcall 2)
    void     (*roomG)(uint32_t room, uint32_t z);                      // 0xB1EC (stdcall 2)
    int32_t  (*badCodePtr)(uint32_t fn);                               // IsBadCodePtr
    void     (*callback)(uint32_t fn, uint32_t ecx, uint32_t edx, const uint32_t* w18);
    void     (*clientH)(uint32_t v, uint32_t client);                  // 0x9D550 (EAX client+0x1B4 word, EDI client)
    void     (*memFree)(uint32_t pool, uint32_t p, uint32_t line);     // 0xA2EC Fog free (ECX pool, EDX ptr, file, line, 0)
    void     (*fatal)(uint32_t line);
};

static uint32_t rd(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static void wr(uint32_t a, uint32_t v) { memcpy((void*)(uintptr_t)a, &v, 4); }

void D2Game_4B7F0(const D2Game4B7F0Env* e, uint32_t id, uint32_t game, uint32_t reason) {
    if (!game) { e->fatal(0x489); return; }
    uint32_t c0 = e->clientById(game, id);
    if (c0) e->resetSlot(c0 + 0x6C, c0);
    if (reason == 1) e->onDrop(e->clientById(game, id), game);
    char name[16]; name[0] = 0;
    const uint32_t b = id & 0xFF;
    if (*e->inited) {
        e->lock(0x110BB8, 0x4CF);
        uint32_t n = e->byId[b];
        if (n) {
            if (rd(n) == id) { strcpy(name, (const char*)(uintptr_t)(n + 0xD)); e->byId[b] = rd(n + 0x4AC); }
            else {
                while (rd(rd(n + 0x4AC)) != id) n = rd(n + 0x4AC);
                uint32_t nx = rd(n + 0x4AC);
                if (!nx) { e->fatal(0x4DB); return; }
                strcpy(name, (const char*)(uintptr_t)(nx + 0xD));
                wr(n + 0x4AC, rd(nx + 0x4AC));
            }
        }
        e->unlock(0x110BB8);
    }
    uint32_t c = 0, p = rd(game + 0x88);
    if (p) {
        if (rd(p) == id) { c = p; wr(game + 0x88, rd(p + 0x4A8)); }
        else {
            while (rd(p + 0x4A8) && rd(rd(p + 0x4A8)) != id) p = rd(p + 0x4A8);
            uint32_t nx = rd(p + 0x4A8);
            if (nx) { c = nx; wr(p + 0x4A8, rd(nx + 0x4A8)); }
        }
    }
    if (*e->inited) {
        const uint32_t h = e->hashName(name) & 0xFF;
        e->lock(0x110BD0, 0x4FE);
        uint32_t n = e->byName[h];
        if (n) {
            if (e->strncmpN(n + 0xD, name, 0x10) == 0) e->byName[h] = rd(n + 0x4B0);
            else {
                uint32_t nx = rd(n + 0x4B0);
                if (!nx) { e->fatal(0x512); return; }
                if (e->strncmpN(nx + 0xD, name, 0x10) != 0)
                    for (;;) { n = rd(n + 0x4B0); nx = rd(n + 0x4B0); if (!nx || e->strncmpN(nx + 0xD, name, 0x10) == 0) break; }
                nx = rd(n + 0x4B0);
                if (nx) wr(n + 0x4B0, rd(nx + 0x4B0));
            }
        }
        e->unlock(0x110BD0);
    }
    if (!c) return;
    e->detach(c, game);
    const uint32_t unit = e->playerUnit(c, 0);
    const uint32_t edx = rd(game + 0x28) & 0xFFFF;
    if (unit) {
        const int32_t exp = e->getStat(unit, 0xD, 0);
        const uint32_t cls = rd(unit + 4);
        const int32_t lvl = e->getStat(unit, 0xC, 0);
        e->unitA(unit, game);
        e->unitB(game, unit);
        e->unitC(game, unit);
        e->unitD(unit, game);
        e->unitE(game, unit);
        const uint32_t room = e->unitRoom(unit);
        if (room) e->roomRemove(room, rd(unit), rd(unit + 0xC));
        e->ownerRemove(rd(c + 0x1A8), unit);
        e->roomF(room, 0);
        e->roomG(room, 0);
        if (const uint32_t t = *e->callbacks) {
            if (e->badCodePtr(rd(t + 4))) { e->fatal(0x592); return; }
            if (const uint32_t fn = rd(*e->callbacks + 4)) {
                const uint32_t w[18] = { cls, (uint32_t)lvl, (uint32_t)exp, (uint32_t)(exp >> 31), rd(c + 0xA) & 0xFFFF, c + 0xD, c + 0x6C,
                                         rd(c + 0x178), 0, 0, c + 0x1D, rd(c + 0x60), 0, 0, 0, 0, 0, c + 0x190 };
                e->callback(fn, c + 0x68, edx, w);
            }
        }
    } else if (const uint32_t t = *e->callbacks) {
        *(uint8_t*)(uintptr_t)(c + 0x6C) = 0;
        if (e->badCodePtr(rd(t + 4))) { e->fatal(0x5BA); return; }
        if (const uint32_t fn = rd(*e->callbacks + 4)) {
            const uint32_t w[18] = { 0, 0, 0, 0, 0, c + 0xD, c + 0x6C, rd(c + 0x178), 0, 0, c + 0x1D, rd(c + 0x60), 0, 0, 0, 0, 0, c + 0x190 };
            e->callback(fn, c + 0x68, edx, w);
        }
    }
    if (const uint32_t v = rd(c + 0x1B4)) e->clientH(v, c);
    if (const uint32_t buf = rd(c + 0x17C)) { e->memFree(rd(rd(c + 0x1A8) + 0x1C), buf, 0x5E1); wr(c + 0x17C, 0); }
    while (const uint32_t n = rd(c + 0x1C0)) { wr(c + 0x1C0, rd(n + 0x204)); e->memFree(0, n, 0x5F1); }
    e->memFree(rd(rd(c + 0x1A8) + 0x1C), c, 0x5F4);
    wr(game + 0x8C, rd(game + 0x8C) - 1);
}
