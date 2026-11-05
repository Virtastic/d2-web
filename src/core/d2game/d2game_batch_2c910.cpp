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
    // D2Common Ordinal 10046(file, line, flag): non-fatal warning (Party.cpp:0x148 after the 0x8D send)
    void     (*displayWarning)(const char* file, int line, int flag);

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
// GAME_TriggerClientSave(client, game): when client has a player (0x4AD10) and its save (0x54320)
// succeeds, walk the game's client list (game+0x88, next +0x4A8). Per client the save unit is
// flag(+0x3D4) bit 0 ? +0x174 : (+0x174 == 0 ? none : E03A0(+0x1A8, +0x170), a failed lookup
// clears +0x174). With a unit: save it (0x54320, name +0x0D, 0) unless it is `client`; then, when
// the callback table slot +0x28 and game+0x74 are set, call it with (name, unit class, level,
// experience, 0, client word +0x0A, client +0x190).
void GAME_TriggerClientSave_113c(void* client, void* game, const Batch5Env* env)
{
    if (client == nullptr) return;
    void* player = env->getPlayerFromClient(client, 0);
    if (player == nullptr) return;
    if (!env->saveWriteFile(game, player, (const char*)client + 0x0D, 1)) return;

    for (void* i = rdptr(game, 0x88); i != nullptr; i = rdptr(i, 0x4A8)) {
        void* unit = rdptr(i, 0x174);
        if ((rd32(i, 0x3D4) & 1) == 0) {
            if (unit == nullptr) continue;
            unit = env->getServerUnit(rd32(i, 0x1A8), rd32(i, 0x170), rd32(i, 0x16C));
            if (unit == nullptr) { wrptr(i, 0x174, nullptr); continue; }
        }
        if (unit == nullptr) continue;
        if (i != client) env->saveWriteFile(game, unit, (const char*)i + 0x0D, 0);
        if (env->callbackTable != nullptr && env->callbackTable[10] != nullptr && rd32(game, 0x74) != 0) {
            // ECX = name, EDX = unit class (+0x04); stack: level, experience, 0, client word +0x0A,
            // client +0x190 (the #10587 calls interleave their pushes with the callback's).
            const int32_t exp = env->unitGetBaseStat(unit, 0x0D, 0);
            const int32_t lvl = env->unitGetBaseStat(unit, 0x0C, 0);
            typedef void (*LadderFn)(const char*, int32_t, int32_t, int32_t, int32_t, uint32_t, void*);
            ((LadderFn)env->callbackTable[10])((const char*)i + 0x0D, (int32_t)rd32(unit, 0x04), lvl, exp, 0,
                rd16(i, 0x0A), (uint8_t*)i + 0x190);
        }
    }
}

// 1.13c D2Game+0x4ae90
// CLIENTS_FillCharacterPreviewInfo: encode the 14-bit version, resolve the client's player (the
// 1.13c body inlines the +0x3D4/+0x174/0xE03A0 lookup), copy the 11 component ids and colours
// (zero colours become 0xFF), then write class+1 (or 1), level (1 when 0 or >99), the encoded
// client flags (hardcore-dead sets bit 8 when the anim mode is 0 or 0x11), the guild flags, the
// three guild emblem bytes (0 becomes 0xFF) and the 4-byte guild tag. When the level/class are
// invalid, any of flags 0x13 is set, or the encoded string is not 33 bytes, the version byte is
// cleared. Returns without touching the preview when the client has no player.
void CLIENTS_FillCharacterPreviewInfo(void* client, uint8_t* preview, const Batch5Env* env)
{
    env->encode14(preview, 0xD);                      // nVersion (1.13c: 0xD)
    if (client == nullptr) fatal(0xFB);               // D2_ASSERT(pClient), line 0xFB in 1.13c

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

    const uint8_t classId = (uint8_t)rd32(player, 0x04);   // dwClassId, byte compare (bl)
    preview[0x0D] = (uint8_t)(classId < 7 ? classId + 1 : 1);

    const uint8_t level = (uint8_t)env->unitGetBaseStat(player, 0x0C, 0);   // STAT_LEVEL, kept as a byte (al)
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
            env->displayWarning("..\\Source\\D2Game\\UNIT\\Party.cpp", 0x148, 0);  // Ordinal 10046
            break;
        }
        previous = node;
    }

    if (env->countPartyNodes(partyId) < 2) env->destroyParty(partyId);
}

// The unit-list release: d2game_9d550.cpp is its own file.

// superseded by d2game_53e50.cpp: 1.13c D2Game+0x53E50
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

// 1.13c D2Game+0x53F10 and +0x54000 moved to d2game_53f10.cpp / d2game_54000.cpp (their own probes).

// 1.13c D2Game+0x2A740
// GAME_VerifyJoinGame (1.13c split): reject a non-NUL-terminated/over-15 name, locale >= 13, a
// client already in that game (0x498B0/0x497C0), an unknown game id (0x2A3B0), a class >= 7, or a
// name failing 0x2A110. Without a callback table this is the whole check (return 1); otherwise the
// token callback (+0x18) decides, after the IsBadCodePtr(0x714) guard.
int32_t GAME_VerifyJoinGame_113c(int32_t clientId, uint16_t gameId, uint8_t playerClass,
                                 const char* clientName, int32_t tokenId, char* accountName,
                                 int32_t* outCharSaveTransactionToken, uint8_t locale,
                                 int32_t* a9, int32_t* a10, int32_t* a11, int32_t* a12,
                                 const Batch5Env* env)
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
    // name/tokenId travel in ECX/EDX (Ghidra drops them); the stack carries gameId..a12.
    typedef int32_t (*TokenFn)(const char*, int32_t, uint16_t, char*, int32_t*, int32_t*, int32_t*,
                               int32_t*, int32_t*);
    return ((TokenFn)env->callbackTable[6])(clientName, tokenId, gameId, accountName,
                                            outCharSaveTransactionToken, a9, a10, a11, a12) ? 1 : 0;
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
    if (!env->clientNameMatchesGameName((const char*)record + 0x15, gameName)) return false;
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
            a = (a >> 1) | (uint32_t)((b & 1) != 0) << 31;
            b >>= 1;
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
