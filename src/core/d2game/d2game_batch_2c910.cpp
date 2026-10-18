// src/core/d2game/d2game_batch_2c910.cpp -- ten 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). They are the client-save
// trigger, character-preview fill, party leave, client-unit release, the three player-save branches,
// the join-game validator pair and the CRT 64-bit signed divide helper.
//
// The retail module state is modelled as opaque host records read by their retail byte offsets:
// scalars are 32-bit words (rd32/wr32), pointers the decompiles follow (game+0x88 client-list head,
// client+0x4A8 next, client+0x174 player, party+0x04 node head, node+0x04 next) hold host pointers.
//
// Every unmapped callee is a seam in Batch5Env, in retail argument order; a Ghidra call that passes
// arguments only in registers gets an explicit parameter, noted at the seam. The fatal path
// Ordinal_10265(code) -> Ordinal_10024(&0x6FD19FF9, text) -> _exit(-1) is modelled by fatal(code).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <unistd.h>

static inline uint8_t rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr8(void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void fatal(uint32_t) { _exit(-1); }

// Host seams for the callees the cached decompiles name but src/ does not implement.
struct Batch5Env {
    // 0x4AD10 CLIENTS_GetPlayerFromClient(pClient, bIgnoreDeath)
    void*    (*getPlayerFromClient)(void* client, int32_t ignoreDeath);
    // 0xE03A0 SUNIT_GetServerUnit(game, guid, type-in-EAX)
    void*    (*getServerUnit)(uint32_t game, uint32_t guid, uint32_t typeEax);
    // 0x54320 D2GAME_SAVE_WriteFile(game, player, name, arg): retail passes these in registers
    int32_t  (*saveWriteFile)(void* game, void* player, const char* name, uint32_t arg);
    // D2Common import #10587 STATLIST_GetUnitBaseStat(unit, stat, layer)
    int32_t  (*unitGetBaseStat)(void* unit, int32_t stat, int32_t layer);
    // D2Common import #10086 FOG_Encode14BitsToString(dst, value)
    void     (*encode14)(void* dst, uint32_t value);
    // D2Common import #10457 INVENTORY_GetItemSaveGfxInfo(player, components, colors)
    int32_t  (*getItemSaveGfxInfo)(void* player, uint8_t* components, uint8_t* colors);
    // D2Common import #506 SStrLen (the 33-byte preview check)
    uint32_t (*strLen)(const char* text);

    // 0x53170 SAVE_CalculateChecksum(data, size)
    int32_t  (*calculateChecksum)(const void* data, uint32_t size);
    // 0xEC7F0 PLRSAVE2_CreateSaveFile(game, player, out, &size, cap, interacts, arg)
    void     (*createSaveFile)(void* game, void* player, void* out, uint32_t* size,
                               uint32_t cap, uint32_t interacts, uint32_t arg);
    // CRT via 0x22FD0 fopen / fwrite / fclose
    void*    (*fileOpen)(const char* path, const char* mode);
    void     (*fileWrite)(const void* data, uint32_t size, void* file);
    void     (*fileClose)(void* file);
    // D2Common import #10029 (trace) and #10115 (FOG_GetSavePath)
    void     (*trace)(const char* format, const char* arg);
    void     (*getSavePath)(char* out, uint32_t cap);
    // D2Common import #10920 (client from player)
    void*    (*getClientFromPlayer)(void* player);
    // 0x49790 CLIENTS_GetSaveHeader(client)
    void*    (*getSaveHeader)(void* client);
    // 0x49760 CLIENTS_GetSaveHeaderSize(client)
    uint32_t (*getSaveHeaderSize)(void* client);
    // 0x496B0 set the client's save checksum (+0x18C) and realm/client-info pointer (+0x68)
    void     (*setSaveChecksum)(void* client, uint32_t checksum);
    void     (*setClientInfo)(void* client, void* clientInfo);
    // 0x49630 CLIENTS_FreeSaveHeader(client); 0x4A190 CLIENTS_AttachSaveFile(client, data, size)
    void     (*freeSaveHeader)(void* client);
    void     (*attachSaveFile)(void* client, const void* data, uint32_t size);

    // 0x9BEB0 PARTY_GetPartyIdForUnitOwner(unit); unit arrives in EAX in retail
    int16_t  (*getPartyIdForUnitOwner)(void* unit);
    // 0x9BD00 find party by id
    void*    (*getPartyById)(int16_t partyId);
    // 0x9BF40 count the party's nodes
    int32_t  (*countPartyNodes)(int16_t partyId);
    // 0x9C7D0 destroy a party with fewer than two nodes
    void     (*destroyParty)(int16_t partyId);
    // 0xDFC10 unit state update; 0x8C570 D2GAME_PACKETS_SendPacket0x8D
    void     (*unitUpdate)(void* unit);
    void     (*sendPacket8D)(void* unit);

    // D2Common import #10383 resets the unit's light map; the comments reads a count from the
    // register pair.  #10144 applies the per-entry collision update.
    uint32_t (*unitResetLightMapList)(void* unit, void** outList);
    void     (*setUnitAliveCollision)(void* unit);

    // 0x498B0 (client id -> its current game name, 16 bytes); 0x497C0 (name matches game name)
    int32_t  (*clientGameName)(uint32_t clientId, uint8_t* out16);
    int32_t  (*clientNameMatchesGameName)(const char* name, const uint8_t* gameName);
    // 0x2A3B0 GAME_GetGameGUIDFromGameId; 0x2A110 IsClientNameValid (name in ESI in retail)
    int32_t  (*getGameGuidFromGameId)(uint16_t gameId);
    int32_t  (*isClientNameValid)(const char* name);
    int32_t  (*isBadCodePtr)(void* fn);

    // 0x6FD31C3C gpD2EventCallbackTable; the callbacks used here are the struct's function slots.
    void**   callbackTable;
};

// ---- save-module state ---------------------------------------------------------------------------
static uint32_t g_batch5_writeSaveFile;    // 0x6FD2D8BC gbWriteSaveFile: file writing enabled
static uint32_t g_batch5_sameSaveCount;    // 0x6FD31F84: realm saves skipped as unchanged

// ---- shared: verify a NUL-terminated name is at most 15 characters -------------------------------
static bool nameFits15(const char* name)
{
    if (name == nullptr) return false;
    for (int32_t i = 0; ; ++i) {
        if (name[i] == '\0') return true;
        if (i >= 15) return false;   // decompile returns 0 on the 16th non-NUL byte
    }
}

// 1.13c D2Game+0x2C910
// GAME_TriggerClientSave: resolve pClient's player (0x4AD10); when it exists, save it through
// 0x54320. When that save succeeds, walk the game's client list: save every other client's player,
// and for every client with a player, call the ladder callback (+0x28) with the client's name,
// class, level and experience stats, flags and save timestamp.
void GAME_TriggerClientSave_113c(void* client, void* game, const Batch5Env* env)
{
    if (client == nullptr) return;
    void* player = env->getPlayerFromClient(client, 0);
    if (player == nullptr) return;
    if (!env->saveWriteFile(game, player, (const char*)client + 0x0D, 1)) return;

    for (void* i = rdptr(game, 0x88); i != nullptr; i = rdptr(i, 0x4A8)) {
        void* p = env->getPlayerFromClient(i, 0);
        if (p == nullptr) continue;
        if (i != client) env->saveWriteFile(game, p, (const char*)i + 0x0D, 0);
        if (env->callbackTable != nullptr && env->callbackTable[10 /* +0x28 pfUpdateCharacterLadder */] != nullptr) {
            const int32_t level = env->unitGetBaseStat(p, 0x0C, 0);   // STAT_LEVEL
            const int32_t exp = env->unitGetBaseStat(p, 0x0D, 0);     // STAT_EXPERIENCE
            // (name, classId, level, experience, 0, client flags, &nSaveCreationTimestamp)
            typedef int32_t (*LadderFn)(const char*, int32_t, int32_t, int32_t, int32_t, uint32_t, void*);
            ((LadderFn)env->callbackTable[10])((const char*)i + 0x0D, (int32_t)rd32(p, 0x04),
                                               level, exp, 0, rd32(i, 0x3D4), (uint8_t*)i + 0x190);
        }
    }
}

// 1.13c D2Game+0x4AE90
// CLIENTS_FillCharacterPreviewInfo: encode the 14-bit version, resolve the client's player (the
// 1.13c body inlines the +0x3D4/+0x174/0xE03A0 lookup), copy the 11 component ids and colours
// (zero colours become 0xFF), then write class+1 (or 1), level (1 when 0 or >99), the encoded
// client flags (hardcore-dead sets bit 8 when the anim mode is 0 or 0x11), the guild flags, the
// three guild emblem bytes (0 becomes 0xFF) and the 4-byte guild tag. When the level/class are
// invalid, any of flags 0x13 is set, or the encoded string is not 33 bytes, the version byte is
// cleared. Returns without touching the preview when the client has no player.
void CLIENTS_FillCharacterPreviewInfo(void* client, uint8_t* preview, const Batch5Env* env)
{
    env->encode14(preview, 10);                       // nVersion
    if (client == nullptr) fatal(0x130);              // D2_ASSERT(pClient)

    void* player = env->getPlayerFromClient(client, 0);
    if (player == nullptr) return;

    uint8_t components[16], colors[16];
    std::memset(components, 0xFF, sizeof components);
    std::memset(colors, 0xFF, sizeof colors);
    env->getItemSaveGfxInfo(player, components, colors);   // import #10457
    for (int32_t i = 0; i < 16; ++i) {
        if (colors[i] == 0) colors[i] = 0xFF;
    }
    std::memcpy(preview + 0x02, components, 11);      // pComponents
    std::memcpy(preview + 0x0E, colors, 11);          // pComponentColors

    const int32_t classId = (int32_t)rd32(player, 0x04);   // dwClassId
    preview[0x0D] = (uint8_t)(classId < 7 ? classId + 1 : 1);

    const uint32_t level = (uint32_t)env->unitGetBaseStat(player, 0x0C, 0);   // STAT_LEVEL
    preview[0x19] = (uint8_t)((level != 0 && level <= 99) ? level : 1);

    uint16_t flags = rd16(client, 0x0A);              // D2PackedClientSaveFlags
    const uint32_t animMode = rd32(player, 0x10);     // dwAnimMode
    if ((animMode == 0x11 || animMode == 0) && (flags & 0x04) != 0) flags |= 0x08;
    else flags &= (uint16_t)~0x08;
    env->encode14(preview + 0x1A, flags);             // nClientFlags
    env->encode14(preview + 0x1C, rd16(client, 0x45E)); // nGuildFlags

    const uint8_t bg = rd8(client, 0x480);
    const uint8_t fg = rd8(client, 0x481);
    const uint8_t type = rd8(client, 0x482);
    preview[0x1E] = bg ? bg : 0xFF;
    preview[0x1F] = fg ? fg : 0xFF;
    preview[0x20] = type ? type : 0xFF;
    wr32(preview, 0x21, rd32(client, 0x460));         // szGuildTag

    if (level == 0 || level > 99 || classId >= 7 || (flags & 0x13) != 0 ||
        env->strLen((const char*)preview) != 33) {
        preview[0] = 0;
    }
}

// 1.13c D2Game+0x9C840
// PARTY_LeaveParty: find the unit's party id (0x9BEB0); remove the node whose GUID is unit+0x0C
// from the party's list (node head party+0x04, next node+0x04), marking the unit and sending the
// 0x8D packet; when fewer than two nodes remain, destroy the party (0x9C7D0).
void PARTY_LeaveParty_113c(void* unit, const Batch5Env* env)
{
    if (unit == nullptr) fatal(0x130);                 // D2_ASSERT(pUnit)
    const int16_t partyId = env->getPartyIdForUnitOwner(unit);   // 0x9BEB0
    if (partyId == (int16_t)-1) return;

    void* party = env->getPartyById(partyId);          // 0x9BD00
    if (party == nullptr) fatal(0x138);                // D2_ASSERT(pParty)

    const uint32_t unitGuid = rd32(unit, 0x0C);
    void* previous = nullptr;
    for (void* node = rdptr(party, 0x04); node != nullptr; node = rdptr(node, 0x04)) {
        if (rd32(node, 0x00) == unitGuid) {
            if (previous == nullptr) wrptr(party, 0x04, rdptr(node, 0x04));
            else wrptr(previous, 0x04, rdptr(node, 0x04));
            env->unitUpdate(unit);                     // 0xDFC10
            env->sendPacket8D(unit);                   // 0x8C570
            break;
        }
        previous = node;
    }

    if (env->countPartyNodes(partyId) < 2) env->destroyParty(partyId);
}

// 1.13c D2Game+0x9D550
// Release the units registered for the unit in EAX: import #10383 returns the list (count in the
// register pair), then import #10144 is applied to each entry, in order. Called from client
// removal (0x4B7F0). partial: the list's element type is not recovered; the loop and order are.
void CLIENTS_ReleaseClientUnits_113c(uint32_t unitEax, const Batch5Env* env)
{
    if (unitEax == 0) fatal(0x14E);
    void* list = nullptr;
    const uint32_t count = env->unitResetLightMapList((void*)(uintptr_t)unitEax, &list);
    for (uint32_t i = 0; i < count; ++i) {
        env->setUnitAliveCollision(((void**)list)[i]);
    }
}

// 1.13c D2Game+0x53E50
// D2GAME_SAVE_AttachSaveFile: free the client's pending save header (0x49630), build the player
// save into a 0x2000-byte buffer (0xEC7F0), assert the size is positive (fatal 0xB27), attach the
// buffer to the client (0x4A190), set client flag bit 4, and arm the chunked send at +0x184 = 5,
// +0x188 = 0.
void D2GAME_SAVE_AttachSaveFile_113c(void* client, void* player, void* game, uint32_t bInteracts,
                                     const Batch5Env* env)
{
    if (client == nullptr) fatal(0xB41);
    if (rdptr(client, 0x17C) != nullptr) env->freeSaveHeader(client);

    uint8_t buffer[0x2000];
    uint32_t size = 0;
    env->createSaveFile(game, player, buffer, &size, 0x2000, bInteracts, 0);
    if ((int32_t)size <= 0) fatal(0xB27);              // D2_ASSERT(nFileSize > 0)
    env->attachSaveFile(client, buffer, size);
    wr32(client, 0x3D4, rd32(client, 0x3D4) | 0x10);
    wr32(client, 0x184, 5);
    wr32(client, 0x188, 0);
}

// 1.13c D2Game+0x53F10
// D2GAME_SAVE_WriteFileToDisk: when file writing is enabled (0x6FD2D8BC), build the player save
// into a 0x2000-byte buffer (0xEC7F0), format "<savePath><name>.d2s" from FOG_GetSavePath and
// write it when the size is positive; a failed fopen traces "Unable to open player save file %s".
void D2GAME_SAVE_WriteFileToDisk_113c(void* player, void* game, const char* name, uint32_t arg,
                                      const Batch5Env* env)
{
    if (g_batch5_writeSaveFile == 0) return;

    char directory[1024] = {};
    env->getSavePath(directory, 1024);

    char fileName[500] = {};
    std::snprintf(fileName, sizeof fileName, "%s%s.d2s", directory, name);

    uint8_t buffer[0x2000];
    uint32_t size = 0;
    env->createSaveFile(game, player, buffer, &size, 0x2000, arg, 0);
    if ((int32_t)size <= 0) return;

    void* file = env->fileOpen(fileName, "wb");
    if (file != nullptr) {
        env->fileWrite(buffer, size, file);
        env->fileClose(file);
        return;
    }
    env->trace("Unable to open player save file %s", fileName);
}

// 1.13c D2Game+0x54000
// D2GAME_SAVE_WriteFileOnRealm: build the save into a 0x2002-byte buffer whose first word is the
// size+2 (0xEC7F0). Compare the freshly calculated checksum (0x53170) with the client's (+0x18C).
// When they match and the old save header (0x49790) has the same size (0x49760), zero the four
// version-91+ fields on both sides, compare the bytes, restore the fields, and on an identical
// save count it, call the relock callback (+0x38), store the client info and return 0. Otherwise
// store the client info and checksum, call the save callback (+0xC) and return 1.
int32_t D2GAME_SAVE_WriteFileOnRealm_113c(void* game, void* player, const char* name,
                                          const char* account, uint32_t bInteracts, uint32_t token,
                                          uint32_t arg, void* clientInfo, const Batch5Env* env)
{
    if (g_batch5_writeSaveFile == 0) return 0;

    uint8_t buffer[0x2002];
    uint32_t size = 0;
    env->createSaveFile(game, player, buffer + 2, &size, 0x2000, bInteracts, arg);
    if ((int32_t)size < 1) return 0;
    size += 2;
    wr16(buffer, 0, (uint16_t)size);                   // stream length prefix

    void* client = env->getClientFromPlayer(player);   // import #10920
    if (client == nullptr) fatal(0x7EB);
    const uint32_t oldChecksum = rd32(client, 0x18C);

    void* oldSave = env->getSaveHeader(client);        // 0x49790
    const uint32_t oldSize = env->getSaveHeaderSize(client);   // 0x49760
    if (oldSave == nullptr) fatal(0xA3B);              // D2_ASSERT(pOldSaveData)

    const uint32_t newChecksum = (uint32_t)env->calculateChecksum(buffer + 2, size - 2);
    bool same = true;
    if (newChecksum == oldChecksum) {
        if (oldSize == size - 2) {
            const uint32_t oldVersion = rd32(oldSave, 0x04);
            uint32_t oldF[4] = {};
            if (oldVersion > 91) {
                oldF[0] = rd32(oldSave, 0x0C); oldF[1] = rd32(oldSave, 0x30);
                oldF[2] = rd32(oldSave, 0x34); oldF[3] = rd32(oldSave, 0xAB);
                wr32(oldSave, 0x0C, 0); wr32(oldSave, 0x30, 0);
                wr32(oldSave, 0x34, 0); wr32(oldSave, 0xAB, 0);
            }
            const uint32_t newVersion = rd32(buffer, 6);   // buffer+2+4
            uint32_t newF[4] = {};
            if (newVersion > 91) {
                newF[0] = rd32(buffer, 14); newF[1] = rd32(buffer, 50);
                newF[2] = rd32(buffer, 54); newF[3] = rd32(buffer, 173);
                wr32(buffer, 14, 0); wr32(buffer, 50, 0);
                wr32(buffer, 54, 0); wr32(buffer, 173, 0);
            }
            for (uint32_t i = 0; i < oldSize; ++i) {
                if (((const uint8_t*)oldSave)[i] != buffer[i + 2]) { same = false; break; }
            }
            if (oldVersion > 91) {
                wr32(oldSave, 0x0C, oldF[0]); wr32(oldSave, 0x30, oldF[1]);
                wr32(oldSave, 0x34, oldF[2]); wr32(oldSave, 0xAB, oldF[3]);
            }
            if (newVersion > 91) {
                wr32(buffer, 14, newF[0]); wr32(buffer, 50, newF[1]);
                wr32(buffer, 54, newF[2]); wr32(buffer, 173, newF[3]);
            }
            if (same) {
                ++g_batch5_sameSaveCount;
                if (env->callbackTable != nullptr && env->callbackTable[14 /* +0x38 pfRelockDatabaseCharacter */] != nullptr) {
                    typedef int32_t (*RelockFn)(void*, const char*, const char*);
                    ((RelockFn)env->callbackTable[14])(clientInfo, name, account);
                }
                env->setClientInfo(client, clientInfo);
                return 0;
            }
        }
        // checksums matched but the bytes differ: fall through to the save callback
    }

    env->setClientInfo(client, clientInfo);
    env->setSaveChecksum(client, newChecksum);
    if (env->callbackTable != nullptr && env->callbackTable[3 /* +0x0C pfSaveDatabaseCharacter */] != nullptr) {
        if (env->isBadCodePtr(env->callbackTable[3]) != 0) fatal(0xAD1);
        typedef int32_t (*SaveDbFn)(void*, const char*, const char*, const void*, uint32_t, uint32_t);
        ((SaveDbFn)env->callbackTable[3])(clientInfo, name, account, buffer, size, token);
    }
    return 1;
}

// 1.13c D2Game+0x2A740
// GAME_VerifyJoinGame (1.13c split): reject a non-NUL-terminated/over-15 name, locale >= 13, a
// client already in that game (0x498B0/0x497C0), an unknown game id (0x2A3B0), a class >= 7, or a
// name failing 0x2A110. Without a callback table this is the whole check (return 1); otherwise the
// token callback (+0x18) decides, after the IsBadCodePtr(0x714) guard.
int32_t GAME_VerifyJoinGame_113c(int32_t clientId, uint16_t gameId, uint8_t playerClass,
                                 const char* clientName, int32_t tokenId, char* accountName,
                                 int32_t* outCharSaveTransactionToken, uint8_t locale,
                                 int32_t* a9, int32_t* a10, const Batch5Env* env)
{
    if (!nameFits15(clientName)) return 0;
    if (locale >= 13) return 0;

    uint8_t gameName[16] = {};
    if (!env->clientGameName((uint32_t)clientId, gameName)) return 0;          // 0x498B0
    if (!env->clientNameMatchesGameName(clientName, gameName)) return 0;       // 0x497C0
    if (!env->getGameGuidFromGameId(gameId)) return 0;                         // 0x2A3B0
    if (playerClass >= 7) return 0;
    if (!env->isClientNameValid(clientName)) return 0;                         // 0x2A110

    if (env->callbackTable == nullptr) return 1;
    if (env->isBadCodePtr(env->callbackTable[6 /* +0x18 pfFindPlayerToken */]) != 0) fatal(0x714);
    typedef int32_t (*TokenFn)(const char*, int32_t, uint16_t, char*, int32_t*, int32_t*, int32_t*);
    return ((TokenFn)env->callbackTable[6])(clientName, tokenId, gameId, accountName,
                                            outCharSaveTransactionToken, a9, a10) ? 1 : 0;
}

// 1.13c D2Game+0x2A850
// Validate a join request record (base in EAX): refuse when the callback table is set and record
// byte +0x11 is nonzero; require the name at +0x15 and the account at +0x01 to be at most 15
// bytes, the locale byte +0x2D <= 0x0D, and flags at +0x27 to have one of bits 1-2. It then
// re-checks the client's game name (0x498B0/0x497C0) and returns class byte +0x12 < 7.
// partial: which field is class/locale rests on the bounds and the 1.10f GAME_VerifyJoinGame shape.
bool GAME_ValidateJoinRequest_113c(uint32_t clientId, const uint8_t* record, const Batch5Env* env)
{
    const uint8_t classId = record[0x12];
    const uint32_t flags = rd32(record, 0x27);

    if (env->callbackTable != nullptr && record[0x11] != 0) return false;
    if (!nameFits15((const char*)record + 0x15)) return false;
    if (record[0x2D] > 0x0D) return false;
    if (!nameFits15((const char*)record + 0x01)) return false;
    if ((flags & 6) == 0) return false;

    uint8_t gameName[16] = {};
    if (!env->clientGameName(clientId, gameName)) return false;
    if (!env->clientNameMatchesGameName((const char*)record + 0x01, gameName)) return false;
    return classId < 7;
}

// 1.13c D2Game+0xF44B0
// __alldiv: Visual Studio's 64-bit signed divide. Negate the dividend and divisor in place, run the
// shift/subtract long division, correct the quotient by one when the multiply-back overshoots, and
// negate the quotient when exactly one operand was negative.
int64_t d2game_alldiv(uint32_t dividendLow, uint32_t dividendHigh,
                      uint32_t divisorLow, uint32_t divisorHigh)
{
    bool negative = (int32_t)dividendHigh < 0;
    if (negative) {
        const bool borrow = dividendLow != 0;
        dividendLow = 0u - dividendLow;
        dividendHigh = 0u - (uint32_t)borrow - dividendHigh;
    }
    if ((int32_t)divisorHigh < 0) {
        negative = !negative;
        const bool borrow = divisorLow != 0;
        divisorLow = 0u - divisorLow;
        divisorHigh = 0u - (uint32_t)borrow - divisorHigh;
    }

    uint32_t qLow, qHigh = 0;
    if (divisorHigh == 0) {
        qHigh = dividendHigh / divisorLow;
        qLow = (uint32_t)((((uint64_t)dividendHigh % divisorLow) << 32 | dividendLow) / divisorLow);
    } else {
        uint32_t a = dividendLow, b = dividendHigh, c = divisorLow, d = divisorHigh;
        for (;;) {
            const uint32_t nextD = d >> 1;
            c = (c >> 1) | (uint32_t)((d & 1) != 0) << 31;
            b = (b >> 1) | (uint32_t)((a & 1) != 0) << 31;
            a >>= 1;
            d = nextD;
            if (d == 0) break;
        }
        uint64_t q = (((uint64_t)b << 32) | a) / c;
        qLow = (uint32_t)q;
        int32_t qi = (int32_t)qLow;
        uint64_t product = (uint64_t)divisorLow * qLow;
        uint32_t highPart = (uint32_t)(product >> 32);
        uint32_t sum = highPart + (uint32_t)qi * divisorHigh;
        const bool carry = sum < highPart;
        if (carry || dividendHigh < sum ||
            (dividendHigh == sum && dividendLow < (uint32_t)product)) {
            qi -= 1;
            qLow = (uint32_t)qi;
        }
    }

    int64_t result = ((int64_t)(uint64_t)((uint64_t)qHigh << 32 | qLow));
    return negative ? -result : result;
}
