// src/core/d2common/d2common_batch_tiny_1018b.cpp -- tiny 1.13c D2Common.dll rows from the cached
// decompiles in build-wasm/ghidra-d2common/decomp/0x<RVA>.c (alpha lane). Base 0x6FD50000; every RVA
// below is retail's. MSVC 2003 Release CRT glue: a lock-8 wrapper, strcpy, and SEH finally fragments
// of stdio routines calling __unlock_fhandle with the descriptor in EBX. The __unlock_fhandle leaf
// (0x7B3B) is still an UNMAPPED row, so it is a host seam argument here
// (the d2client_batch_1019h convention).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>



// _unlock(8), recovered in d2common_crt_b24.cpp.
void D2Common_19BF_Unlock8(void);

typedef void (*D2CommonUnlockFhandleFn)(int32_t file_handle);  // 0x07B3B __unlock_fhandle

// 1.13c D2Common+0x3004: a 6-byte wrapper that releases CRT lock-table entry 8 by calling 0x19BF.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_3004_Unlock8(void)
{
	D2Common_19BF_Unlock8();
}

// 1.13c D2Common+0x6960: the CRT's strcpy, returning the destination. The word-at-a-time source
// body is the standard zero-byte test; strcpy has the same observable.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t* D2Common_6960_Strcpy(uint32_t* pDest, const uint32_t* pSrc)
{
	return reinterpret_cast<uint32_t*>(
		std::strcpy(reinterpret_cast<char*>(pDest), reinterpret_cast<const char*>(pSrc)));
}

// 1.13c D2Common+0x48A6: finally fragment of a stdio routine; __unlock_fhandle(fh) with fh in EBX.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_48A6_UnlockFhandle(
	int32_t file_handle, D2CommonUnlockFhandleFn unlock_fhandle)
{
	unlock_fhandle(file_handle);
}

// 1.13c D2Common+0x4D78: finally fragment of a stdio routine; __unlock_fhandle(fh) with fh in EBX.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_4D78_UnlockFhandle(
	int32_t file_handle, D2CommonUnlockFhandleFn unlock_fhandle)
{
	unlock_fhandle(file_handle);
}

// 1.13c D2Common+0x4ED8: finally fragment of a stdio routine; __unlock_fhandle(fh) with fh in EBX.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_4ED8_UnlockFhandle(
	int32_t file_handle, D2CommonUnlockFhandleFn unlock_fhandle)
{
	unlock_fhandle(file_handle);
}

// 1.13c D2Common+0x7350: finally fragment of a stdio routine; __unlock_fhandle(fh) with fh in EBX.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_7350_UnlockFhandle(
	int32_t file_handle, D2CommonUnlockFhandleFn unlock_fhandle)
{
	unlock_fhandle(file_handle);
}

// --- second tiny batch (rows 0x2EF0, 0x3433, 0x343C, 0x3618, 0x3624, 0x44BC) --------------------
// The lock-table leaf 0x2E44 is implemented in d2common_batch_tiny_1019i.cpp.
extern "C" void D2Common_2E44_Unlock(uint32_t nIdx);

// 1.13c D2Common+0x2EF0: nine-byte wrapper (caller 0x2E59 __mtinitlocknum): _unlock(10).
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_2EF0_Unlock10(void)
{
	D2Common_2E44_Unlock(10);
}

// 1.13c D2Common+0x3433: nine-byte wrapper (caller 0x3401 ___updatetlocinfo): _unlock(0xC).
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_3433_Unlock12(void)
{
	D2Common_2E44_Unlock(0xC);
}

// 1.13c D2Common+0x343C: nine-byte FlsAlloc fallback thunk (installed by 0x365C __mtinit when
// kernel32 lacks FlsAlloc). `call [TlsAlloc]; ret 4`: the callback argument is ignored and the new
// TLS index is returned in EAX to __mtinit, which stores it. wasm has no TLS indices, so the
// allocator is a host seam; -1 (TLS_OUT_OF_INDEXES) is the failure value __mtinit tests.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_343C_TlsAlloc(
	void* callback, uint32_t (*tls_alloc)(void))
{
	(void)callback;
	return tls_alloc();
}

