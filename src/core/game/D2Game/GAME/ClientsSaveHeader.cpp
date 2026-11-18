#include <cstdlib>
#include <cstring>
#include <Fog.h>

#include "GAME/Clients.h"
#include "GAME/Game.h"

// 1.13c D2Game+0x49630 (esi = client)
// Frees the held save header through the game's memory pool and resets the header size, sent-bytes and the +0x184 word; clears flag 0x10.
void __fastcall CLIENTS_FreeSaveHeader(D2ClientStrc* pClient)
{
	D2_ASSERT(pClient);

	if (pClient->pSaveHeader)
	{
		D2_FREE_POOL(pClient->pGame->pMemoryPool, pClient->pSaveHeader);
		pClient->pSaveHeader = nullptr;
	}

	pClient->nSaveHeaderSize = 0;
	pClient->nSaveHeaderDataSentBytes = 0;
	pClient->unk0x184 = 0;
	pClient->dwFlags &= ~0x10u;
}

// 1.13c D2Game+0x496B0 (ecx = client, eax = size; stack = source)
// A different size, or a header already held, replaces the header with a fresh allocation of `nSize` bytes; then copies the data in.
void __fastcall CLIENTS_CopySaveDataToClient(D2ClientStrc* pClient, const void* pData, uint32_t nSize)
{
	D2_ASSERT(pClient);

	if ((uint32_t)pClient->nSaveHeaderSize != nSize || pClient->pSaveHeader)
	{
		if (pClient->pSaveHeader)
		{
			D2_FREE_POOL(pClient->pGame->pMemoryPool, pClient->pSaveHeader);
		}
		pClient->pSaveHeader = (D2SaveHeaderStrc*)D2_ALLOC_POOL(pClient->pGame->pMemoryPool, nSize);
		pClient->nSaveHeaderSize = (int32_t)nSize;
	}

	memcpy(pClient->pSaveHeader, pData, nSize);
}

// 1.13c D2Game+0x49760 (eax = client)
uint32_t __fastcall CLIENTS_GetSaveHeaderSize(D2ClientStrc* pClient)
{
	D2_ASSERT(pClient);
	return pClient->nSaveHeaderSize;
}

// 1.13c D2Game+0x49790 (eax = client)
D2SaveHeaderStrc* __fastcall CLIENTS_GetSaveHeader(D2ClientStrc* pClient)
{
	D2_ASSERT(pClient);
	return pClient->pSaveHeader;
}
