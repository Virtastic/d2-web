// src/core/d2game/d2game_batch_B24D0.cpp -- Stage A batch lane (OBJECTIVE v8.3): implementations for
// nine 1.13c D2Game.dll rows transcribed from the cached Ghidra decompiles
// (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Implementation + marker only: no spec, no vector.
//
// Host model: retail records are opaque host objects addressed by their retail byte offsets
// (rd8/rd16/rd32/rdptr, wr8/wr16/wr32/wrptr). Every callee src/ does not own yet is a seam on
// D2GameBatchB74Env in retail argument order; register-only arguments (EAX/EBX/ECX/EDX/ESI/EDI
// registers Ghidra failed to name, and stack slots Ghidra dropped) are explicit parameters below.
// Every ABI in this file was settled against the retail bytes with objdump at the 0x6FC20000 image
// base, because the decompiler lost the register arguments on 0x967B0/0xD29F0/0xDC030/0xDE8F0 and
// mis-read the packet buffers. The fatal path Fog #10265(id) -> #10024(text) -> _exit(-1) is
// modelled as d2game_fatal(id): only the exit is observable.
//
// Prior art (1.10f): src/D2Game/GAME/Arena.cpp (ARENA_AllocArenaUnit) for 0xB24D0;
// src/D2Game/PLAYER/PlayerPets.cpp (D2PetListStrc/D2PetDataStrc walk) for 0x967B0;
// src/D2Game/ITEMS/ItemMode.cpp and SkillItem.cpp (IFLAG_TARGETING + USESTACKABLEITEM 0x3F loop)
// for 0xD29F0; src/D2Game/UNIT/SUnitDmg.cpp (ComputeExperienceGain / DistributeExperience /
// AddExperienceForPlayer / D2PartyExpStrc) for 0xDC030 and 0xDE8F0. The three packet senders
// (0x8B9B0/0x8BD00/0x8AF10/0x7A540) match the 1.13c layouts in
// reference/Diablo2PacketsData/src/data/1.13c/gs2client.json (0x20 ATTRIBUTEUPDATE,
// 0x1D/1E/1F SETATTR, 0x42 CLEARCURSOR, 0x0A REMOVEOBJECT). 1.13c ordinal numbers are kept as the
// seam names (src/core/fn/import_thunk.cpp); no guessed 1.10f name is substituted.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>
#include "../fn/hookenv.h"

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees src/ does not own. The retail data tables (sgptDataTables) are a host
// object: nPetTypeTxtRecordCount at +0xBF0.
struct D2GameBatchB74Env {
    void*    dataTables;

    // Fog #10045 FOG_AllocPool(pool in ECX, size in EDX, file, line, 0); 0xB24D0 passes 8/0x47.
    void*    (*ord10045)(void* pPool, uint32_t nSize, uint32_t nLine);
    void*    (*ord10920)(void* pUnit);          // D2Common #10920 UNITS_GetPlayerData

    // 0x8B9B0 / 0x8BD00 / 0x8AF10 / 0x7A540: D2GAME_PACKETS_SendPacket(client in EAX, buf, len).
    void     (*sendPacket)(void* pClient, const void* pBuf, uint32_t nLen);

    // 0xD29F0: inventory iteration and item fields.
    void*    (*ord10460)(void* pInventory);
    void*    (*ord10464)(void* pNode);
    void*    (*ord11147)(void* pNode);
    int32_t  (*ord10202)(void* pItem, uint32_t nStat, uint32_t nFlag, const void* pData);
    void     (*ord10977)(void* pItem, uint32_t nStat, int32_t nValue);

    // 0xDC030: experience-gain factor ladder.
    int32_t  (*ord10066)(int32_t nClassId);     // D2Common #10066 max level for class
    uint32_t (*fnDAA40)(int32_t nDefenderLevel, uint32_t nDefenderExp, int32_t nAttackerLevel);
    uint32_t (*fnDA9E0)(uint32_t nValue, int32_t nAttackerLevel);
    int32_t  (*ord10973)(void* pUnit, uint32_t nStat, int32_t nParam);
    int32_t  (*fn14D0)(int32_t nDivisor, uint32_t nValue, int32_t nPercent);
    uint32_t (*fnDB6B0)(void* pAttacker, void* pGame, int32_t nAttackerLevel, uint32_t nValue);

    // 0xDE8F0: retail 0x9C070 with callback 0xDAED0 (x86) fills the party structure.
    void     (*iterateParty)(void* pAttacker, void* pGame, void* pPartyExp);
    void     (*addExperienceForPlayer)(void* pPlayer, void* pGame, int32_t nLevel, uint32_t nExp);
};