// 1.13c D2Common+0x3618: nine-byte wrapper (caller 0x34E6 __freefls): _unlock(0xD).
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_3618_Unlock13(void)
{
	D2Common_2E44_Unlock(0xD);
}

// 1.13c D2Common+0x3624: nine-byte wrapper (caller 0x34E6 __freefls): _unlock(0xC).
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_3624_Unlock12(void)
{
	D2Common_2E44_Unlock(0xC);
}

// 1.13c D2Common+0x44BC: the CRT `_errno` accessor -- `call 0x3475 __getptd; add eax, 8; ret`, the
// address of _terrno in the calling thread's _ptiddata (VS2003 offset 8; _tdoserrno follows at 0xC).
// 17 callers, the whole stdio/errno family. __getptd (0x3475) is still an UNMAPPED row, so it is a
// caller-supplied seam, as for the __unlock_fhandle fragments above.
typedef void* (*D2CommonGetPtdFn)(void);  // 0x03475 __getptd

extern "C" EMSCRIPTEN_KEEPALIVE int32_t* D2Common_44BC_Errno(D2CommonGetPtdFn get_ptd)
{
	return reinterpret_cast<int32_t*>(static_cast<uint8_t*>(get_ptd()) + 8);
}

// --- third tiny batch (rows 0x44C5, 0x465A, 0x4C60, 0x5390, 0x5F4A, 0x61EA) ----------------------

// 1.13c D2Common+0x44C5: the CRT `_doserrno` accessor -- `call 0x3475 __getptd; add eax, 0xC; ret`,
// the address of _tdoserrno in the calling thread's _ptiddata (VS2003: _terrno at 8, _tdoserrno at
// 0xC). 11 callers, the stdio/errno family. __getptd (0x3475) is still UNMAPPED, so it stays a
// caller-supplied seam, exactly as for 0x44BC above.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t* D2Common_44C5_Doserrno(D2CommonGetPtdFn get_ptd)
{
	return reinterpret_cast<int32_t*>(static_cast<uint8_t*>(get_ptd()) + 0xC);
}

// 1.13c D2Common+0x465A: nine-byte wrapper (caller 0x4541): _unlock(1).
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_465A_Unlock1(void)
{
	D2Common_2E44_Unlock(1);
}

// 1.13c D2Common+0x4C60: nine-byte wrapper (caller 0x4C0D): _unlock(4).
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_4C60_Unlock4(void)
{
	D2Common_2E44_Unlock(4);
}

// 1.13c D2Common+0x5390: nine-byte wrapper (caller 0x52CA): _unlock(4).
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_5390_Unlock4(void)
{
	D2Common_2E44_Unlock(4);
}

// 1.13c D2Common+0x5F4A: nine-byte wrapper (caller 0x5ED8): _unlock(4).
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_5F4A_Unlock4(void)
{
	D2Common_2E44_Unlock(4);
}

// 1.13c D2Common+0x61EA: nine-byte wrapper (caller 0x6082): _unlock(4).
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_61EA_Unlock4(void)
{
	D2Common_2E44_Unlock(4);
}

// --- fourth tiny batch (rows 0x70F80, 0x87537, 0x867D6, 0x867E9, 0x87C5B, 0x876BE) ---------------

// 1.13c D2Common+0x70F80: always returns 1; when its second argument is zero it closes the logging
// subsystem first (0x12AA0 __log_close) and otherwise touches nothing. The first decompiled argument
// is unread. 0x12AA0 is still UNMAPPED, so it stays a caller-supplied seam, as in
// d2common_batch_233b0.cpp.
typedef void (*D2CommonLogCloseFn)(void);  // 0x12AA0 __log_close

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_70F80(uint32_t unused, int32_t nFlag,
	D2CommonLogCloseFn log_close)
{
	(void)unused;
	if (nFlag == 0)
		log_close();
	return 1;
}

