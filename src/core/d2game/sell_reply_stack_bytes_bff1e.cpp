// 1.13c D2Game+0xBFF1E: bytes 3..6 of the sell success reply (0x2A, action 3, result 1).
// The sell (D2Game+0xBF990) sends its success reply after reading the player's gold with D2Common 10973. That call
// pushes EBX (D2Common+0x38B7B) and then EDI (+0x38B89); the reply composer (D2Game+0x8AC90, `sub esp,0x10`) lays its
// packet over the same slots and never writes bytes 3..6. So byte 3 is EDI's top byte and bytes 4..6 are EBX's low
// three bytes. On the stored-item path EDI is the player unit (`mov edi,ebp`, 0xBFE31) and EBX is the player's
// client struct (PlayerData+0x9C, 0xBFDF9).
// The two addresses are heap state. A recorded session supplies the addresses its retail rig held (like its seed);
// otherwise the engine uses its own player unit and client pointers, the same mechanism on our heap.
#include <cstdint>
#include <emscripten.h>

static uint32_t g_rigPlayer, g_rigClient;
static bool g_rigSet;

extern "C" EMSCRIPTEN_KEEPALIVE void d2_session_rig_addresses(uint32_t player, uint32_t client, int on) {
    g_rigPlayer = player; g_rigClient = client; g_rigSet = on != 0;
}

uint32_t D2GAME_SellReplyStackBytes(const void* pPlayer, const void* pClient) {
    const uint32_t edi = g_rigSet ? g_rigPlayer : (uint32_t)(uintptr_t)pPlayer;
    const uint32_t ebx = g_rigSet ? g_rigClient : (uint32_t)(uintptr_t)pClient;
    return (edi >> 24) | ((ebx & 0xFFFFFFu) << 8);
}
