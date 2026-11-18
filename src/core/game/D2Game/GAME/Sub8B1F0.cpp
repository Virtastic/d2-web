#include "oc20g11.h"

#include <cstring>
#include <D2PacketDef.h>

#include "GAME/SCmd.h"

// 1.13c D2Game+0x8B1F0 (eax = amount, ecx = side; stack = client)
// Sends server packet 0x79 (6 bytes): the opcode, the side byte and the 32-bit amount.
void __fastcall sub_8B1F0(D2ClientStrc* pClient, int32_t nAmount, char nSide)
{
	uint8_t buffer[8] = {};
	buffer[0] = 0x79;
	buffer[1] = (uint8_t)nSide;
	memcpy(buffer + 2, &nAmount, 4);
	D2GAME_PACKETS_SendPacket_6FC3C710(pClient, buffer, 6);
}

// 1.13c D2Game+0x8B2F0 (eax = source; stack = client)
// Relays the 0x15-byte block at the source verbatim.
void __fastcall sub_8B2F0(const void* pSrc, D2ClientStrc* pClient)
{
	uint8_t buffer[0x15];
	memcpy(buffer, pSrc, sizeof(buffer));
	D2GAME_PACKETS_SendPacket_6FC3C710(pClient, buffer, sizeof(buffer));
}