// 1.13c D2Common+0x87C5B (__controlfp): __control87 with the __EM_INVALID bit (0x80000) cleared from
// the mask. __control87 (0x87C29) is still UNMAPPED, so the abstract control-word transform is a
// caller-supplied seam here; D2DDraw's own copy of the pair is in d2ddraw_batch_crtfp.cpp.
typedef uint32_t (*D2CommonControl87Fn)(uint32_t newValue, uint32_t mask);  // 0x87C29 __control87

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_87C5B_Controlfp(uint32_t newValue, uint32_t mask,
	D2CommonControl87Fn control87)
{
	return control87(newValue, mask & 0xFFF7FFFFu);
}

// 1.13c D2Common+0x87537 (__setdefaultprecision): __controlfp(0x10000, 0x30000) -- select the
// default (53-bit mantissa) precision and leave rounding and exception masks unchanged.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_87537_SetDefaultPrecision(
	D2CommonControl87Fn control87)
{
	D2Common_87C5B_Controlfp(0x10000, 0x30000, control87);
}

// 1.13c D2Common+0x876BE (__IsZeroMan): 1 when the three dwords of the long-double mantissa at p are
// all zero, otherwise 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_876BE_IsZeroMan(const uint32_t* pMan)
{
	return (pMan[0] == 0 && pMan[1] == 0 && pMan[2] == 0) ? 1 : 0;
}

// --- fifth tiny batch (rows 0x878AA, 0x878C0, 0x876A3) -------------------------------------------

// 1.13c D2Common+0x876A3 (__CopyMan): copies the three dwords of the 12-byte _LDBL12 at pSrc to pDst.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_876A3_CopyMan(uint32_t* pDst, const uint32_t* pSrc)
{
	for (int i = 0; i < 3; ++i)
		pDst[i] = pSrc[i];
}

// --- sixth tiny batch (rows 0x45640, 0x87209, 0x246F0) -------------------------------------------

// 1.13c D2Common+0x45640: the selector arrives in EAX and the dword pointer in ECX (Ghidra's
// in_EAX/unaff_ parameter split). For selectors 6..0xB and 0xD8/0xD9 the pointed dword is shifted
// left by 8 as a 32-bit value; every other selector leaves it untouched.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_45640_ShiftBySelector(uint32_t nSelector, int32_t* pValue)
{
	switch (nSelector)
	{
	case 0x06: case 0x07: case 0x08: case 0x09: case 0x0A: case 0x0B:
	case 0xD8: case 0xD9:
		*pValue = (int32_t)((uint32_t)*pValue << 8);
		break;
	default:
		break;
	}
}

// 1.13c D2Common+0x87209 (__shift): when the offset (retail EDI) is non-zero, move the whole
// NUL-terminated string including its terminator to pStr+offset, i.e. memmove of strlen+1 bytes.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_87209_Shift(char* pStr, int32_t nOffset)
{
	if (nOffset != 0)
		std::memmove(pStr + nOffset, pStr, std::strlen(pStr) + 1);
}

// 0x22920 is ITEMS_CheckIfSocketableByItemId, mapped in d2common_batch_33020.cpp.
extern "C" int D2Common_10211_ITEMS_CheckIfSocketableByItemId(int nItemId);

// 1.13c D2Common+0x246F0 (D2GetItemLevelReq, ordinal 10757): for a UNIT_ITEM (unit type 4 at +0x00)
// return the byte result of ITEMS_CheckIfSocketableByItemId on the dword at +0x04; a null unit or
// any other unit type yields 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint8_t D2Common_246F0_D2GetItemLevelReq(const int32_t* pUnit)
{
	if (pUnit != nullptr && pUnit[0] == 4)
		return (uint8_t)D2Common_10211_ITEMS_CheckIfSocketableByItemId(pUnit[1]);
	return 0;
}

// --- seventh tiny batch (rows 0x86FE4, 0x3A710, 0x53370, 0x533F0, 0x44BA0, 0x87C71) -------------

