// src/core/d2game/d2game_batch_9bf40.cpp -- twelve 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): the party node count and
// party delete, the per-unit-type teardown phases the 1.13c SUNIT_RemoveUnit dispatcher (0xE20A0)
// factored out of the 1.10f inlined body, the quest-chain free, the four GAME/Clients.cpp save-header
// helpers, and the game-GUID list sweep.
//
// Records are opaque host objects addressed by retail byte offsets (rd32/rd16/rdptr/wr32/wrptr).
// Every callee that src/ does not own is a host seam on D2GameBatch9Env in retail argument order; a
// Ghidra call whose arguments arrived in registers (in_EAX/unaff_ESI/unaff_EDI/unaff_DI) gets an
// explicit parameter here, noted at each seam. The fatal path Fog #10265(id) -> #10024(text) ->
// _exit(-1) is modelled as d2game_fatal(id): only the exit is observable.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <Windows.h>   // CRITICAL_SECTION no-ops

static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees the cached decompiles name but src/ does not implement.
struct D2GameBatch9Env {
    // 0x9BD00 PARTY_GetPartyById(partyId)
    void*    (*getPartyById)(int16_t partyId);
    // 0x9C6F0 PARTY_FreeParty(game, party): unlink its nodes, then free party and nodes
    void     (*freeParty)(void* game, void* party);

    // Fog #10045 FOG_AllocPool(pool, size, file, line, 0) / #10046 FOG_FreePool(pool, p, file, line, 0)
    void*    (*allocPool)(void* pool, uint32_t size);
    void     (*freePool)(void* pool, void* p);

    // D2Common #10757 ITEMS_CheckIfSocketable(item)
    int32_t  (*itemIsSocketable)(void* item);
    // 0xE1110 SUNIT_RemoveAllItemsFromInventory(game, item)
    void     (*removeAllItemsFromInventory)(void* game, void* item);
    // D2Common #10820 INVENTORY_FreeInventory(inventory)
    void     (*freeInventory)(void* inventory);
    // 0x8E880 D2GAME_FreeTimersOnUnit(game, unit): free the unit's pTimer chain (+0xDC)
    void     (*freeTimers)(void* game, void* unit);

    // D2Common #10422 CHAT_FreeHoverMsg(pool, hoverText)
    void     (*freeHoverText)(void* pool, void* hoverText);
    // D2Common #10181 UNITS_GetTimerArg(unit) / 0x2E790 free the timer-arg chain and the arg block
    void*    (*getTimerArg)(void* unit);
    void     (*freeTimerArgChain)(void* timerArg);
    // D2Common #10178 MISSILE_FreeMissileData(missile)
    void     (*freeMissileData)(void* missile);

    // D2Common #10505 D2Common_10228(unit): remove the unit's dynamic-path footprint when it has one
    void     (*resetUnitFootprint)(void* unit);
    // D2Common #11016 UNITROOM_RemoveUnitFromRoom(unit)
    void     (*removeUnitFromRoom)(void* unit);
    // 0xE02A0 SUNIT_UnlinkFromUnitList(game, unit): type-indexed list head, next at unit+0xE4
    void     (*unlinkFromUnitList)(void* game, void* unit);
    // 0xE0BF0 player interact cleanup (game in EDI, unit in EAX): resolve the interact unit by
    // type/GUID, clear the interaction and release the vendor/object side
    void     (*playerInteractCleanup)(void* game, void* unit);
};

// ---- module state --------------------------------------------------------------------------------
static uint32_t g_batch9_gameGuids[1024];       // 0x6FD30C08..0x6FD31C08: game GUID per slot, 0 = empty
static CRITICAL_SECTION g_batch9_gameListLock;  // 0x6FD30BE8

// 1.13c D2Game+0x9BF40
// PARTY_GetNodeCount: the party's node count, or -1 when no party has the id.
int32_t PARTY_GetNodeCount_113c(int16_t partyId, const D2GameBatch9Env* env)
{
    void* party = env->getPartyById(partyId);          // 0x9BD00, party id in DX
    if (party == nullptr) return -1;

    int32_t count = 0;
    for (void* node = rdptr(party, 0x04); node != nullptr; node = rdptr(node, 0x04)) {
        ++count;                                        // pPartyNodes at +0x04, node pNext at +0x04
    }
    return count;
}

