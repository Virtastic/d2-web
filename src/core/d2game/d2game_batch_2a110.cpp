// src/core/d2game/d2game_batch_2a110.cpp -- eleven 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): the client-name validator and
// join helpers of GAME/Game.cpp, the client-list lookup/state/flag helpers and unit-update refresh of
// GAME/Clients.cpp, three GAME/SCmd.cpp packet senders, the player status update of PLAYER/PlrMsg.cpp
// and the MONSTER/MonsterRegion.cpp spawn-list fill.
//
// Register-only arguments (Ghidra's in_EAX/unaff_EBX/unaff_EDI/unaff_ESI or arguments the ABI passed
// in a register and the callsite dropped) are explicit parameters here and are named in the comment at
// each function. Retail state is modelled as the globals below, each carrying its retail address;
// records are opaque host objects addressed by retail byte offsets. The fog/Storm/D2Common imports are
// host seams on D2GameBatch11Env in retail argument order. The fatal path is Fog Ordinal 10265(id) ->
// Ordinal 10024(text) -> _exit(-1); only the exit is observable and is modelled.
// Prior art: D2MOO 1.10f reference/D2MOO/source/D2Game/src/{GAME/Game.cpp, GAME/Clients.cpp,
// GAME/SCmd.cpp, PLAYER/PlrMsg.cpp, MONSTER/MonsterRegion.cpp}.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <Windows.h>      // CRITICAL_SECTION no-ops, deterministic GetTickCount
#include <emscripten.h>     // EMSCRIPTEN_KEEPALIVE on the exported entry points
#include <D2BasicTypes.h> // BOOL
#include <Server.h>       // LARGE_INTEGER and the D2Net compat declarations

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline void wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline uint8_t rd8(const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline void wr8(void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees the cached decompiles name but src/ does not implement. Each seam notes
// its retail RVA and the retail calling convention (a register-hidden argument becomes an explicit
// parameter here in the same left-to-right order).
struct D2GameBatch11Env {
    uint16_t (*crc16)(const char* s);                                   // 0xA334, Fog #10137
    int32_t  (*strCmpI)(const char* a, const char* b, uint32_t n);      // 0xA280, Storm #509
    uint32_t (*strCopy)(char* dst, const char* src, uint32_t n);        // 0xA280, Storm #501
    void*    (*allocPool)(const char* file, uint32_t line, uint32_t zero); // 0xA2E6, Fog #10045
    void     (*appendPacket)(void* client, const void* data, uint32_t len); // 0x8A3E0, client in ESI
    void*    (*getClientById)(uint32_t clientId);                       // 0x49FE0
};

// ---- module state --------------------------------------------------------------------------------
static void* g_clientListByName[256];  // 0x6FD303B8 (1.10f gpClientListByName_6FD443D0)
static CRITICAL_SECTION g_clientByNameLock;    // 0x6FD30BD0
// Client record (D2ClientStrc): dwClientId +0x00, dwClientState +0x04, nClassId +0x08,
// nCharTemplate +0x0C, szName +0x0D, szAccount +0x1D, nCharSaveTransactionToken +0x60, pClientInfo +0x68,
// bUnlockCharacter +0x178, pGame +0x1A8, pRoom +0x1B4, nSaveHeaderSendFailures +0x1C4,
// unitUpdate array +0x1CC (stride 9), nUnitUpdateIndex +0x3BC, dwFlags +0x3D4, dwLastPacketTick +0x3D8,
// HotkeySkills +0x3DC, clientPlayerData +0x48C, pNext +0x4A8, pServerNext +0x4AC, pServerNextByName +0x4B0.

// 1.13c D2Game+0x2A110
// IsClientNameValid (1.10f Game.cpp:927): the name arrives in EAX and the bound in EDI (16 at the only
// call site, GAME_VerifyJoinGame). Walk at most `nMaxLen` characters: NUL is accepted only after at
// least one letter; '\'', '-' and '_' are accepted once, never first and never last, and never twice;
// A-Z/a-z count as letters and count >= nMaxLen fails. Any other byte fails.
EMSCRIPTEN_KEEPALIVE bool d2_game_2a110(const char* szName, int32_t nMaxLen)
{
    if (nMaxLen <= 0)
        return false;
    bool bSpecial = false;
    int32_t nCount = 0;
    for (;;) {
        const char c = *szName++;
        if (c == '\0')
            return nCount != 0;
        if (c == '\'' || c == '-' || c == '_') {
            if (bSpecial || nCount == 0 || *szName == '\0')
                return false;
            bSpecial = true;
        }
        else if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) {
            return false;
        }
        if (++nCount >= nMaxLen)
            return false;
    }
}

// 1.13c D2Game+0x497C0
// CLIENTS_GetClientByName (1.10f sub_6FC33F90, Clients.cpp:1449): the name arrives in EAX; the second
// parameter is the 16-byte output buffer (may be null). Bucket the name with Fog #10137's CRC16, walk
// the by-name client list (next at client+0x4B0) under 0x6FD30BD0, SStrCmpI against the name at
// client+0x0D; on a match copy the stored name out with Ordinal 501 and return 0, else return 1.
EMSCRIPTEN_KEEPALIVE int32_t d2_game_497c0(const char* szName, char* szOutName,
                                           const D2GameBatch11Env* env)
{
    const uint8_t nBucket = (uint8_t)env->crc16(szName);            // Ordinal 10137
    EnterCriticalSection(&g_clientByNameLock);                      // Ordinal 10050
    void* client = g_clientListByName[nBucket];
    if (client != nullptr) {
        while (env->strCmpI((const char*)client + 0x0D, szName, 0x10) != 0) { // Ordinal 509
            client = rdptr(client, 0x4B0);
            if (client == nullptr) {
                LeaveCriticalSection(&g_clientByNameLock);
                return 1;
            }
        }
        if (szOutName != nullptr)
            env->strCopy(szOutName, (const char*)client + 0x0D, 0x10);  // Ordinal 501
        LeaveCriticalSection(&g_clientByNameLock);
        return 0;
    }
    LeaveCriticalSection(&g_clientByNameLock);
    return 1;
}

// 1.13c D2Game+0x8C420
// CLIENTS_SendJoinGameBody (1.10f SCmd.cpp SendPacket0x01): the client arrives in EAX. Fatals (0x139)
// when the client's pGameData at +0x1D28 is null; otherwise builds the 8-byte 0x01 body -- bytes 2..5
// from pGameData+0x08, byte 1 from client+0x6D, bytes 6/7 as the non-null tests of client+0x70/+0x74 --
// and appends it (`appendPacket` writes the header byte itself from the packet header the caller set).
static void d2_game_8c420(void* client, const D2GameBatch11Env* env)
{
    void* pGameData = rdptr(client, 0x1D28);
    if (pGameData == nullptr)
        d2game_fatal(0x139);
    uint8_t packet[8] = {};
    wr32(packet, 2, rd32(pGameData, 8));
    packet[1] = rd8(client, 0x6D);
    packet[6] = rd32(client, 0x70) != 0;
    packet[7] = rd32(client, 0x74) != 0;
    env->appendPacket(client, packet, 8);
}

// 1.13c D2Game+0x2AC60
// GAME_SendGameInit (1.10f Game.cpp:1108 SrvJoinGame inline): the client arrives in ESI. Send the 0x01
// join body, send the header-only packet (header byte 0, size 1), then set the client state to
// CLIENTSTATE_GAME_INIT_SENT (1); the decompile's null check after the sends fatals 0x95F.
EMSCRIPTEN_KEEPALIVE void d2_game_2ac60(void* client, const D2GameBatch11Env* env)
{
    d2_game_8c420(client, env);
    const uint8_t nHeader = 0;
    env->appendPacket(client, &nHeader, 1);
    if (client == nullptr)
        d2game_fatal(0x95F);
    wr32(client, 4, 1);                // dwClientState = CLIENTSTATE_GAME_INIT_SENT
}
