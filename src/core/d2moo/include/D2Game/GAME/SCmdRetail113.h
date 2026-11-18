#pragma once
// 1.13c D2Game server->client packet writers that have no 1.10f D2MOO counterpart (each builds one packet in a local buffer and
// hands it to D2GAME_PACKETS_SendPacket_6FC3C710, the 0x8A3E0 send queue). The client rides in EAX or on the stack in retail;
// here it is the first argument. Bytes the retail frame never writes go out as zero.
#include <cstdint>

#include "GAME/SCmd.h"

// 1.13c D2Game+0x8C3E0: {al, cl, edx, word@6, dword@8}, 12 bytes.
void D2Retail113_SendPacket_8C3E0(D2ClientStrc* pClient, uint8_t al, uint8_t cl, uint32_t edx, uint16_t w6, uint32_t d8);
// 1.13c D2Game+0x8C4F0: 53-byte 0xB2 packet delivered through D2NET_10006 (0xA35E).
void D2Retail113_SendPacket_8C4F0(uint32_t nStackArg, const uint8_t* pSrc, uint16_t cx, uint16_t ax);
// 1.13c D2Game+0x8A810 / +0x8A8E0: opcode 0x88 / 0x83, name with NUL at 1, p3 at 5, len at 7, p4 at 9, payload at 11.
void D2Retail113_SendPacket_8A810(D2ClientStrc* pClient, const char* szName, uint16_t nLen, uint16_t p3, uint16_t p4, const void* pPayload);
void D2Retail113_SendPacket_8A8E0(D2ClientStrc* pClient, const char* szName, uint16_t nLen, uint16_t p3, uint16_t p4, const void* pPayload);
// 1.13c D2Game+0x8A890: opcode 0x84, byte, then nCount bytes.
void D2Retail113_SendPacket_8A890(D2ClientStrc* pClient, uint8_t b1, const void* pSrc, uint16_t nCount);
// 1.13c D2Game+0x8AB50: opcode 0x57, 14 bytes.
void D2Retail113_SendPacket_8AB50(D2ClientStrc* pClient, uint32_t eaxIn, uint8_t cl, uint16_t dx, uint16_t p4, uint16_t p5, uint16_t p6);
// 1.13c D2Game+0x8ABF0: copies a 15-byte record and sends it.
void D2Retail113_SendPacket_8ABF0(D2ClientStrc* pClient, const void* pRecord);
// 1.13c D2Game+0x8AD80: opcode 0x4D (eax == 0) or 0x9A, 17 bytes.
void D2Retail113_SendPacket_8AD80(D2ClientStrc* pClient, uint32_t eax, uint32_t p1, uint32_t p2, uint16_t p4, uint16_t p5, uint32_t p6, uint16_t p7, uint8_t p8);
// 1.13c D2Game+0x8AF60: opcode 0x40, 13 bytes; a null unit sends the id -1.
void D2Retail113_SendPacket_8AF60(D2ClientStrc* pClient, const D2UnitStrc* pUnit, uint32_t p2, uint32_t p4);
// 1.13c D2Game+0x8B360: opcode 0x5C, 5 bytes.
void D2Retail113_SendPacket_8B360(D2ClientStrc* pClient, uint32_t eax);
// 1.13c D2Game+0x8B540: opcode 0x93, 8 bytes.
void D2Retail113_SendPacket_8B540(D2ClientStrc* pClient, uint32_t eax, uint8_t p2, uint8_t p4, int32_t p5, uint8_t p6);