// 1.13c D2Game+0x9C7D0
// PARTY_Delete: unlink the party with the given id from the game's party-control list and destroy it.
// game in EAX, party id in DI. Null party control is fatal (0xB4).
void PARTY_Delete_113c(void* game, int16_t partyId, const D2GameBatch9Env* env)
{
    void* control = rdptr(game, 0x1D2C);                // pGame->pPartyControl
    if (control == nullptr) d2game_fatal(0xB4);

    void* previous = nullptr;
    void* party = rdptr(control, 0x04);                 // pPartyControl->pParties
    if (party != nullptr) {
        while (rd16(party, 0x00) != (uint16_t)partyId) {
            previous = party;
            party = rdptr(party, 0x04);                 // pParty->pNext at +0x04
            if (party == nullptr) return;
        }
        if (previous != nullptr) wrptr(previous, 0x04, rdptr(party, 0x04));
        else wrptr(control, 0x04, rdptr(party, 0x04));
        env->freeParty(game, party);                    // 0x9C6F0
    }
}

// 1.13c D2Game+0x10700
// Item teardown, first phase (1.10f sub_6FC502B0): a socketable item has its socketed contents
// removed and its inventory (unit+0x60) freed; then the unit's timer chain is released.
void ITEMS_FreeSocketableInventory_113c(void* game, void* item, const D2GameBatch9Env* env)
{
    if (env->itemIsSocketable(item)) {                  // D2Common #10757
        env->removeAllItemsFromInventory(game, item);   // 0xE1110
        env->freeInventory(rdptr(item, 0x60));          // D2Common #10820, pInventory
    }
    env->freeTimers(game, item);                        // 0x8E880
}

// 1.13c D2Game+0x2EE60
// Object teardown (1.10f OBJECTS_FreeHoverMessage): free the hover text (unit+0xA4) through the
// game's memory pool (game+0x1C), free the timer-argument chain, then the unit's timer chain.
void OBJECTS_FreeHoverMessage_113c(void* game, void* object, const D2GameBatch9Env* env)
{
    if (object != nullptr && rdptr(object, 0xA4) != nullptr) {
        env->freeHoverText(rdptr(game, 0x1C), rdptr(object, 0xA4)); // D2Common #10422
    }
    env->freeTimerArgChain(env->getTimerArg(object));   // D2Common #10181 then 0x2E790
    env->freeTimers(game, object);                      // 0x8E880
}

// 1.13c D2Game+0x33FF0
// QUESTS_FreeChainRecord: free a quest chain record (pNext at +0x04) through the game pool.
void QUESTS_FreeChainRecord_113c(void* pool, void* record, const D2GameBatch9Env* env)
{
    while (record != nullptr) {
        void* next = rdptr(record, 0x04);
        env->freePool(pool, record);                    // Fog #10046, QUESTS.CPP line 0x50
        record = next;
    }
}

// 1.13c D2Game+0x6F880
// MISSILES_Free: release the missile data (D2Common #10178) and the unit's timer chain.
void MISSILES_Free_113c(void* game, void* missile, const D2GameBatch9Env* env)
{
    if (missile != nullptr) {
        env->freeMissileData(missile);                  // D2Common #10178
        env->freeTimers(game, missile);                 // 0x8E880
    }
}

// 1.13c D2Game+0xE0C80
// SUNIT_RemoveUnit, first phase (1.13c split of 1.10f SUNIT_RemoveUnit): assert the unit and its type
// (< 6), run the player interact cleanup for players, reset the footprint for player/monster/object/
// missile, and (when unlinkFlag is 0) remove it from its room. Then unlink it from the game's
// type-indexed unit list. game in ECX/EDI, unit in EAX, unlinkFlag on the stack.
void SUNIT_RemoveUnitPre_113c(void* game, void* unit, uint32_t unlinkFlag, const D2GameBatch9Env* env)
{
    if (unit == nullptr) d2game_fatal(0x3AD);
    const uint32_t unitType = rd32(unit, 0x00);         // dwUnitType
    if (unitType >= 6) d2game_fatal(0x3AE);

    switch (unitType) {
    case 0:                                             // UNIT_PLAYER
        env->playerInteractCleanup(game, unit);         // 0xE0BF0
        [[fallthrough]];
    case 1:                                             // UNIT_MONSTER
    case 2:                                             // UNIT_OBJECT
    case 3:                                             // UNIT_MISSILE
        env->resetUnitFootprint(unit);                  // D2Common #10505 (D2Common_10228)
        [[fallthrough]];
    default:                                            // ITEM/TILE fall straight through
        if (unlinkFlag == 0) env->removeUnitFromRoom(unit); // D2Common #11016
        env->unlinkFromUnitList(game, unit);            // 0xE02A0
        return;
    }
}

