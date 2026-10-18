// src/core/d2game/d2game_batch_37f20.cpp -- 1.13c D2Game.dll batch 37f20, transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Stage A batch lane: implementation + marker only, no spec, no vector. Retail module state is
// modelled as opaque host records read by their retail byte offsets. Register-only arguments
// (Ghidra's in_EAX / unaff_EDI / unaff_BP, and callee-saved registers the decompiler dropped) and
// stack areas it could not tie to a parameter are explicit parameters here. Unmapped callees are
// seams on D2GameBatch37F20Env in retail argument order. The fatal path Ordinal_10265(id) ->
// Ordinal_10024(&0x6FD19FF9, id) -> _exit(-1) is modelled by d2game_fatal, as in the other batches.
//
// Prior art: 1.10f D2Game has the same save-section readers in
// reference/D2MOO/source/D2Game/src/PLAYER/PlrSave2.cpp and PlrSave.cpp, and the same pet-list
// initialisation loop in PLAYERPETS_AllocPetList (src/core/d2moo/src/D2Game/PLAYER/PlayerPets.cpp).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { return (uint16_t)(rd8(p, o) | (rd8(p, o + 1) << 8)); }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v)  { ((uint8_t*)p)[o] = v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
// Retail addresses arrive as 32-bit integers in the decompile (in_EAX, ...).
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }
static inline uint16_t rd16(uint32_t a, uint32_t o) { return rd16((const void*)(uintptr_t)a, o); }
static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return rd8((const void*)(uintptr_t)a, o); }
static inline void*    rdptr(uint32_t a, uint32_t o) { return rdptr((const void*)(uintptr_t)a, o); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees and data tables the cached decompiles name but src/ does not
// implement. Ordinals are the 1.13c imports; the 1.10f crosswalk is named in the comment.
struct D2GameBatch37F20Env {
    // sgptDataTables: pPetTypeTxt at +0xBE8, nPetTypeTxtRecordCount at +0xBF0 (D2DataTbls.h).
    uint32_t pDataTables;
    // 1.13c D2Game+0x96EA0. In EAX it also reads the pet type (the decompile lost that register).
    void (*sub96EA0)(void* pGame, int32_t nPetType);
    // D2Common #10920, 1.10f UNITS_GetPlayerData(pUnit).
    uint32_t (*unitsGetPlayerData)(void* pUnit);
    // D2Common #10979, 1.10f WAYPOINTS_CopyAndValidateWaypointData(pWaypointData, pSavedData).
    void (*waypointCopyAndValidate)(void* pWaypointData, const void* pSavedData);
    // 1.13c D2Game+0xD60D0, called as (pAttacker, nArg2, flag, pDefender, 1).
    int32_t (*subD60D0)(void* pAttacker, uint32_t nArg2, uint8_t flag, void* pDefender, uint32_t one);
    // D2Common #10417, 1.10f UNITS_StoreLastAttacker(pUnit, pKiller). 1.10f declares void; retail
    // reads the returned byte (Ghidra's AL) into 0xD60D0's third argument.
    uint8_t (*unitsStoreLastAttacker)(void* pUnit, void* pKiller);
    // D2Common #10795, 1.10f QUESTRECORD_CopyBufferToRecord(pQuestRecord, pBuffer, nSize, bFlags).
    void (*questRecordCopyBufferToRecord)(void* pQuestRecord, const void* pBuffer, uint32_t nSize,
                                          int32_t nFlags);
    // D2Common #11042; 1.10f SKILLS_GetPlayerSkillCount, used here as a two-dword setter with layer
    // -1.
    void (*ordinal11042)(void* pUnit, uint32_t nValue, uint32_t nLayer);
    // D2Common #11072 (unused in 1.10f), same call shape as #11042.
    void (*ordinal11072)(void* pUnit, uint32_t nValue, uint32_t nLayer);
};

// 1.13c D2Game+0x37F20
// EAX = pDefender, EDI = pAttacker (register-only, dropped by the decompiler); Ghidra's
// unaff_retaddr is 0xD60D0's second argument, i.e. a stack argument of this function. A null EAX
// exits through Fog with id 0xC6, a null EDI with 0xC7. Otherwise D2Common #10417 runs first and its
// byte is forwarded to 0xD60D0(pAttacker, nArg2, byte, pDefender, 1); the result is its non-zero-ness.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_37f20(void* pDefender, void* pAttacker, uint32_t nArg2,
                                                const D2GameBatch37F20Env* env)
{
    if (pDefender == nullptr) d2game_fatal(0xC6);
    if (pAttacker == nullptr) d2game_fatal(0xC7);

    const uint8_t flag = env->unitsStoreLastAttacker(pDefender, pAttacker);
    return env->subD60D0(pAttacker, nArg2, flag, pDefender, 1) != 0;
}