// 1.13c D2Common+0x86FE4 (__fpmath): CRT float setup. 0x86FAC fills the __fptrap dispatch table,
// 0x87589 (__ms_p5_mp_test_fdiv) probes the host CPU and its answer lands in retail DAT_6fdf1490,
// and a non-zero argument selects the default x87 precision through 0x87537. Neither CRT leaf nor
// the control word exists under wasm, so each arrives as a required caller seam; the probe answer is kept in
// a file-local mirror of the retail global.
static int32_t g_D2Common_p5_fdiv_test = 0;              // retail DAT_6fdf1490
typedef void (*D2CommonFptrapInitFn)(void);              // 0x86FAC
typedef int32_t (*D2CommonMsP5FdivTestFn)(void);         // 0x87589

extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_86FE4_Fpmath(int32_t nSetDefaultPrecision,
	D2CommonFptrapInitFn fnInitFptrap, D2CommonMsP5FdivTestFn fnMsP5FdivTest,
	D2CommonControl87Fn fnControl87)
{
	fnInitFptrap();
	g_D2Common_p5_fdiv_test = fnMsP5FdivTest();
	if (nSetDefaultPrecision != 0)
		D2Common_87537_SetDefaultPrecision(fnControl87);
}

// 1.13c D2Common+0x53370 (#10038): signed index/count comparison, then retail32
// address arithmetic. These are serialized x86 addresses, never native pointers.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_53370_GetCountedTableRecord(
    int32_t nIndex, int32_t nCount, uint32_t retailBase)
{
    return nIndex < nCount ? retailBase + uint32_t(nIndex) * 0x34u : 0u;
}

// 1.13c D2Common+0x533f0 (#10924): required Fog seams preserve native text/context
// pointers; retail always terminates after reporting the error.
using D2CommonFogErrorTextFn = const char* (*)(uint32_t);
using D2CommonFogDisplayHaltFn = void (*)(void*, const char*);
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_533F0_FatalExit(
    void* errorContext, D2CommonFogErrorTextFn getErrorText,
    D2CommonFogDisplayHaltFn displayHalt)
{
    displayHalt(errorContext, getErrorText(0x6AE));
    std::_Exit(-1);
}

// 1.13c D2Common+0x87C71 (___addl): add two unsigned dwords, store the low 32-bit sum through pOut,
// and return 1 when the add carries out of bit 31. A carry out shows up as the sum being below
// either operand.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_87C71_Addl(uint32_t nLeft, uint32_t nRight,
	uint32_t* pOut)
{
	const uint32_t nSum = nLeft + nRight;
	const uint32_t nCarry = (nSum < nLeft || nSum < nRight) ? 1u : 0u;
	*pOut = nSum;
	return nCarry;
}

// --- eighth tiny batch (rows 0x535D0, 0x87589, 0x4D970, 0x85CF0, 0x85DF0) ----------------------
// 0x3B5B0 belongs to another change and is not touched here.

// 1.13c D2Common+0x535D0: consume the next numeric field of a mutable string cursor. The cursor sits
// one past the token after the separator is overwritten with a NUL; a missing separator leaves the
// cursor null and the whole remaining string is parsed. The parsed integer is the return value.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Common_535D0_SplitAndAtoi(char** ppCursor, char cSeparator,
    int32_t (*retailAtol)(const char*))
{
	char* const pStart = *ppCursor;
	char* const pSeparator = std::strchr(pStart, cSeparator);
	*ppCursor = pSeparator;
	if (pSeparator != nullptr) {
		*pSeparator = '\0';
		*ppCursor = pSeparator + 1;
	}
	// The retail CRT parser is required: host long width and overflow rules differ.
	return retailAtol(pStart);
}

// 1.13c D2Common+0x87589 (__ms_p5_mp_test_fdiv): probe the host for the divide-by-zero exception
// feature through KERNEL32, and fall back to the internal CRT test (0x87549) when the module or the
// export is absent. Both host calls and the fallback are seams under wasm.
typedef void* (*D2CommonGetModuleHandleFn)(const char* szModule);       // GetModuleHandleA
typedef void* (*D2CommonGetProcAddressFn)(void* hModule, const char* szName);
typedef int32_t (*D2CommonIsProcessorFeaturePresentFn)(uint32_t nFeature);
typedef int32_t (*D2CommonMsP5TestFdivFn)(void);                          // 0x87549

extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Common_87589_MsP5MpTestFdiv(
	D2CommonGetModuleHandleFn fnGetModuleHandle, D2CommonGetProcAddressFn fnGetProcAddress,
	D2CommonMsP5TestFdivFn fnTestFdiv)
{
	void* const hKernel32 = fnGetModuleHandle("KERNEL32");
	if (hKernel32 != nullptr) {
		auto fnFeature = reinterpret_cast<D2CommonIsProcessorFeaturePresentFn>(
			fnGetProcAddress(hKernel32, "IsProcessorFeaturePresent"));
		if (fnFeature != nullptr) {
			return fnFeature(0);
		}
	}
	return fnTestFdiv();
}

// 1.13c D2Common+0x4D970: derive two packed coordinates from an input pair. The first output is the
// difference scaled by 16 and shifted down one cell; the second is the sum scaled by 8 and shifted up
// by two cells. No callers or callees; pure arithmetic.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_4D970_CoordPair(int32_t nA, int32_t nB,
	int32_t* pOutDifference, int32_t* pOutSum)
{
	// Preserve retail's four stores: the output pointers may alias.
    *pOutDifference = int32_t((uint32_t(nA) - uint32_t(nB)) * 0x10u);
    *pOutSum = int32_t((uint32_t(nA) + uint32_t(nB)) * 8u);
    *pOutDifference = int32_t(uint32_t(*pOutDifference) - 0x10u);
    *pOutSum = int32_t(uint32_t(*pOutSum) + 0x10u);
}

// 1.13c D2Common+0x85CF0: hand the two identifiers 0x18C and 0x18D to 0x85A80 as a two-entry table
// with a zero payload. 0x85A80 is UNMAPPED, so it arrives as a seam.
typedef void (*D2CommonProcessIdPairFn)(const uint32_t* pIds, int32_t nCount, int32_t nPayload);

extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_85CF0_ProcessTwoIds(D2CommonProcessIdPairFn fnProcess)
{
	const uint32_t aIds[2] = {0x18C, 0x18D};
	fnProcess(aIds, 2, 0);
}

// 1.13c D2Common+0x85DF0: apply value 0x16C to the three indices 2, 1, 3 in that order. 0x43100 is
// UNMAPPED, so it arrives as a seam.
typedef void (*D2CommonApplyIndexValueFn)(int32_t nIndex, int32_t nValue);

extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_85DF0_ApplyThree(D2CommonApplyIndexValueFn fnApply)
{
	fnApply(2, 0x16C);
	fnApply(1, 0x16C);
	fnApply(3, 0x16C);
}

// 1.13c D2Common+0x3475 (__getptd, 113 B): the current thread's _ptiddata, created on first use (same
// shape as BNClient+0x3255): TLS miss -> calloc(1,0x8C); _initaddr +0x54 = retail VA 0x6FDE9720 (RVA 0x99720),
// _holdrand +0x14 = 1, _thandle +0x04 = -1, _tid +0x00 = thread id; failure runs __amsg_exit(0x10).
// Single-threaded engine: the TLS slot is this one static block, _tid = 1. GetLastError is untouched.
static uint8_t g_d2c_ptd[0x8C];
static bool    g_d2c_ptd_ready;

extern "C" EMSCRIPTEN_KEEPALIVE void* D2Common_3475_GetPtd(void)
{
	if (!g_d2c_ptd_ready) {
		*(uint32_t*)(g_d2c_ptd + 0x54) = 0x99720;
		*(uint32_t*)(g_d2c_ptd + 0x14) = 1;
		*(uint32_t*)(g_d2c_ptd + 0x04) = 0xFFFFFFFFu;
		*(uint32_t*)(g_d2c_ptd + 0x00) = 1;
		g_d2c_ptd_ready = true;
	}
	return g_d2c_ptd;
}