// 1.13c D2Game+0x49630
// CLIENTS_FreeSaveHeader (ESI = client): free the pending save header (+0x17C) through the game pool,
// clear the size (+0x180), data-sent count (+0x188) and +0x184, and clear CLIENTFLAGEX_SAVE_LOADED
// (0x10) in +0x3D4. A null client is fatal (0xB69).
void CLIENTS_FreeSaveHeader_113c(void* client, const D2GameBatch9Env* env)
{
    if (client == nullptr) d2game_fatal(0xB69);
    if (rdptr(client, 0x17C) != nullptr) {
        void* pool = rdptr(rdptr(client, 0x1A8), 0x1C); // pClient->pGame->pMemoryPool
        env->freePool(pool, rdptr(client, 0x17C));      // Clients.cpp line 0xB6C
        wrptr(client, 0x17C, nullptr);
    }
    wr32(client, 0x180, 0);
    wr32(client, 0x188, 0);
    wr32(client, 0x184, 0);
    wr32(client, 0x3D4, rd32(client, 0x3D4) & ~0x10u);
}

// 1.13c D2Game+0x496B0
// CLIENTS_CopySaveDataToClient: when the stored size differs or a header is already held, free the
// old header, allocate exactly nSize from the game pool and record it (+0x17C / +0x180), then copy
// the data in. 1.13c copies unconditionally after the allocation (1.10f guarded on a non-null
// header); a null client is fatal (0xB55). nSize arrived in EAX, pData on the stack, client in ECX.
void CLIENTS_CopySaveDataToClient_113c(void* client, const void* data, uint32_t size,
                                       const D2GameBatch9Env* env)
{
    if (client == nullptr) d2game_fatal(0xB55);

    if (rd32(client, 0x180) != size || rdptr(client, 0x17C) != nullptr) {
        void* pool = rdptr(rdptr(client, 0x1A8), 0x1C); // pClient->pGame->pMemoryPool
        if (rdptr(client, 0x17C) != nullptr) {
            env->freePool(pool, rdptr(client, 0x17C));  // Clients.cpp line 0xB5A
        }
        wrptr(client, 0x17C, env->allocPool(pool, size)); // #10045, Clients.cpp line 0xB5C
        wr32(client, 0x180, size);
    }
    std::memcpy(rdptr(client, 0x17C), data, size);
}

// 1.13c D2Game+0x49760
// CLIENTS_GetSaveHeaderSize: the pending save header's size (+0x180); null client fatal (0xB4B).
uint32_t CLIENTS_GetSaveHeaderSize_113c(void* client)
{
    if (client == nullptr) d2game_fatal(0xB4B);
    return rd32(client, 0x180);
}

// 1.13c D2Game+0x49790
// CLIENTS_GetSaveHeader: the pending save header pointer (+0x17C); null client fatal (0xB41).
void* CLIENTS_GetSaveHeader_113c(void* client)
{
    if (client == nullptr) d2game_fatal(0xB41);
    return rdptr(client, 0x17C);
}

// 1.13c D2Game+0x2A0D0
// GAME_RemoveGuidFromList (ESI = GUID): under the game-list lock, zero every slot whose GUID equals
// the argument. The list is the 1024 dwords at 0x6FD30C08..0x6FD31C08.
void GAME_RemoveGuidFromList_113c(uint32_t guid)
{
    EnterCriticalSection(&g_batch9_gameListLock);       // Fog #10050
    for (int32_t i = 0; i < 1024; ++i) {                // piVar1 < 0x6FD31C08
        if (g_batch9_gameGuids[i] == guid) g_batch9_gameGuids[i] = 0;
    }
    LeaveCriticalSection(&g_batch9_gameListLock);
}
