#include "GAME/SCmdRetail113.h"
#include <test_seam.h>
#include <Server.h>

#include <cstring>
#include <vector>

// 1.13c server->client packet writers; see the header. Written from the 1.13c decompile (frame layouts: the lowest local is
// byte 0, local_NN sits at byte base - NN).

namespace
{
inline void W16(uint8_t* b, uint32_t o, uint16_t v) { std::memcpy(b + o, &v, 2); }
inline void W32(uint8_t* b, uint32_t o, uint32_t v) { std::memcpy(b + o, &v, 4); }
inline void Send(D2ClientStrc* pClient, void* pBuf, uint32_t nLen) { D2_CALL(D2GAME_PACKETS_SendPacket_6FC3C710, pClient, pBuf, (int32_t)nLen); }

// 0x8A810 and 0x8A8E0 share one frame: opcode, the name copied unbounded with its NUL, p3, len, p4, payload.
void SendNamed(D2ClientStrc* pClient, uint8_t nOpcode, const char* szName, uint16_t nLen, uint16_t p3, uint16_t p4, const void* pPayload)
{
	const size_t nName = std::strlen(szName) + 1;
	std::vector<uint8_t> b((size_t)nLen + 11 > 1 + nName ? (size_t)nLen + 11 : 1 + nName);
	b[0] = nOpcode;
	std::memcpy(b.data() + 1, szName, nName);
	W16(b.data(), 5, p3);
	W16(b.data(), 7, nLen);
	W16(b.data(), 9, p4);
	if (nLen) std::memcpy(b.data() + 11, pPayload, nLen);
	Send(pClient, b.data(), (uint32_t)nLen + 11);
}
}

// 1.13c D2Game+0x8C3E0
void D2Retail113_SendPacket_8C3E0(D2ClientStrc* pClient, uint8_t al, uint8_t cl, uint32_t edx, uint16_t w6, uint32_t d8)
{
	uint8_t b[12];
	b[0] = al;
	b[1] = cl;
	W32(b, 2, edx);
	W16(b, 6, w6);
	W32(b, 8, d8);
	Send(pClient, b, 12);
}

// 1.13c D2Game+0x8C4F0
void D2Retail113_SendPacket_8C4F0(uint32_t nStackArg, const uint8_t* pSrc, uint16_t cx, uint16_t ax)
{
	uint8_t b[0x35] = { 0xB2 };
	uint32_t n = 0;
	while (n < 15 && pSrc[n]) { b[1 + n] = pSrc[n]; ++n; }
	b[1 + n] = 0;
	W16(b, 49, cx);
	W16(b, 51, ax);
	D2_CALL(D2NET_10006, (int8_t)0, (int32_t)nStackArg, (void*)b, 0x35u);
}

// 1.13c D2Game+0x8A810
void D2Retail113_SendPacket_8A810(D2ClientStrc* pClient, const char* szName, uint16_t nLen, uint16_t p3, uint16_t p4, const void* pPayload)
{
	SendNamed(pClient, 0x88, szName, nLen, p3, p4, pPayload);
}

// 1.13c D2Game+0x8A8E0
void D2Retail113_SendPacket_8A8E0(D2ClientStrc* pClient, const char* szName, uint16_t nLen, uint16_t p3, uint16_t p4, const void* pPayload)
{
	SendNamed(pClient, 0x83, szName, nLen, p3, p4, pPayload);
}

// 1.13c D2Game+0x8A890
void D2Retail113_SendPacket_8A890(D2ClientStrc* pClient, uint8_t b1, const void* pSrc, uint16_t nCount)
{
	std::vector<uint8_t> b((size_t)nCount + 2, 0);
	b[0] = 0x84;
	b[1] = b1;
	if (nCount) std::memcpy(b.data() + 2, pSrc, nCount);
	Send(pClient, b.data(), (uint32_t)nCount + 2);
}

// 1.13c D2Game+0x8AB50
void D2Retail113_SendPacket_8AB50(D2ClientStrc* pClient, uint32_t eaxIn, uint8_t cl, uint16_t dx, uint16_t p4, uint16_t p5, uint16_t p6)
{
	uint8_t b[14] = { 0x57 };
	W32(b, 1, eaxIn);
	b[5] = cl;
	W16(b, 6, dx);
	W16(b, 8, p4);
	W16(b, 10, p5);
	W16(b, 12, p6);
	Send(pClient, b, 14);
}

// 1.13c D2Game+0x8ABF0
void D2Retail113_SendPacket_8ABF0(D2ClientStrc* pClient, const void* pRecord)
{
	uint8_t b[15];
	std::memcpy(b, pRecord, 15);
	Send(pClient, b, 15);
}

// 1.13c D2Game+0x8AD80 (the decompile's ternary: a zero flag selects 0x4D)
void D2Retail113_SendPacket_8AD80(D2ClientStrc* pClient, uint32_t eax, uint32_t p1, uint32_t p2, uint16_t p4, uint16_t p5, uint32_t p6, uint16_t p7, uint8_t p8)
{
	uint8_t b[17] = {};
	b[0] = eax == 0 ? 0x4D : 0x9A;
	b[1] = (uint8_t)p1;
	W32(b, 2, p2);
	W32(b, 6, p6);
	b[10] = p8;
	W16(b, 11, p4);
	W16(b, 13, p5);
	W16(b, 15, p7);
	Send(pClient, b, 17);
}

// 1.13c D2Game+0x8AF60
void D2Retail113_SendPacket_8AF60(D2ClientStrc* pClient, const D2UnitStrc* pUnit, uint32_t p2, uint32_t p4)
{
	uint8_t b[13] = {};
	b[0] = 0x40;
	W32(b, 1, pUnit ? pUnit->dwUnitId : 0xFFFFFFFFu);
	W32(b, 5, p2);
	W32(b, 9, p4);
	Send(pClient, b, 13);
}

// 1.13c D2Game+0x8B360
void D2Retail113_SendPacket_8B360(D2ClientStrc* pClient, uint32_t eax)
{
	uint8_t b[5] = { 0x5C };
	W32(b, 1, eax);
	Send(pClient, b, 5);
}

// 1.13c D2Game+0x8B540
void D2Retail113_SendPacket_8B540(D2ClientStrc* pClient, uint32_t eax, uint8_t p2, uint8_t p4, int32_t p5, uint8_t p6)
{
	uint8_t b[8] = { 0x93 };
	W32(b, 1, eax);
	b[5] = p4;
	b[6] = p2;
	b[7] = p5 ? p6 : 4;
	Send(pClient, b, 8);
}
