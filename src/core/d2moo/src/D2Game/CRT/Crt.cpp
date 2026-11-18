#include "CRT/Crt.h"

#include <cstring>

// 1.13c D2Game+0x3B51: tail fragment into 0x3F76 with lock number 0xC -- __unlock(12), reached from __updatetlocinfo (0x3B1F)
void D2GAME_CRT_Unlock12_3B51()
{
    d2game_unlock_3f76(0xC);
}

// 1.13c D2Game+0x60DE: __unlock(4), the small-block heap bookkeeping tail of __calloc_crt
void D2GAME_CRT_Unlock4_60DE()
{
    d2game_unlock_3f76(4);
}

// 0x6FD3A210: allocator mode; 3 selects the small-block heap.
static int32_t gCrtAllocMode = 0;
// 0x6FD39FC0: largest request the small-block heap serves.
static uint32_t gCrtSbhThreshold = 0;
// 0x6FD300DC: non-zero when a _new_handler is installed.
static int32_t gCrtHasNewHandler = 0;

// 1.13c D2Game+0x6018 (__calloc_crt): void* __cdecl calloc(size_t count, size_t size). count == 0 or size <= 0xFFFFFFE0/count (the
// unsigned overflow guard) yields bytes = count*size, a zero byte count grows to one. Each pass: with the mode 3 small-block heap, a
// 16-byte-rounded request and bytes <= threshold, the block lock (4) is taken, ___sbh_alloc_block(bytes) tried and 0x60DE run; success
// memsets `bytes` and returns. Otherwise (or on failure) HeapAlloc(heap, 8, rounded) is tried. A failure returns null unless a new
// handler is installed, in which case 0x9710(rounded) is called and the pass repeats while it returns non-zero.
void* D2GAME_CRT_Calloc_6018(uint32_t count, uint32_t size, const D2GameCrtCallocEnv* env)
{
    if (count != 0 && size > 0xFFFFFFE0u / count)
    {
        return nullptr;
    }
    const uint32_t bytes = count * size;
    uint32_t dwBytes = bytes != 0 ? bytes : 1;
    for (;;)
    {
        if (dwBytes < 0xFFFFFFE1u)
        {
            void* dst = nullptr;
            if (gCrtAllocMode == 3)
            {
                dwBytes = (dwBytes + 0xFu) & 0xFFFFFFF0u;
                if (bytes <= gCrtSbhThreshold)
                {
                    env->lock(4);
                    dst = env->sbhAllocBlock(bytes);
                    env->sub60DE();
                    if (dst != nullptr)
                    {
                        std::memset(dst, 0, bytes);
                        env->unlock(4);
                        return dst;
                    }
                    env->unlock(4);
                }
            }
            dst = env->heapAlloc(dwBytes);
            if (dst != nullptr)
            {
                return dst;
            }
        }
        else
        {
            void* dst = env->heapAlloc(dwBytes);
            if (dst != nullptr)
            {
                return dst;
            }
        }
        if (gCrtHasNewHandler == 0 || env->callnewh(dwBytes) == 0)
        {
            return nullptr;
        }
    }
}

// 1.13c D2Game+0x3D4B __freeptd(ptd): with an allocated TLS slot, fetch the calling thread's _tiddata when none was passed, run
// __freefls@4 on it, then clear the slot with TlsSetValue(index, NULL). The slot is -1 when the CRT never initialized: nothing happens.
void D2GAME_CRT_Freeptd_3D4B(void* ptd, const D2GameFreeptdEnv* env)
{
    if (env->tls_index == 0xFFFFFFFFu)
    {
        return;
    }
    if (ptd == nullptr)
    {
        ptd = env->tls_get_value(env->tls_index);
    }
    if (env->freefls_fn)
    {
        env->freefls_fn(ptd);
    }
    else
    {
        d2_fn_d2game_3c04((uint8_t*)ptd, &env->freefls);
    }
    env->tls_set_value(env->tls_index, nullptr);
}

// DAT_6fd30054: 0 unknown, 1 wide API available, 2 wide API unavailable.
static uint32_t gCrtStringTypeWideState = 0;

