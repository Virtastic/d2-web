// Stage A implementations from the cached 1.13c D2Game function bodies.
// Prior art: D2MOO PlrSave/ItemMode call sites. Callees whose 1.13c ordinal
// meaning is not pinned are passed in as callbacks (same style as
// d2game_batch_d1f60_54720.cpp); register inputs are explicit parameters.
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <unistd.h>

// 1.13c D2Game+0xAF610 -- true if any of the 16 bytes at (data+4) is nonzero.
// unit: int* with [0]==1 type tag, [5] = data pointer.
extern "C" uint32_t D2Game_AF610_HasComponents(const int32_t* unit)
{
    const char* p = nullptr;
    if (unit && unit[0] == 1 && unit[5]) p = (const char*)(uintptr_t)unit[5] + 4;
    if (!p) return 0; // retail would fault on null here; callers guard
    for (int i = 0; i < 0x10; ++i)
        if (p[i]) return 1;
    return 0;
}

namespace {
struct ItemOps {
    int32_t* (*create)();                              // 0x54720
    int32_t (*getKind)(int32_t*);                      // Ordinal_10417
    int32_t (*canPlace)(int32_t, int32_t*, int32_t);   // Ordinal_11041
    int32_t (*place)(int32_t, int32_t*, int32_t);      // Ordinal_10405
    void (*setMode)(int32_t*, int32_t);                // 11090
    void (*setType)(int32_t*, int32_t);                // 10058
    void (*setSlot)(int32_t*, int32_t);                // 10012
    void (*setFlag)(int32_t*, int32_t, int32_t);       // 10892
    void (*attach)(int32_t, int32_t*);                 // 10050
    void (*touch)(int32_t*, int32_t);                  // 10201
    void (*refresh)();                                 // 0x345A0
    int32_t (*fromBuf)(int32_t*, uint32_t);            // 0xD76A0
    int32_t (*fromInv)(int32_t*, uint32_t, void*);     // 0xD4A70
    void (*setA)(int32_t*, uint32_t);                  // Ordinal_10882 (rec+0x18)
    void (*setB)(int32_t*, uint32_t);                  // Ordinal_11118 (rec+0x1c)
    void (*prep1130)(int32_t*);                        // 0x1130
    void (*prep1100)(int32_t*);                        // 0x1100
};
}

// 1.13c D2Game+0x54A20
extern "C" uint32_t D2Game_54A20_PlaceCorpseItemByMode(const ItemOps* o, const uint8_t* rec,
        int32_t* game, uint32_t* outFlag, int32_t* ref, uint32_t* loadedId)
{
    int32_t* it = o->create();
    if (!it) return 0xd;
    int32_t kind = it[4];
    if (kind == 0) {
        o->setMode(it, 4);
        o->setA(it, *(const uint32_t*)(rec + 0x18));   // 1.13c: Ordinal_10882 / 11118, then 0x1130, 0x1100
        o->setB(it, *(const uint32_t*)(rec + 0x1c));
        o->prep1130(it); o->prep1100(it);
        if (!o->fromBuf(game, (uint32_t)it[3])) return 0xd;
    } else if (kind == 1) {
        int32_t owner = game[0x18];
        int32_t c = o->getKind(it);
        if (!(c && o->canPlace(owner, it, c))) return 0xd;
        if (!o->place(owner, it, (c == 0xb || c == 0xc) ? 4 : 3)) return 0xd;
        o->setType(it, c);
        it[0x31] &= ~2;
        o->setMode(it, 1);
        o->setSlot(it, 0xff);
        o->setFlag(it, 8, 1);
        it[0x31] &= ~0x2000000;
        o->attach(owner, it);
        o->touch(game, 1);
        o->refresh();
    } else if (kind == 6 && *ref) {
        o->setMode(it, 4);
        if (!o->fromInv(game, *ref ? *(uint32_t*)(uintptr_t)(*ref + 0xc) : 0xffffffffu, nullptr)) return 0xd;
    } else return 0xd;
    if (rec[1]) *game = (int32_t)(uintptr_t)it; // retail stores the item into *param_2
    *outFlag = rec[1];
    return 0;
}

namespace {
struct PickupOps {
    int32_t* (*create)();                                 // 0x54720
    void (*setMode)(int32_t*, int32_t);                   // 11090
    int32_t (*fromBuf)(void*, int32_t*, uint32_t);        // 0xD76A0
    int32_t (*fromHandle)(void*, int32_t*, int32_t, int32_t, void*); // 0xD60D0
    int32_t (*fromId)(void*, int32_t*, int32_t, int32_t, void*);     // 0xD4E80
    int32_t (*fromUnit)(void*, int32_t*, void*);          // 0xD1650
    int32_t (*fromInv)(void*, int32_t*, uint32_t, void*); // 0xD4A70
    void (*setA)(int32_t*, uint32_t);                     // Ordinal_10882
    void (*setB)(int32_t*, uint32_t);                     // Ordinal_11118
    void (*prep1130)(int32_t*);                           // 0x1130
    void (*prep1100)(int32_t*);                           // 0x1100
};
}