static const D2GameBatchB74Env kNoEnv{};
static const D2GameBatchB74Env& batchEnv(const uint32_t* r) {
    return r[5] ? *(const D2GameBatchB74Env*)(uintptr_t)r[5] : kNoEnv;
}

// 1.13c D2Game+0xB24D0
// ARENA_AllocArenaUnit. EAX=pGame (its memory pool at +0x1C), stack argument pUnit, ret 4. Allocate
// an 8-byte D2ArenaUnitStrc from the game pool, clear both dwords, and store it in the player data
// at +0x34. Asserts 0x4A when the unit has no player data.
static void d2game_B24D0_113c(void* pGame, void* pUnit, const D2GameBatchB74Env& env)
{
    uint8_t* pArenaUnit = (uint8_t*)env.ord10045((void*)(uintptr_t)rd32(pGame, 0x1C), 8, 0x47);
    wr32(pArenaUnit, 0x0, 0);
    wr32(pArenaUnit, 0x4, 0);
    void* pPlayerData = env.ord10920(pUnit);
    if (pPlayerData == nullptr) d2game_fatal(0x4A);
    wr32(pPlayerData, 0x34, (uint32_t)(uintptr_t)pArenaUnit);   // a 4-byte slot at 1.13c's layout: a pointer-sized store clobbers +0x38
}

// 1.13c D2Game+0x967B0
// First pet GUID for a pet type: EAX=pUnit, ESI=nPetType, result in EAX, no stack arguments.
// Asserts 0x171 without player data. The pet list is at player data +0x44; its array base is the
// first pointer, entries are 12 bytes (D2PetDataStrc pointer / count / max), and the returned GUID
// is the record's +0x04 dword. -1 when the list, the type range, the slot or the pet data is absent.
[[maybe_unused]] static int32_t d2game_967B0_113c(void* pUnit, int32_t nPetType, const D2GameBatchB74Env& env)
{
    uint8_t* pPlayerData = (uint8_t*)env.ord10920(pUnit);
    if (pPlayerData == nullptr) d2game_fatal(0x171);
    uint8_t* pPetList = (uint8_t*)rdptr(pPlayerData, 0x44);
    if (pPetList == nullptr) return -1;
    if (nPetType < 0) return -1;
    if (nPetType >= (int32_t)rd32(env.dataTables, 0xBF0)) return -1;
    const uintptr_t nSlot = (uintptr_t)rdptr(pPetList, 0x0) + (uintptr_t)nPetType * 12;
    if (nSlot == 0) return -1;
    uint8_t* pPetData = (uint8_t*)rdptr((const void*)nSlot, 0x0);
    if (pPetData == nullptr) return -1;
    return (int32_t)rd32(pPetData, 0x4);
}

// 1.13c D2Game+0x8B9B0
// ATTRIBUTEUPDATE (0x20). EAX carries the attribute id in AX, the client is stack argument 1
// (player data +0x9C at the retail call sites), stack 2 is the packet id (the callers pass 0x20),
// stack 3 the unit GUID and stack 4 the value. Asserts 0x4B8 for an attribute id of 0xFF or more.
// Buffer: id, GUID (dword), attribute (byte), value (dword) -- 10 bytes.
static void d2game_8B9B0_113c(void* pClient, uint8_t nPacketId, uint32_t nGuid, uint32_t nValue,
                              uint16_t nAttribute, const D2GameBatchB74Env& env)
{
    if (nAttribute >= 0xFF) d2game_fatal(0x4B8);
    uint8_t packet[10];
    packet[0] = nPacketId;
    std::memcpy(packet + 1, &nGuid, 4);
    packet[5] = (uint8_t)nAttribute;
    std::memcpy(packet + 6, &nValue, 4);
    env.sendPacket(pClient, packet, 10);
}