// 1.13c D2Game+0x96F40
// pGame arrives in EBX: its only caller 0x97120 forwards its own EBX, the game pointer. For each pet
// type record the pet type id at record+0x0A is handed to 0x96EA0. The table pointer and record
// count are re-read every iteration, as retail does (0x96EA0 may reallocate player data).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_96f40(void* pGame, const D2GameBatch37F20Env* env)
{
    int32_t i = 0;
    int32_t n = (int32_t)rd32(env->pDataTables, 0xBF0);
    while (i < n) {
        const uint32_t record = rd32(env->pDataTables, 0xBE8) + (uint32_t)i * 0xE0;
        // Retail re-checks the offset and the count around the call; only call 0x96EA0 when both hold.
        if (i >= 0 && i < n && record != 0)
            env->sub96EA0(pGame, (int32_t)(int16_t)rd16(record, 0x0A));

        n = (int32_t)rd32(env->pDataTables, 0xBF0);
        ++i;
    }
}

// 1.13c D2Game+0x53040 (1.10f D2GAME_SAVE_ReadWaypointData_6FC8AD50 / PLRSAVE2_ReadWaypointData)
// ECX is a scratch pointer in the caller (0x55E10 passes &size) and is unused in the body. The player
// is the callee-saved EDI of the save loader, so it is explicit here. The section is 0x50 bytes: the
// 'WS' marker (0x5357) at +0, then three 24-byte act records from +4. Every record's first word must
// be 0x102, 0x101 or 0 (the 1.10f flag validation); a bad record returns 3 before the copy. The
// matched records are copied through D2Common #10979 into the player's three waypoint records at
// pPlayerData+0x1C. On success the consumed size 0x50 is returned through pOutSize.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_53040(void* pUnit, const void* pSection,
                                                  uint32_t* pOutSize,
                                                  const D2GameBatch37F20Env* env)
{
    uint8_t buffer[0x50];
    std::memcpy(buffer, pSection, sizeof buffer);

    *pOutSize = 0;
    const uint32_t pPlayerData = env->unitsGetPlayerData(pUnit);
    if (rd16(buffer, 0) != 0x5357) return 3;

    for (int32_t i = 0; i < 3; ++i) {
        const uint8_t* record = buffer + 4 + 24 * i;
        const uint16_t flags = rd16(record, 0);
        if (flags != 0x102 && flags != 0x101 && flags != 0) return 3;

        env->waypointCopyAndValidate(rdptr(pPlayerData, 0x1C + 4 * i), record);
    }

    *pOutSize = 0x50;
    return 0;
}

// 1.13c D2Game+0x530D0
// EDX = pSection, then pUnit and pOutSize on the stack (caller 0x55E10 passes local_18 and &local_1c).
// The section is 298 bytes: "Woo!" (0x216F6F57) at +0 and version 6 at +4, else it is rejected with 2.
// The three 96-byte quest records start at +10 -- 298 = 10 + 3*96, the layout the 1.13c-checked
// D2WEB_PLRSAVE2_ReadQuestSection in PlrSaveItems.cpp uses; the decompiler's stack-relative record
// pointer is four bytes low. They are copied into pPlayerData->pQuestData[0..2] (pPlayerData+0x10)
// through D2Common #10795; on success the consumed size 0x12A is returned through pOutSize.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_530d0(void* pUnit, const void* pSection,
                                                  uint32_t* pOutSize,
                                                  const D2GameBatch37F20Env* env)
{
    uint8_t buffer[0x12A];
    std::memcpy(buffer, pSection, sizeof buffer);

    *pOutSize = 0;
    if (rd32(buffer, 0) != 0x216F6F57u || rd32(buffer, 4) != 6) return 2;

    const uint32_t pPlayerData = env->unitsGetPlayerData(pUnit);
    for (int32_t i = 0; i < 3; ++i)
        env->questRecordCopyBufferToRecord(rdptr(pPlayerData, 0x10 + 4 * i), buffer + 10 + 96 * i,
                                           0x60, 1);

    *pOutSize = 0x12A;
    return 0;
}

