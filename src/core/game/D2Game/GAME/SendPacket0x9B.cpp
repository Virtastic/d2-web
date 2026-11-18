#include "GAME/SCmd.h"
#include <D2PacketDef.h>
#include <test_seam.h>

// 1.13c D2Game+0x8A690
// ECX = the dword at bytes 3..6 (the revive cost), AX = the word at bytes 1..2 (the merc name string id), client on the stack. 0x9B, 7 bytes, queued to the client.
void __fastcall D2GAME_PACKETS_SendPacket0x9B_6FC3FB30(D2ClientStrc* pClient, int16_t a2, int32_t a3)
{
    D2GSPacketSrv9B packet9B = {};

    packet9B.nHeader = 0x9B;
    packet9B.unk0x01 = (uint16_t)a2;
    packet9B.unk0x03 = (uint32_t)a3;

    D2_CALL(D2GAME_PACKETS_SendPacket_6FC3C710, pClient, &packet9B, sizeof(packet9B));
}