// 1.13c D2Game+0xD29F0
// Clear the targeting flag on every inventory item of a unit. Stack argument 1 is the unit, ret 4.
// Requires a unit and its inventory at +0x60. The item fields (#10202, stat 4, flag 0x1D7) are
// cleared with #10977(item, 4, 0); for a player unit (type 0) the item's GUID is announced with a
// USESTACKABLEITEM (0x3F) packet on the client from player data +0x9C:
// {0x3F, 0xFF, GUID or 0, 0xFFFF} -- 8 bytes. The item-node iteration is #10460/#10464 and the node
// to item conversion #11147.
static void d2game_D29F0_113c(void* pUnit, const D2GameBatchB74Env& env)
{
    if (pUnit == nullptr) return;
    void* pInventory = rdptr(pUnit, 0x60);
    if (pInventory == nullptr) return;
    void* pNode = env.ord10460(pInventory);
    if (pNode == nullptr) return;
    do {
        uint8_t* pItem = (uint8_t*)env.ord11147(pNode);
        if (env.ord10202(pItem, 4, 0x1D7, (const void*)(uintptr_t)0x6FD1B14C) != 0) {
            env.ord10977(pItem, 4, 0);
            if (rd32(pUnit, 0x0) == 0) {
                // The retail code does not re-check the player data after the unit-type test.
                void* pClient = rdptr(env.ord10920(pUnit), 0x9C);
                uint8_t packet[8];
                packet[0] = 0x3F;
                packet[1] = 0xFF;
                const uint32_t nGuid = pItem != nullptr ? rd32(pItem, 0xC) : 0;
                std::memcpy(packet + 2, &nGuid, 4);
                wr16(packet, 6, 0xFFFF);
                env.sendPacket(pClient, packet, 8);
            }
        }
        pNode = env.ord10464(pNode);
    } while (pNode != nullptr);
}

// 1.13c D2Game+0x8AF10
// CLEARCURSOR (0x42). EAX=record, stack argument 1 = client, ret 4. {0x42, record type or 6,
// GUID or -1} -- 6 bytes; the record type is the low byte of the record's +0x00 dword and the GUID
// its +0x0C dword.
static void d2game_8AF10_113c(void* pRecord, void* pClient, const D2GameBatchB74Env& env)
{
    uint8_t packet[6] = {};
    packet[0] = 0x42;
    uint32_t nType = 6;
    uint32_t nGuid = 0xFFFFFFFF;
    if (pRecord != nullptr) {
        nType = rd32(pRecord, 0x0) & 0xFF;
        nGuid = rd32(pRecord, 0xC);
    }
    packet[1] = (uint8_t)nType;
    std::memcpy(packet + 2, &nGuid, 4);
    env.sendPacket(pClient, packet, 6);
}

// The object-removal notice: moved to d2game_7a540.h.

// 1.13c D2Game+0xDC030
// SUNITDMG_ComputeExperienceGain. EAX=nDefenderExperience, EBX=pAttacker, EDI=nAttackerLevel,
// stack 1 = pGame, stack 2 = nDefenderLevel, ret 8; result in EAX. Clamp the defender experience to
// 0x7FFFFF and return 1 when it is not positive; take the attacker's class id from +0x04 only for a
// type-0 unit and return 0 when the attacker's level is not below #10066(class)'s max level. Then
// the level-difference ladder (0xDAA40), its experience-ratio adjustment (0xDA9E0), the per-cent
// bonus from stat 0x55 (#10973 -> 0x14D0 with 100) and the hireling/party tail 0xDB6B0.
static uint32_t d2game_DC030_113c(uint32_t nDefenderExperience, void* pAttacker,
                                  int32_t nAttackerLevel, void* pGame, int32_t nDefenderLevel,
                                  const D2GameBatchB74Env& env)
{
    uint32_t nExp = nDefenderExperience;
    if ((int32_t)nExp > 0x7FFFFF) {
        nExp = 0x7FFFFF;
    } else if ((int32_t)nExp <= 0) {
        return 1;
    }

    int32_t nClassId = 0;
    if (pAttacker != nullptr && rd32(pAttacker, 0x0) == 0) nClassId = (int32_t)rd32(pAttacker, 0x4);
    const int32_t nMaxLevel = env.ord10066(nClassId);
    if (nAttackerLevel >= nMaxLevel) return 0;

    uint32_t nValue = env.fnDAA40(nDefenderLevel, nExp, nAttackerLevel);
    nValue = env.fnDA9E0(nValue, nAttackerLevel);
    const int32_t nPercent = env.ord10973(pAttacker, 0x55, 0);
    if (nPercent != 0) nValue += (uint32_t)env.fn14D0(100, nValue, nPercent);
    return env.fnDB6B0(pAttacker, pGame, nAttackerLevel, nValue);
}

// 1.13c D2Game+0xDE8F0
// Party experience distribution (the helper SUNITDMG_DistributeExperience calls after the killer
// resolution). EAX=pAttacker, stack 1 = pGame, stack 2 = pDefender, stack 3 = nAttackerLevel,
// stack 4 = nDefenderLevel, stack 5 = nDefenderExperience, ret 0x14. The 0x4C party structure is
// {pDefender, members[8], levels[8], nMembers, nLevelSum} and is filled by the retail party
// iteration (0x9C070 with callback 0xDAED0). No members or no level sum ends the function; a
// one-member party credits the attacker directly; otherwise the defender experience is inflated by
// 89/256 per extra member, turned into a per-level share by the level sum, and each member receives
// ComputeExperienceGain(share, member level) through AddExperienceForPlayer (0xDE6D0).
struct D2GamePartyExpStrc {
    void*    pDefender;
    void*    pMembers[8];
    int32_t  nLevels[8];
    int32_t  nMembers;
    int32_t  nLevelSum;
};

