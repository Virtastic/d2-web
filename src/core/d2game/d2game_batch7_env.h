// src/core/d2game/d2game_batch7_env.h -- the host seam struct of d2game_batch_2dc70.cpp, shared with the FN probes (src/core/fn/d2game_2dc70.cpp, _2e360.cpp).
#pragma once
#include <cstdint>
// Host seams for the callees the cached decompiles name but src/ does not implement.
struct D2GameBatch7Env {
    // 0x2AAE0 GAME_GetGameByGUID: the decompiles pass the GUID/client key in ESI/ECX.
    void*    (*getGame)(uint32_t key);
    // 0x49FE0 CLIENTS_GetClientFromClientId(game, clientId): game in EAX, id in the argument.
    void*    (*findClientInGame)(void* game, uint32_t clientId);
    // 0x29BA0 GAME_LeaveGamesCriticalSection(game); 0x2C830 client sweep; 0x2D500 send-queue pump
    // (game argument, client in EAX).
    void     (*unlockGame)(void* game);
    void     (*clientSweep)(void* game);
    void     (*pumpClientQueue)(void* client, void* game, uint32_t flag);   // 0x2D500: client in EAX, then game and 0
    // 0x8A3E0 appends len bytes to the client's SCmd block list (client in EAX).
    void     (*queuePacket)(void* client, const void* data, uint32_t len);
    // 0x2D790 client flush (client in ESI, game in EDI).
    void     (*flushClient)(void* client, void* game);
    // 0x4B7F0 CLIENTS_RemoveClientFromGame(game, clientId, flag).
    void     (*removeClient)(void* game, uint32_t clientId, uint32_t flag);
    // 0xCBDB0 broadcast of the 0x5A leave packet (packet in EAX, game on the stack).
    void     (*terminateName)(const void* packet, void* game);
    // 0x2BF90 (game in EAX, client in EDI) and 0x4B500 (client, game, two flags).
    void     (*prepareClientState)(void* game, void* client);
    void     (*addClientToGame)(void* client, void* game, uint32_t a3, uint32_t a4);
    // 0x4BCC0 (client in EAX, game, then 1, 0, 0): 0, or the rejection code sent as {0xB4, code LE}.
    uint32_t (*buildRejectPacket)(void* client, void* game, uint32_t a2, uint32_t a3, uint32_t a4);
    // 0x4AD10 CLIENTS_GetPlayerFromClient(client, ignoreDeath).
    void*    (*getPlayerFromClient)(void* client, int32_t ignoreDeath);
    // 0x4AD80 (client in EBX) and 0x8A2A0 (client in the stack argument).
    void     (*refreshUnitList)(void* client, uint32_t count);
    void     (*sendUnitsUpdate)(void* client);
    // 0xE03A0 SUNIT_GetServerUnit(game in ECX, type in EDX, guid on the stack).
    void*    (*getServerUnit)(void* game, uint32_t unitType, uint32_t unitGuid);
    // 0xCC030 unit update dispatch (unit in EAX).
    int32_t  (*unitUpdateDispatch)(void* unit);
    // 0x8A730 D2GAME_PACKETS_SendPacket_0x8F (client in EAX).
    void     (*sendPacket8F)(void* client);
    // Storm #509 SStrCmp(a, b, n); Storm #501 SStrCopy(dst, src, n).
    int32_t  (*strCmp)(const void* a, const void* b, uint32_t n);
    void     (*strCopy)(void* dst, const void* src, uint32_t n);
    // D2Net #10002 send(channel, clientId, data, len); the other three are D2Net ordinals 10015,
    // 10000 and 10001, whose register-only arguments are lost in the decompiles.
    int32_t  (*send)(uint32_t channel, uint32_t clientId, const void* data, uint32_t len);
    uint32_t (*net10015)(uint32_t clientId);                         // 0xA364: the client's game GUID, which 0x2AAE0 takes
    void     (*net10000)(uint32_t clientId);                         // 0xA37C (stack: the id)
    void     (*net10001)(uint32_t clientId, uint32_t file, uint32_t line);   // 0xA358 (id, source-file string 0x6FD1CD98, 0x633)
    uint32_t (*tick)();                                              // GetTickCount (0x4A780); null = the host clock
    void (*enterClientList)();                                       // 0xA328 lock thunk record (0x4A780); null = none
};