// 1.13c D2Game+0x53340
// EAX = pSection (Ghidra's in_EAX), stack = &size. The player is the callee-saved EDI of the save
// loader, like 0x53040. The 0x34-byte section holds the 0x7701 marker at +0 (a 16-bit compare), three
// 8-byte quest-intro blobs at +4/+0xC/+0x14 and three 8-byte NPC-intro blobs at +0x1C/+0x24/+0x2C
// (D2PlrIntroSaveStrc). Each blob is copied into the buffer behind
// pPlayerIntro[i]->pQuestIntroFlags / ->pNpcIntroFlags; pPlayerIntro[0..2] is at pPlayerData+0x60.
// A wrong marker returns 0xB without touching the output size; success writes 0x34 there.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_53340(void* pUnit, const void* pSection,
                                                  uint32_t* pOutSize,
                                                  const D2GameBatch37F20Env* env)
{
    uint8_t buffer[0x34];
    std::memcpy(buffer, pSection, sizeof buffer);

    *pOutSize = 0;
    if (rd16(buffer, 0) != 0x7701) return 0xB;

    const uint32_t pPlayerData = env->unitsGetPlayerData(pUnit);
    for (int32_t i = 0; i < 3; ++i) {
        const uint32_t pIntro = (uint32_t)(uintptr_t)rdptr(pPlayerData, 0x60 + 4 * i);
        void* pQuestIntroBuffer = rdptr(rdptr(pIntro, 0x00), 0x00);
        wr32(pQuestIntroBuffer, 0, rd32(buffer, 4 + 8 * i));
        wr32(pQuestIntroBuffer, 4, rd32(buffer, 8 + 8 * i));
        void* pNpcIntroBuffer = rdptr(rdptr(pIntro, 0x04), 0x00);
        wr32(pNpcIntroBuffer, 0, rd32(buffer, 0x1C + 8 * i));
        wr32(pNpcIntroBuffer, 4, rd32(buffer, 0x20 + 8 * i));
    }

    *pOutSize = 0x34;
    return 0;
}

// 1.13c D2Game+0x534D0
// nSectionSize is param_2, pCursor advances, pOutSize receives the consumed size. Nothing is consumed
// unless pUnit is non-null, its first dword is zero, and pPlayerData+0x9C (the player's hotkey/skill
// record) is non-null; whenever that record exists the out size is set to 0x30, but the eight entries
// are only written when the section is larger than 0x48 bytes.
// Each entry is 8 bytes (u16 skill id, u8 hand, u32 -1) from pHotkeys+0x41C. The source (skill, hand)
// pairs are at pUnit+0, +6, +12, +18 and, after the outer 0x18 step, +24, +30, +36, +42. A skill byte
// in [0x9C,0xD8] becomes byte+0x41, 0xFF becomes 0xFFFF, anything else stays as-is.
// For a section larger than 0x51 the cursor's two dwords go to D2Common #11042 and #11072 with -1 and
// the cursor advances by 8 (the `+ (int)param_1` inside that address in the decompile is an artifact).
// Always returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_534d0(void* pUnit, uint32_t nSectionSize,
                                                  uint32_t* pCursor, uint32_t* pOutSize,
                                                  const D2GameBatch37F20Env* env)
{
    *pOutSize = 0;
    if (pUnit == nullptr || rd32(pUnit, 0) != 0) return 0;

    const uint32_t pHotkeys = rd32(env->unitsGetPlayerData(pUnit), 0x9C);
    if (pHotkeys == 0) return 0;

    *pOutSize = 0x30;
    if (nSectionSize <= 0x48) return 0;

    for (int32_t outer = 0; outer < 2; ++outer) {
        for (int32_t entry = 0; entry < 4; ++entry) {
            const uint32_t src = (uint32_t)(outer * 0x18 + entry * 6);
            const uint32_t dst = pHotkeys + 0x41C + (uint32_t)((outer * 4 + entry) * 8);

            uint16_t skill = rd8(pUnit, src);
            if (skill >= 0x9C && skill <= 0xD8) skill = (uint16_t)(skill + 0x41);
            else if (skill == 0xFF) skill = 0xFFFF;

            wr16((void*)(uintptr_t)dst, 0, skill);
            wr8((void*)(uintptr_t)dst, 2, rd8(pUnit, src + 1));
            wr32((void*)(uintptr_t)dst, 4, 0xFFFFFFFFu);
        }
    }

    if (nSectionSize > 0x51) {
        const uint32_t cursor = *pCursor;
        const uint32_t second = rd32(cursor, 4);
        env->ordinal11042(pUnit, rd32(cursor, 0), 0xFFFFFFFFu);
        env->ordinal11072(pUnit, second, 0xFFFFFFFFu);
        *pCursor = cursor + 8;
    }

    return 0;
}