static void d2game_DE8F0_113c(void* pAttacker, void* pGame, void* pDefender, int32_t nAttackerLevel,
                              int32_t nDefenderLevel, uint32_t nDefenderExperience,
                              const D2GameBatchB74Env& env)
{
    D2GamePartyExpStrc party;
    std::memset(&party, 0, sizeof party);
    party.pDefender = pDefender;
    env.iterateParty(pAttacker, pGame, &party);
    if (party.nMembers <= 0 || party.nLevelSum <= 0) return;

    if (party.nMembers == 1) {
        const uint32_t nGain = d2game_DC030_113c(nDefenderExperience, pAttacker, nAttackerLevel,
                                                 pGame, nDefenderLevel, env);
        env.addExperienceForPlayer(pAttacker, pGame, nAttackerLevel, nGain);
        return;
    }

    const int32_t nExp = (int32_t)nDefenderExperience
        + (89 * (int32_t)nDefenderExperience * (party.nMembers - 1)) / 256;
    const float fMultiplier = (float)((double)nExp / (double)party.nLevelSum);
    for (int32_t i = 0; i < party.nMembers; i++) {
        const int32_t nShare = (int32_t)((double)party.nLevels[i] * (double)fMultiplier);
        const uint32_t nGain = d2game_DC030_113c((uint32_t)nShare, party.pMembers[i],
                                                 party.nLevels[i], pGame, nDefenderLevel, env);
        env.addExperienceForPlayer(party.pMembers[i], pGame, party.nLevels[i], nGain);
    }
}

// 1.13c D2Game+0xB24D0 FUNCTION-mode probe: r[0]=pGame EAX, r[1]=pUnit (stack), r[2]=arena blob 0xA2E6 hands back,
// r[3]=player data blob 0xA4C0 hands back, r[5]=hook log. Both seams log through HookEnv as the capture's hook recorder
// does (0xA2E6: ECX pool, EDX size, file, line, 0; 0xA4C0: the unit).
#include "../fn/hookenv.h"
static HookEnv* g_b24d0H; static uint32_t g_b24d0Ar, g_b24d0Pd;
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_b24d0(uint32_t* r)
{
    static uint32_t none[2] = {0, 0};
    HookEnv he((uint32_t*)(uintptr_t)r[5], none); g_b24d0H = &he;
    g_b24d0Ar = r[2]; g_b24d0Pd = r[3];
    D2GameBatchB74Env env{};
    env.ord10045 = [](void* pool, uint32_t size, uint32_t line) -> void* {
        g_b24d0H->h(0xA2E6, 0, (uint32_t)(uintptr_t)pool, size, 0, 0, 0xB20FB568u, line, 0);
        return (void*)(uintptr_t)g_b24d0Ar; };
    env.ord10920 = [](void* unit) -> void* {
        g_b24d0H->h(0xA4C0, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)unit);
        return (void*)(uintptr_t)g_b24d0Pd; };
    d2game_B24D0_113c((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], env);
    r[4] = 0;
}

// The 0x967B0 entry point is d2game_967b0 in d2game_967b0.cpp; the FN probe supplies the seams.

// Probe entry for 0x8B9B0 (EAX = attribute, client and packet fields on the stack); `send` takes the place of 0x8A3E0.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2game_8b9b0_send(void* client, uint8_t nPacketId, uint32_t nGuid, uint32_t nValue,
                                            uint16_t nAttribute, void (*send)(void*, const void*, uint32_t))
{
    D2GameBatchB74Env env{};
    env.sendPacket = send;
    d2game_8B9B0_113c(client, nPacketId, nGuid, nValue, nAttribute, env);
}

