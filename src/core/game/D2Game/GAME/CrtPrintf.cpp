#include "GAME/CrtPrintf.h"

// 1.13c D2Game+0x4352
void CRT_WriteChar(uint8_t nChar, D2CrtFile32* pFile, int32_t* pCount, D2CrtFlushFn pfFlush)
{
	if (!(pFile->dwFlag & 0x40) || pFile->pBase)
	{
		uint32_t nResult;
		if (--pFile->nCnt < 0)
		{
			nResult = pfFlush((int32_t)(int8_t)nChar, pFile);
		}
		else
		{
			*(uint8_t*)(uintptr_t)pFile->pPtr = nChar;
			pFile->pPtr++;
			nResult = nChar;
		}

		if (nResult == 0xFFFFFFFFu)
		{
			*pCount = -1;
			return;
		}
	}

	*pCount += 1;
}

// 1.13c D2Game+0x4385
void CRT_WriteMultiChar(uint8_t nChar, int32_t nNum, D2CrtFile32* pFile, int32_t* pCount, D2CrtFlushFn pfFlush)
{
	do
	{
		if (nNum < 1)
		{
			return;
		}
		--nNum;
		CRT_WriteChar(nChar, pFile, pCount, pfFlush);
	}
	while (*pCount != -1);
}

// 1.13c D2Game+0x43A9
void CRT_WriteString(int32_t* pCount, const uint8_t* pText, D2CrtFile32* pFile, int32_t nLen, D2CrtFlushFn pfFlush)
{
	if (!(pFile->dwFlag & 0x40) || pFile->pBase)
	{
		do
		{
			if (nLen < 1)
			{
				return;
			}
			--nLen;
			CRT_WriteChar(*pText++, pFile, pCount, pfFlush);
		}
		while (*pCount != -1);
	}
	else
	{
		*pCount += nLen;
	}
}