// 1.13c D2Game+0x75E8 (421 B), ___crtGetStringTypeA. Argument slots were checked against this DLL's instructions: +0x14 output, +0x18
// code page, +0x1c LCID. The locale, codepage lookup/conversion and Win32 APIs are env inputs. Retail converts onto the stack
// (__chkstk/alloca), with calloc only after an SEH stack-overflow handler; the wide path here uses stack storage and never allocates.
int32_t D2GAME_CRT_GetStringTypeA_75E8(uint32_t info_type, const char* src, int32_t cch, uint16_t* out, uint32_t code_page, uint32_t locale, int32_t b_error, const D2GameGetStringTypeAEnv* env)
{
    if (gCrtStringTypeWideState == 0)
    {
        const uint16_t empty[] = { 0 };
        uint16_t probe[2] = { 0, 0 };
        if (env->get_string_type_w(1, empty, 1, probe) != 0)
        {
            gCrtStringTypeWideState = 1;
        }
        else if (env->get_last_error() == 0x78)
        {
            gCrtStringTypeWideState = 2;
        }
    }

    if (code_page == 0)
    {
        code_page = env->default_code_page;
    }
    if (gCrtStringTypeWideState == 1)
    {
        const int32_t wideCount = (int32_t)env->multi_byte_to_wide_char(code_page, (uint32_t)(b_error != 0) * 8u + 1u, src, cch, nullptr, 0);
        if (wideCount == 0)
        {
            return 0;
        }
        const uint32_t allocation = ((uint32_t)wideCount * 2u + 3u) & ~3u;
        auto* wide = (uint16_t*)__builtin_alloca(allocation);
        std::memset(wide, 0, (size_t)wideCount * sizeof(uint16_t));
        int32_t result = 0;
        const int32_t converted = (int32_t)env->multi_byte_to_wide_char(code_page, 1, src, cch, wide, wideCount);
        if (converted != 0)
        {
            result = env->get_string_type_w(info_type, wide, converted, out);
        }
        return result;
    }
    if (gCrtStringTypeWideState != 0 && gCrtStringTypeWideState != 2)
    {
        return 0;
    }

    const uint32_t ansiLocale = locale != 0 ? locale : env->default_locale;
    const int32_t ansiCodepage = env->locale_ansi_codepage(ansiLocale, env);
    if (ansiCodepage == -1)
    {
        return 0;
    }
    char* converted = nullptr;
    if ((uint32_t)ansiCodepage != code_page)
    {
        uint32_t length = (uint32_t)cch;
        converted = env->convert(code_page, (uint32_t)ansiCodepage, src, &length, nullptr, 0, env);
        if (converted == nullptr)
        {
            return 0;
        }
        src = converted;
        cch = (int32_t)length;
    }
    const int32_t result = env->get_string_type_a(ansiLocale, info_type, src, cch, out);
    if (converted != nullptr)
    {
        env->free(converted);
    }
    return result;
}

#include "../../../../d2game/d2game_crt_commit.h"

// 0x5924 __flush, src/core/d2game/d2game_batch_7de3.cpp (host stdio owns the buffer).
extern "C" int32_t D2Game_CrtFlush(FILE* stream);

// 1.13c D2Game+0x5981 (__fflush_lk): __flush first, any failure is -1. A stream flagged 0x4000 then commits its
// descriptor (_file) through __commit (0x8B19), whose OS and handle-table callbacks are the seam.
int32_t __cdecl D2GAME_CRT_FflushLk_6FC25981(D2GameCrtFile32* pRetailFile, FILE* pHostStream, const D2GameCrtCommitSeams* pCommitSeams)
{
	if (D2Game_CrtFlush(pHostStream) != 0)
	{
		return -1;
	}

	if ((pRetailFile->flag & 0x4000) != 0)
	{
		return d2_fn_d2game_8B19(pRetailFile->file, pCommitSeams) != 0 ? -1 : 0;
	}

	return 0;
}

// 1.13c D2Game+0x7460 (_strlen): scan to a word boundary byte by byte, then a word at a time (the
// 0x7EFEFEFF/0x81010100 zero-byte test), then the byte inside the word that holds the NUL.
size_t __cdecl D2GAME_CRT_Strlen_6FC27460(const char* szStr)
{
	const uint8_t* pByte = (const uint8_t*)szStr;
	while (((uintptr_t)pByte & 3) != 0)
	{
		if (*pByte++ == 0)
		{
			return (size_t)(pByte - 1 - (const uint8_t*)szStr);
		}
	}

	const uint32_t* pWord = (const uint32_t*)pByte;
	for (;;)
	{
		const uint32_t nWord = *pWord++;
		if (((nWord ^ 0xFFFFFFFFu ^ (nWord + 0x7EFEFEFFu)) & 0x81010100u) == 0)
		{
			continue;
		}

		const uint8_t* pBase = (const uint8_t*)(pWord - 1);
		if ((uint8_t)nWord == 0)
		{
			return (size_t)(pBase - (const uint8_t*)szStr);
		}
		if ((uint8_t)(nWord >> 8) == 0)
		{
			return (size_t)(pBase + 1 - (const uint8_t*)szStr);
		}
		if ((nWord & 0xFF0000u) == 0)
		{
			return (size_t)(pBase + 2 - (const uint8_t*)szStr);
		}
		if ((nWord & 0xFF000000u) == 0)
		{
			return (size_t)(pBase + 3 - (const uint8_t*)szStr);
		}
	}
}