// 1.13c D2Game+0xD29F0: the entry the FN probe drives, with its own seam table (same walk as
// d2game_D29F0_113c above; the probe's seams take the place of the D2Common iteration, the
// predicate, the stat write, the player-data lookup and the client send).
struct D2GameD29F0Env {
    void*   (*first)(void* pInventory);
    void*   (*next)(void* pNode);
    void*   (*nodeItem)(void* pNode);
    int32_t (*check)(void* pItem, uint32_t a, uint32_t b, const void* pFile);
    void    (*setStat)(void* pItem, uint32_t a, int32_t v);
    void*   (*playerData)(void* pUnit);
    void    (*send)(void* pClient, const void* pBuf, uint32_t nLen);
};
extern "C" void d2_fn_d2game_D29F0(void* pUnit, const D2GameD29F0Env* e)
{
    if (pUnit == nullptr) return;
    void* pInventory = rdptr(pUnit, 0x60);
    if (pInventory == nullptr) return;
    void* pNode = e->first(pInventory);
    if (pNode == nullptr) return;
    do {
        uint8_t* pItem = (uint8_t*)e->nodeItem(pNode);
        if (e->check(pItem, 4, 0x1D7, (const void*)(uintptr_t)0x6FD1B14C) != 0) {
            e->setStat(pItem, 4, 0);
            if (rd32(pUnit, 0x0) == 0) {
                void* pClient = rdptr(e->playerData(pUnit), 0x9C);
                uint8_t packet[8];
                packet[0] = 0x3F;
                packet[1] = 0xFF;
                const uint32_t nGuid = pItem != nullptr ? rd32(pItem, 0xC) : 0;
                std::memcpy(packet + 2, &nGuid, 4);
                wr16(packet, 6, 0xFFFF);
                e->send(pClient, packet, 8);
            }
        }
        pNode = e->next(pNode);
    } while (pNode != nullptr);
}

// Probe entry for 0x8AF10 (EAX = record, client on the stack); `send` takes the place of 0x8A3E0.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2game_8af10_send(void* record, void* client, void (*send)(void*, const void*, uint32_t))
{
    D2GameBatchB74Env env{};
    env.sendPacket = send;
    d2game_8AF10_113c(record, client, env);
}

// 1.13c D2Game+0xDC030 replay entry: regs eax=defender exp, ebx=attacker, edi=attacker level;
// r[0]=eax r[1]=ebx r[2]=edi r[3]=pGame r[4]=result r[5]=defender level r[6]=log r[7]=sc.
static HookEnv* g_dc030e;
static int32_t  dc030_a48a(int32_t c) { return (int32_t)g_dc030e->h(0xA48A, 0, 0, 0, 0, 0, (uint32_t)c); }
static uint32_t dc030_aa40(int32_t dl, uint32_t ex, int32_t al) { return g_dc030e->h(0xDAA40, (uint32_t)al, (uint32_t)dl, ex); }
static uint32_t dc030_a9e0(uint32_t v, int32_t al) { return g_dc030e->h(0xDA9E0, v, 0, 0, 0, 0, (uint32_t)al); }
static int32_t  dc030_a42a(void* u, uint32_t st, int32_t l) { return (int32_t)g_dc030e->h(0xA42A, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)u, st, (uint32_t)l); }
static int32_t  dc030_14d0(int32_t dv, uint32_t v, int32_t pc) { return (int32_t)g_dc030e->h(0x14D0, (uint32_t)pc, (uint32_t)dv, v); }
static uint32_t dc030_b6b0(void* at, void* g, int32_t al, uint32_t v) { return g_dc030e->h(0xDB6B0, (uint32_t)(uintptr_t)at, 0, 0, 0, 0, (uint32_t)(uintptr_t)g, (uint32_t)al, v); }
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_dc030(uint32_t* r)
{
    HookEnv e((uint32_t*)(uintptr_t)r[6], (const uint32_t*)(uintptr_t)r[7]); g_dc030e = &e;
    D2GameBatchB74Env env{};
    env.ord10066 = dc030_a48a; env.fnDAA40 = dc030_aa40; env.fnDA9E0 = dc030_a9e0;
    env.ord10973 = dc030_a42a; env.fn14D0 = dc030_14d0; env.fnDB6B0 = dc030_b6b0;
    r[4] = d2game_DC030_113c(r[0], (void*)(uintptr_t)r[1], (int32_t)r[2], (void*)(uintptr_t)r[3], (int32_t)r[5], env);
}

// 1.13c D2Game+0xDE8F0 (r[0]=pGame, r[1]=pAttacker EAX, r[2]=pDefender, r[3]=nAttackerLevel,
// r[6]=nDefenderLevel, r[7]=nDefenderExperience)
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_de8f0_old(uint32_t* r)
{
    d2game_DE8F0_113c((void*)(uintptr_t)r[1], (void*)(uintptr_t)r[0], (void*)(uintptr_t)r[2],
                      (int32_t)r[3], (int32_t)r[6], r[7], batchEnv(r));
    r[4] = 0;
}
