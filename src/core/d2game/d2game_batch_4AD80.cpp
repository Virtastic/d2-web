// src/core/d2game/d2game_batch_4AD80.cpp -- eleven 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): the per-client unit-update
// list builder and its 0x16 packet sender, the quest/missile/skill helpers and the four small
// callees reached from the unit-update caller. Base 0x6FC20000.
//
// Retail module state is modelled as opaque host records read by their retail byte offsets; the
// client record's +0x1B8 is the outgoing packet block (length word first, payload at +4), +0x1B4 the
// room/unit array handle and +0x1CC the 9-byte unit-update entries built by the 0x4AD80 path.
// Register-only arguments (Ghidra's in_EAX/unaff_EBX/unaff_ESI) are explicit parameters and are
// named at each function. Unmapped callees are seams on D2GameBatch14Env in retail argument order.
// The fatal path is Fog Ordinal 10265(code) -> Ordinal 10024(text) -> _exit(-1); only the exit is
// observable and is modelled.
// Prior art: D2MOO 1.10f for CLIENTS_GetPlayerFromClient (0x4AD10) and the packet 0x16 layout.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <emscripten.h>
#include <Windows.h>      // CRITICAL_SECTION no-ops, deterministic GetTickCount
#include <D2BasicTypes.h> // BOOL, needed by the compat Server.h below
#include <Server.h>       // Sleep no-op