// 1.13c D2Game+0x82B0 (__aulldvrm): the unsigned 64-bit quotient of (hi:lo dividend) / (hi:lo divisor), returned in EDX:EAX.
// A divisor with a high word is shifted down until it fits in 32 bits, the quotient estimated, then corrected by one.
uint64_t __stdcall D2GAME_CRT_Aulldvrm_6FC282B0(uint32_t nDividendLo, uint32_t nDividendHi, uint32_t nDivisorLo, uint32_t nDivisorHi)
{
	if (nDivisorHi == 0)
	{
		const uint32_t nQuotientHi = nDividendHi / nDivisorLo;
		const uint32_t nQuotientLo = (uint32_t)((((uint64_t)(nDividendHi % nDivisorLo) << 32) | nDividendLo) / nDivisorLo);
		return ((uint64_t)nQuotientHi << 32) | nQuotientLo;
	}

	uint32_t nShiftedDivisorLo = nDivisorLo;
	uint32_t nShiftedDivisorHi = nDivisorHi;
	uint32_t nShiftedDividendLo = nDividendLo;
	uint32_t nShiftedDividendHi = nDividendHi;
	do
	{
		nShiftedDivisorLo = (nShiftedDivisorLo >> 1) | ((nShiftedDivisorHi & 1) << 31);
		nShiftedDivisorHi >>= 1;
		nShiftedDividendLo = (nShiftedDividendLo >> 1) | ((nShiftedDividendHi & 1) << 31);
		nShiftedDividendHi >>= 1;
	}
	while (nShiftedDivisorHi != 0);

	uint32_t nQuotient = (uint32_t)((((uint64_t)nShiftedDividendHi << 32) | nShiftedDividendLo) / nShiftedDivisorLo);
	const uint64_t nProduct = (uint64_t)nDivisorLo * nQuotient;
	const uint32_t nProductHi = (uint32_t)(nProduct >> 32);
	const uint32_t nHiCheck = nProductHi + nQuotient * nDivisorHi;
	if (nHiCheck < nProductHi || nDividendHi < nHiCheck || (nDividendHi <= nHiCheck && nDividendLo < (uint32_t)nProduct))
	{
		--nQuotient;
	}
	return nQuotient;
}

// 1.13c D2Game+0x56BD (__write_nolock(fh, buf, count)): the ioinfo record is _pioinfo[fh >> 5] + (fh & 0x1F) * 0x24 (+0 OS handle,
// +4 flags). Flag 0x20 (FAPPEND) seeks to the end first. Without 0x80 (FTEXT) one WriteFile; with it the buffer is copied 0x400 bytes
// at a time inserting '\r' before every '\n', repeating while the produced length fits the written count and input remains. A
// non-zero total returns total - newlines. Otherwise a pending error maps to errno/doserrno (5 -> 9 / 5, else __dosmaperr) and the
// result is -1; with none, flag 0x40 with a leading 0x1A byte returns 0, else errno 0x1C / doserrno 0 and -1.
int32_t D2GAME_CRT_WriteNolock_56BD(uint32_t fd, const char* buf, uint32_t count, const D2GameCrtWriteNolockEnv* env)
{
	if (count == 0) return 0;

	uint8_t* io = (uint8_t*)(uintptr_t)(env->pioinfo[(int32_t)fd >> 5] + (fd & 0x1F) * 0x24);
	if ((io[4] & 0x20) != 0) env->lseeki64lk(fd, 0, 0, 2);
	uint32_t handle;
	std::memcpy(&handle, io, 4);
	uint32_t total = 0, newlines = 0, written = 0, error = 0;

	if ((io[4] & 0x80) == 0)
	{
		if (!env->writeFile(handle, buf, count, &written, 0)) error = env->getLastError();
		else total = written;
	}
	else
	{
		const char* src = buf;
		uint8_t textBuf[0x402];
		uint8_t* dst;
		do
		{
			uint32_t produced = 0;
			uint32_t offset = (uint32_t)(src - buf);
			dst = textBuf;
			do
			{
				if (count <= offset) break;
				const char c = *src++;
				++offset;
				if (c == '\n') { ++newlines; *dst++ = '\r'; ++produced; }
				*dst++ = (uint8_t)c;
				++produced;
			} while (produced < 0x400);
			if (!env->writeFile(handle, textBuf, (uint32_t)(dst - textBuf), &written, 0))
			{
				error = env->getLastError();
				break;
			}
			total += written;
		} while (((uint32_t)(dst - textBuf) <= written) && ((uint32_t)(src - buf) < count));
	}

	if (total != 0) return (int32_t)(total - newlines);
	if (error != 0)
	{
		if (error == 5)
		{
			*env->errnoPtr() = 9;
			*env->doserrnoPtr() = 5;
			return -1;
		}
		env->dosmaperr(error);
		return -1;
	}
	if ((io[4] & 0x40) != 0 && *buf == '\x1A') return 0;
	*env->errnoPtr() = 0x1C;
	*env->doserrnoPtr() = 0;
	return -1;
}
