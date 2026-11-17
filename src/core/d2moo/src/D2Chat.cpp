#include <cstring>

#include <D2Chat.h>
#include <D2Lang.h>
#include <D2StrTable.h>
#include <Fog.h>

// 1.13c D2Common+0x18280
//D2Common.0x6FDC3BF0 (#10892)
D2HoverTextStrc* __stdcall CHAT_AllocHoverMsg(void* pMemPool, const char* szText, int nTimeout)
{
	// Retail measures the text, clamps the copied length to 254, allocates (Chat.cpp line 0x32), stores
	// display time 8 * len + 125 and expiry display + timeout, copies len + 1 bytes of text, writes the
	// terminator at szMsg[len + 1], then the language byte.
	const size_t nTextLength = strlen(szText);
	if (nTextLength == 0)
	{
		return NULL;
	}

	const int nLength = nTextLength < 255 ? (int)nTextLength : 254;

	D2HoverTextStrc* pHoverMsg = D2_CALLOC_STRC_POOL(pMemPool, D2HoverTextStrc);

	const int nDisplayTime = nLength * 8 + 125;
	pHoverMsg->dwDisplayTime = nDisplayTime;
	pHoverMsg->dwExpireTime = nDisplayTime + nTimeout;
	pHoverMsg->bUsed = FALSE;
	strncpy(pHoverMsg->szMsg, szText, nLength + 1);
	pHoverMsg->szMsg[nLength + 1] = '\0';
	pHoverMsg->nLangId = STRTABLE_GetLanguage();

	return pHoverMsg;
}

// 1.13c D2Common+0x18260
//D2Common.0x6FDC3C80 (#10893)
void __stdcall CHAT_FreeHoverMsg(void* pMemPool, D2HoverTextStrc* pHoverMsg)
{
	if (pHoverMsg)
	{
		D2_FREE_POOL(pMemPool, pHoverMsg);
	}
}

// 1.13c D2Common+0x18250
//D2Common.0x6FDC3CA0 (#10894)
uint32_t __stdcall CHAT_GetDisplayTimeFromHoverMsg(D2HoverTextStrc* pHoverMsg)
{
	return pHoverMsg->dwDisplayTime;
}

// 1.13c D2Common+0x18240
//D2Common.0x6FDC3CB0 (#10895)
uint32_t __stdcall CHAT_GetTimeoutFromHoverMsg(D2HoverTextStrc* pHoverMsg)
{
	return pHoverMsg->dwExpireTime;
}

// 1.13c D2Common+0x18220
//D2Common.0x6FDC3CC0 (#10896)
void __stdcall CHAT_CopyHoverMsgToBuffer(D2HoverTextStrc* pHoverMsg, char* szMessage)
{
	int nCounter = 0;

	do
	{
		szMessage[nCounter] = pHoverMsg->szMsg[nCounter];

		++nCounter;
	}
	while (pHoverMsg->szMsg[nCounter - 1]);
}

// 1.13c D2Common+0x18210
//D2Common.0x6FDC3CE0 (#10897)
BOOL __stdcall CHAT_GetUsedFromHoverMsg(D2HoverTextStrc* pHoverMsg)
{
	return pHoverMsg->bUsed;
}

// 1.13c D2Common+0x18200
//D2Common.0x6FDC3CF0 (#10898)
void __stdcall CHAT_SetUsedInHoverMsg(D2HoverTextStrc* pHoverMsg, BOOL bUsed)
{
	pHoverMsg->bUsed = bUsed;
}

// 1.13c D2Common+0x181F0
//D2Common.0x6FDC3D00 (#10899)
uint8_t __stdcall CHAT_GetLangIdFromHoverMsg(D2HoverTextStrc* pHoverMsg)
{
	return pHoverMsg->nLangId;
}

// 1.13c D2Common+0x181E0
//D2Common.0x6FDC3D10 (#10900)
void __stdcall CHAT_SetLangIdInHoverMsg(D2HoverTextStrc* pHoverMsg, uint8_t nLangId)
{
	pHoverMsg->nLangId = nLangId;
}