static inline uint8_t rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr8(void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees the cached decompiles name but src/ does not implement.
struct D2GameBatch14Env {
    // D2Common #10383 (1.13c ordinal): the unit array behind the room handle at client+0x1B4.
    // Retail passes (handle, void** out, uint32_t* count); the out pointer is zero when the handle
    // has no rooms.
    uint32_t (*getRoomUnitArray)(uint32_t roomHandle, void** out, uint32_t* count);
    // 0x4AD10 CLIENTS_GetPlayerFromClient(client, ignoreDeath); the client is passed in ESI.
    void*    (*getPlayerFromClient)(void* client, int32_t ignoreDeath);
    // 0x21100 prepares the client unit-update list (client in EAX).
    void     (*prepareUnitUpdate)(void* client);
    // 0x21130 initialises the 0x38-node sort list (buffer in ESI) and returns the client's
    // coordinate key for the distance comparison.
    uint32_t (*getUnitUpdateKey)(void* client, void* nodes);
    // 0x4A460 inserts one unit into the sorted node list: param_1 = unit, param_2 = key,
    // param_3 = best distance so far, param_4 = client, param_5 = max count; the node array
    // (12 bytes per node: unit, distance, next index) arrives in ESI.
    void     (*insertSortedUnit)(void* unit, uint32_t key, uint32_t* best, void* client,
                                 uint32_t maxCount, uint32_t* nodes);
    // 0x4A370 writes the sorted list into client+0x1CC (9 bytes per entry); client in ESI,
    // node array in the argument.
    void     (*flushUnitUpdateList)(uint32_t* nodes, void* client);
    // D2Common #10920 (1.13c ordinal): the unit's inventory/data block; the unit arrives in EAX.
    void*    (*getUnitInventory)(void* unit);
    // D2Common #10608 = D2ItemSetPage(inventory, page).
    void     (*itemSetPage)(uint32_t inventory, uint32_t page);
    // 0xAE6F0 item-chain callback; the unit arrives in EAX.
    void     (*itemChainCallback)(void* unit);
    // 0x74810 (game, unit, 0x60, 0x2000, 0, out).
    void     (*saveItems)(void* game, void* unit, uint32_t a3, uint32_t a4, uint32_t a5, void* out);
    // D2Common #10973 (1.13c ordinal): the pending save/consumable result.
    int32_t  (*getSaveResult)(void* unit);
    // 0x21670 adjusts a stat/attribute by the signed argument.
    void     (*adjustStat)(int32_t value);
    // 0x8E680 supplies an event-timer node for the unit (unit in EAX); the flags word at +2 is
    // OR-ed by the callers.
    void*    (*allocEventTimer)(void* unit);
    // D2Common Ordinal 11103 on a unit node (0x977E0's branch).
    void     (*ordinal11103)(void* node);
    // 0x15D0 (0, 0xF): the branch selector in 0x977E0.
    uint32_t (*sub15d0)(uint32_t a, uint32_t b);
    // 0xE0A20(game): the value stored in the 0x977E0 callback block.
    uint32_t (*sube0a20)(void* game);
    // 0x75E10(game, block): consumes the 6-word callback block.
    void     (*sub75e10)(void* game, void* block);
    // 0xDEEE0(game, node, 0, 0): the else branch of 0x977E0.
    void     (*subdeee0)(void* game, void* node, void* a3, uint32_t a4);
    // 0xDFC10(arg): walks every unit of the game (game in EDI) and invokes the callback held in
    // EBX on each unit whose D2Common Ordinal 10494(unit,7) is zero; the argument is passed
    // through. The callback is a caller-supplied register argument, so this is opaque here.
    void     (*subdfc10)(void* arg);
    // 0x8C570: sends packet 0x8D for the unit (unit in EAX, game in ESI).
    void     (*sub8c570)(void* unit, void* game);
    // D2Common Ordinal 10046(file, line, flag): retail's non-fatal party/file warning.
    void     (*displayWarning)(const char* file, uint32_t line, uint32_t flag);
};

// 1.13c D2Game+0x4AD80
// CLIENTS_BuildUnitsUpdateList (client in EBX): the room handle at client+0x1B4 is required (fatal
// 0x6F7 when null) and D2Common #10383 must return a unit array (fatal 0x6F9 when null). The client's
// player is fetched with 0x4AD10; when absent the function returns without touching the list. The
// list is prepared by 0x21100, keyed by 0x21130, the count at client+0x3BC reset to zero, and every
// unit on each room entry's +0x74 chain (next at +0xE8) is inserted by 0x4A460 against the caller's
// max count. 0x4A370 then flushes the sorted list into client+0x1CC.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_4ad80(void* client, uint32_t maxCount, const D2GameBatch14Env* env)
{
    if (rd32(client, 0x1B4) == 0) d2game_fatal(0x6F7);

    void* rooms = nullptr;
    uint32_t nRooms = 0;
    env->getRoomUnitArray(rd32(client, 0x1B4), &rooms, &nRooms);
    if (rooms == nullptr) d2game_fatal(0x6F9);

    void* player = env->getPlayerFromClient(client, 0);
    if (player == nullptr) return;

    env->prepareUnitUpdate(player);                                   // 0x21100
    uint32_t nodes[0x38 * 3];
    std::memset(nodes, 0, sizeof nodes);
    const uint32_t key = env->getUnitUpdateKey(player, nodes);        // 0x21130
    uint32_t best = 0;
    wr32(client, 0x3BC, 0);

    for (uint32_t i = 0; i < nRooms; ++i) {
        const uint32_t entry = rd32(rooms, i * 4);
        for (void* unit = rdptr((const void*)(uintptr_t)entry, 0x74); unit != nullptr;
             unit = rdptr(unit, 0xE8)) {
            env->insertSortedUnit(unit, key, &best, client, maxCount, nodes); // 0x4A460
        }
    }
    env->flushUnitUpdateList(nodes, client);                          // 0x4A370
}

// 1.13c D2Game+0x8A2A0
// D2GAME_PACKETS_SendPacket0x16_UnitsUpdate (client in EDX; retail's ECX argument is unused): when
// client+0x3BC holds a non-zero entry count, append packet 0x16 to the block at client+0x1B8. The
// payload is: id 0x16, uint16 size = count*9 + 0x0D, byte count, then the count 9-byte entries from
// client+0x1CC; the block's leading length word advances by the low 16 bits of that size.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_8a2a0(void* client)
{
    const uint32_t count = rd32(client, 0x3BC);
    if (count == 0) return;

    uint8_t* block = (uint8_t*)rdptr(client, 0x1B8);
    uint8_t* p = block + rd32(block, 0) + 4;
    const uint32_t size = count * 9 + 0x0D;
    p[0] = 0x16;
    wr16(p, 1, (uint16_t)size);
    p[3] = (uint8_t)count;

    const uint8_t* src = (const uint8_t*)client + 0x1CC;
    uint8_t* dst = p + 4;
    for (uint32_t i = 0; i < count; ++i, src += 9, dst += 9) {
        std::memcpy(dst, src, 9);
    }
    wr32(block, 0, rd32(block, 0) + (size & 0xFFFF));
}

// 1.13c D2Game+0x715E0
// Inventory/trade item pass (game in ECX, unit in EAX): D2Common #10920 on the unit must return a
// data block (fatal 0x87D); pages 1..12 of the inventory at unit+0x60 are selected with
// D2ItemSetPage, the block's +0x5C must be non-null (fatal 0x885), every entry on the unit+0xDC
// chain (next at +0x24) whose leading byte is 9 goes through 0xAE6F0, and 0x74810 saves the items
// with the (0x60, 0x2000, 0) header. A positive #10973 result on a unit whose +0x10 is neither 0
// nor 0x11 applies -result and the block's +0x10 through 0x21670.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_715e0(void* game, void* unit, const D2GameBatch14Env* env)
{
    void* data = env->getUnitInventory(unit);                        // D2Common #10920
    if (data == nullptr) d2game_fatal(0x87D);

    for (uint32_t page = 1; page < 0xD; ++page) {
        env->itemSetPage(rd32(unit, 0x60), page);                    // D2Common #10608
    }

    void* block = (void*)(uintptr_t)rd32(data, 0x5C);
    if (block == nullptr) d2game_fatal(0x885);

    for (void* e = (void*)(uintptr_t)rd32(unit, 0xDC); e != nullptr;
         e = (void*)(uintptr_t)rd32(e, 0x24)) {
        if (rd8(e, 0) == 9) env->itemChainCallback(unit);            // 0xAE6F0
    }

    uint32_t out = 0;
    env->saveItems(game, unit, 0x60, 0x2000, 0, &out);               // 0x74810
    const int32_t result = env->getSaveResult(unit);                 // D2Common #10973
    const uint32_t kind = rd32(unit, 0x10);
    if (result > 0 && kind != 0 && kind != 0x11) {
        env->adjustStat(-result);                                    // 0x21670
        env->adjustStat((int32_t)rd32(block, 0x10));                 // 0x21670
    }
}

// Retail 0x6FD1A3DC: unit type -> event-queue slot (D2MOO EVENT_MapUnitTypeToIndex, 1.10f table
// 0x6FD27D6C; player, monster, object, missile, item, tile, other).
static const int32_t g_d2game_6FD1A3DC[] = { 0, 1, 3, 2, 4, -1, 0 };

// 1.13c D2Game+0x8E810
// EVENT_AddInfiniteTimer (unit in EAX, event type in EBX, game in EDI; stack args p1, p2, p3):
// only event types below 0xF are queued. 0x8E680 supplies the timer node; flags |= 4,
// dwEventCustomId = p2, type = EBX, nExpireFrame = -1, dwEventCustomParam = p3, callback = p1.
// The node is pushed on the per-unit-type infinite list at game->[0xB8]+0xA04+slot*4 (slot from the
// 0x6FD1A3DC table, unit type 6 when the unit is null); node+0x1C = old head, node+0x20 = 0, and
// the old head's +0x20 points back at the node.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_8e810(void* game, void* unit, uint32_t type,
                                        uint32_t p1, uint32_t p2, uint32_t p3,
                                        const D2GameBatch14Env* env)
{
    if (type >= 0xF) return;

    uint8_t* ev = (uint8_t*)env->allocEventTimer(unit);               // 0x8E680
    ev[2] |= 4;
    wr32(ev, 0x14, p2);
    ev[0] = (uint8_t)type;
    wr32(ev, 0x04, 0xFFFFFFFF);
    wr32(ev, 0x18, p3);
    wr32(ev, 0x2C, p1);

    uint32_t unitType = unit ? rd32(unit, 0x00) : 6;
    const intptr_t slotIndex = (intptr_t)g_d2game_6FD1A3DC[unitType];
    uint8_t* queue = (uint8_t*)(uintptr_t)rd32(game, 0xB8);
    uint32_t* slot = (uint32_t*)(queue + 0xA04 + slotIndex * 4);

    const uint32_t head = rd32(slot, 0);
    wr32(ev, 0x1C, head);
    wr32(ev, 0x20, 0);
    if (head != 0) wr32((void*)(uintptr_t)head, 0x20, (uint32_t)(uintptr_t)ev);
    wr32(slot, 0, (uint32_t)(uintptr_t)ev);
}