// 1.13c D2Game+0x54BC0
extern "C" uint32_t D2Game_54BC0_DropPickupMercItem(const PickupOps* o, void* ctx, int32_t* game,
        const uint8_t* rec, uint32_t* out)
{
    if (out) *out = 0;
    int32_t* it = o->create();
    if (!it) return 0xc;
    int32_t ok;
    switch (it[4]) {
    case 0: // 1.13c also calls Ordinal_10114(it, ..., "PlrSave.cpp", 0xe2) here (stack args unpinned: capture request)
        o->setMode(it, 4);
        // 10882/11118 args are stack garbage in the decompile: unpinned, capture request
        { const uint32_t id = (uint32_t)it[3]; o->prep1130(it); o->prep1100(it); ok = o->fromBuf(ctx, game, id); }
        break;
    case 1: o->setMode(it, 4); ok = o->fromHandle(game, it, *rec, 1, nullptr); break;
    case 2: o->setMode(it, 4);
        o->setA(it, *(const uint32_t*)(rec + 0x18)); o->setB(it, *(const uint32_t*)(rec + 0x1c));
        ok = o->fromId(game, it, *(int32_t*)(rec + 0x18), 0, nullptr); break;
    case 4: o->setMode(it, 4); ok = o->fromUnit(game, it, nullptr); break;
    case 6:
        o->setMode(it, 4);
        if (!*(int32_t*)it) return 0xc;
        ok = o->fromInv(ctx, game, *(uint32_t*)(uintptr_t)(*(int32_t*)it + 0xc), nullptr);
        if (!ok) return 0xc;
        break;
    default: return 0xc;
    }
    if (!ok) return 0xc;
    if (rec[1]) *(int32_t*)it = (int32_t)(uintptr_t)it;
    if (out) *out = rec[1];
    return 0;
}

// Retail 0x719A0 (disassembled): EAX = game, ESI = unit a, stack = (unit b, stateA, stateB, retVal), ret 0x10. EBP/EBX are not inputs.
// Every callee is an op: 10201 touch(u, 1); 0x716D0 reset(game, u); 0x700C0 post(game = EBX, u = EAX); 0xD4570 notify(u = EAX);
// 10920 client(u) returns the record whose +0x9C is the client (only when *u == 0, else the client is 0); 0x8A3E0 send(client = EAX, buf, len);
// 0x8E6F0 killRoom(node = EAX); 0x8EA80 event(game = ECX, u = EDX, 0xd, frame + 1, 0, 0, 0).
namespace {
struct DuelOps {
    void (*touch)(int32_t*, int32_t);          // 10201
    void (*reset)(void* game, int32_t* u);     // 0x716D0
    void (*post)(void* game, int32_t* u);      // 0x700C0
    void (*notify)(int32_t*);                  // 0xD4570
    uint8_t* (*client)(int32_t*);              // 10920 (0xA4C0): record, +0x9C = client
    void (*send)(uint32_t client, const uint8_t* buf, uint32_t len); // 0x8A3E0
    void (*killRoom)(uint8_t* node);           // 0x8E6F0
    void (*event)(void* game, int32_t* u, int, int, int, int, int); // 0x8EA80
};
}

// 1.13c D2Game+0x719A0 -- end of a duel: flag both units, reset state, post events.
extern "C" void D2Game_719A0_EndDuel(const DuelOps* o, uint8_t* game, int32_t* a, int32_t* b,
        int32_t* stateA, int32_t* stateB, uint32_t retVal)
{
    for (int32_t* u : {a, b}) o->touch(u, 1);
    for (int32_t* u : {a, b}) if (u) u[0x32] |= 0x800000;
    for (int32_t* u : {a, b}) { u[0x19] = -1; u[0x1a] = 6; *(uint8_t*)(u + 0x1b) = 0; }
    for (int32_t* u : {a, b}) o->reset(game, u);
    for (int32_t* u : {a, b}) o->post(game, u);
    stateA[0x14] = (int32_t)retVal;
    stateB[0x14] = 9;
    for (int32_t* u : {a, b}) o->notify(u);
    for (int32_t* u : {a, b}) {
        uint32_t client = 0;
        if (*u == 0) client = *(uint32_t*)(o->client(u) + 0x9c);
        const uint8_t pkt[2] = {0x77, 0x0c};
        o->send(client, pkt, 2);
    }
    for (int32_t* u : {a, b})
        for (uint8_t* p = (uint8_t*)(uintptr_t)u[0x37]; p;) {
            uint8_t c = *p; uint8_t* node = p; p = *(uint8_t**)(p + 0x24);
            if (c == 0x0d) o->killRoom(node);
        }
    for (int32_t* u : {a, b})
        o->event(game, u, 0xd, *(int32_t*)(game + 0xa8) + 1, 0, 0, 0);
}
