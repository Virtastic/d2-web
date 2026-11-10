// 0x01, the game's flags, the first packet a joining client gets: 8 bytes, header, difficulty (u8),
// arena flags (u32), expansion (u8), ladder (u8).
#include <cstdint>
#include <cstring>
#include <GAME/Arena.h>
#include <GAME/Game.h>
#include <GAME/SCmd.h>

void __fastcall D2GAME_PACKETS_SendPacket0x01_6FC3C7C0(D2ClientStrc* pClient, char nHeader, D2GameStrc* pGame)
{
    uint8_t packet[8] = {};
    packet[0] = (uint8_t)nHeader;
    packet[1] = pGame->nDifficulty;
    const uint32_t flags = pGame->pArenaCtrl ? ARENA_GetFlags(pGame) : 0;
    std::memcpy(packet + 2, &flags, 4);
    packet[6] = pGame->bExpansion != 0;
    packet[7] = pGame->dwGameType != 0;
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, sizeof packet);
}
