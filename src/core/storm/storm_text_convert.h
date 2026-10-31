// Storm text conversion helpers: UTF-8/UTF-16 unit counting and byte-to-UTF-16 table copy.
#pragma once
#include <cstdint>

// 1.13c Storm+0x356A0: scan up to nLimit UTF-8 bytes; returns UTF-16 units incl. the NUL, else a negative status.
int32_t Storm_Utf8CountUnits(const uint8_t* pSrc, uint32_t nLimit, uint32_t* pnConsumed);
// 1.13c Storm+0x35990: scan up to nLimit UTF-16 units; returns UTF-8 bytes incl. the NUL, else -1.
int32_t Storm_Utf16CountUtf8Bytes(const uint16_t* pSrc, uint32_t nLimit, uint32_t* pnConsumed);
// 1.13c Storm+0x35A90 (code page 437) and 1.13c Storm+0x35AF0 (Mac OS Roman, three retail entries): units written.
uint32_t Storm_ByteToWideCopyTableA(uint16_t* pDst, const uint8_t* pSrc, uint32_t nCap);
uint32_t Storm_ByteToWideCopyTableB(uint16_t* pDst, const uint8_t* pSrc, uint32_t nCap);
