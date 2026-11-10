// The experience update the per-client vitals pass sends when the player's experience differs from what the client was
// last told.
#include <cstdint>

#include <GAME/Clients.h>
#include <GAME/SCmd.h>

// 1.13c D2Game+0x8BDA0
void __fastcall D2GAME_PACKETS_SendPacket0x1A_B_C_6FC3D410(D2ClientStrc* pClient, int32_t nExperience, int32_t nPreviousExperience)
{
    // A gain below 255 goes as a byte (0x1A), below 65535 as a word (0x1B); a loss or a larger gain as the absolute
    // value (0x1C).
    const int32_t nDiff = nExperience - nPreviousExperience;
    if (nDiff >= 0 && nDiff < 0xFFFF) {
        if (nDiff < 0xFF) {
            uint8_t packet[2] = { 0x1A, (uint8_t)nDiff };
            D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, sizeof(packet));
            return;
        }
        uint8_t packet[3] = { 0x1B, (uint8_t)nDiff, (uint8_t)(nDiff >> 8) };
        D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, sizeof(packet));
        return;
    }
    uint8_t packet[5] = { 0x1C, (uint8_t)nExperience, (uint8_t)(nExperience >> 8), (uint8_t)(nExperience >> 16), (uint8_t)(nExperience >> 24) };
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, packet, sizeof(packet));
}