// 1.13c D2Game+0x977E0
// Unit-handle event (game in ECX, unit id in EDX): fatal 0x2D3 when the game is null. The unit is
// looked up in bucket game+0x1320+(id&0x7F)*4 and the chain (next at +0xE4) is followed until the
// node's id at +0x0C matches; a missing node returns. A node with bit 8 of +0xC8 set is fatal
// (0x5A); otherwise +0xC4 gains flag 0x4000000 and either the 0x15D0(0,0xF) branch calls
// Ordinal 11103 and hands {node, 0xE0A20(game), 0, 0, 0x6400, 0} to 0x75E10, or 0xDEEE0(game, node,
// 0, 0) runs. Both paths finish with a zero-filled 7-word record whose first word is the unit id,
// passed to 0xDFC10.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_977e0(void* game, uint32_t unitId, const D2GameBatch14Env* env)
{
    if (game == nullptr) d2game_fatal(0x2D3);

    uint8_t* node = (uint8_t*)(uintptr_t)rd32(game, 0x1320 + (unitId & 0x7F) * 4);
    if (node == nullptr) return;
    while (rd32(node, 0x0C) != unitId) {
        node = (uint8_t*)(uintptr_t)rd32(node, 0xE4);
        if (node == nullptr) return;
    }
    if ((rd32(node, 0xC8) >> 8 & 1) != 0) d2game_fatal(0x5A);
    wr32(node, 0xC4, rd32(node, 0xC4) | 0x4000000);

    if (env->sub15d0(0, 0xF) == 0) {
        env->ordinal11103(node);                                     // D2Common Ordinal 11103
        uint32_t block[7] = { 0, 0, 0, 0, 0, 0, 0 };
        block[0] = (uint32_t)(uintptr_t)node;
        block[1] = env->sube0a20(game);                              // 0xE0A20
        block[4] = 0x6400;
        env->sub75e10(game, block);                                  // 0x75E10
    }
    else {
        env->subdeee0(game, node, nullptr, 0);                       // 0xDEEE0
    }

    uint32_t fields[7] = { 0, 0, 0, 0, 0, 0, 0 };
    fields[0] = unitId;
    env->subdfc10(fields);                                           // 0xDFC10
}

