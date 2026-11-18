#pragma once

#include <cstdint>

// The MSVC 2003 CRT printf output helpers D2Game.dll carries (1.13c D2Game+0x4352 and +0x43A9), over the retail 32-bit FILE layout.
#pragma pack(1)
struct D2CrtFile32
{
	uint32_t pPtr;			//0x00 next byte of the buffer (a retail address)
	int32_t nCnt;			//0x04 bytes left in the buffer
	uint32_t pBase;			//0x08 buffer base
	uint32_t dwFlag;		//0x0C 0x40 = string stream
};
#pragma pack()

// __flsbuf (1.13c D2Game+0x4239, sub_4239): flushes the stream and stores the byte; returns the byte or -1.
typedef uint32_t(* D2CrtFlushFn)(int32_t nChar, D2CrtFile32* pFile);

// 1.13c D2Game+0x4352 _write_char: AL = byte (nChar), ECX = pFile, ESI = pCount (the running count; -1 on error).
void CRT_WriteChar(uint8_t nChar, D2CrtFile32* pFile, int32_t* pCount, D2CrtFlushFn pfFlush);
// 1.13c D2Game+0x43A9 _write_string: EAX = pCount, ECX = pText, EDI = pFile, stack = nLen.
void CRT_WriteString(int32_t* pCount, const uint8_t* pText, D2CrtFile32* pFile, int32_t nLen, D2CrtFlushFn pfFlush);
// 1.13c D2Game+0x4385 _write_multi_char: EAX = pCount; writes nChar nNum times, stopping once the count reads -1.
void CRT_WriteMultiChar(uint8_t nChar, int32_t nNum, D2CrtFile32* pFile, int32_t* pCount, D2CrtFlushFn pfFlush);