// 1.13c D2Game+0x9C6F0
// Party teardown (party in ECX, game in EAX; 1.10f sibling: the member-notify tail of
// PARTY_LeaveParty): the party's node list at party+4 (node = unit id at +0, next at +4) is walked.
// An empty list is retail's non-fatal warning at Party.cpp line 0xa5, leaving the function; a null
// game is fatal 0x2D3. Each node's unit is found in game+0x1120+(id&0x7F)*4 (next at +0xE4) and a
// missing unit is fatal 0x9A. 0xDFC10 runs the caller's per-unit callback on it, 0x8C570 sends its
// 0x8D packet, and retail logs Party.cpp line 0xa2 after every node.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_9c6f0(void* party, void* game, const D2GameBatch14Env* env)
{
    static const char kPartyFile[] = "..\\Source\\D2Game\\UNIT\\Party.cpp";

    uint8_t* n = (uint8_t*)(uintptr_t)rd32(party, 4);
    for (;;) {
        if (n == nullptr) {
            env->displayWarning(kPartyFile, 0xA5, 0);                // D2Common Ordinal 10046
            return;
        }
        if (game == nullptr) d2game_fatal(0x2D3);
        const uint32_t id = rd32(n, 0);
        uint8_t* u = (uint8_t*)(uintptr_t)rd32(game, 0x1120 + (id & 0x7F) * 4);
        while (u != nullptr && rd32(u, 0x0C) != id) {
            u = (uint8_t*)(uintptr_t)rd32(u, 0xE4);
        }
        if (u == nullptr) d2game_fatal(0x9A);

        env->subdfc10(u);                                            // 0xDFC10
        env->sub8c570(u, game);                                      // 0x8C570
        n = (uint8_t*)(uintptr_t)rd32(n, 4);
        env->displayWarning(kPartyFile, 0xA2, 0);                    // D2Common Ordinal 10046
    }
}
