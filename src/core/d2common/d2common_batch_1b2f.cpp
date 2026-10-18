// 1.13c D2Common batch: +0x1B2F __cexit, +0x1FDC, +0x2039, +0x2043, +0x219F. Five
// link-safe naming implementations transcribed from the cached decompiles; the callees that are
// separate, still-UNMAPPED rows (doexit 0x1A4A, __unlock_file2 0x427F, __unlock_file 0x4250,
// __fsopen 0x1FE7) are named in ponytail notes and not called.
#include <cstdint>
#include <cstdio>

// 1.13c D2Common+0x1B2F
// __cexit = doexit(code=0, quick=0, returning=1): run the CRT exit handlers, then return to the
// caller instead of terminating. Retail's observable is the handler walk in doexit (0x1A4A), which
// is a separate UNMAPPED row; with no handlers ported the remaining rule is "return to the caller".
// ponytail: the onexit/initterm walk lands with 0x1A4A.
extern "C"
void D2Common_1B2F_Cexit(void)
{
}

// 1.13c D2Common+0x1FDC
// Eleven-byte tail fragment of _wprintf (0x1F81, its only caller): the error path releases the file
// lock with __unlock_file2(lock=1, handle). The handle is the caller's live ESI, not an argument;
// it is taken here as a parameter. __unlock_file2 (0x427F) is a separate UNMAPPED row.
// ponytail: the release is a no-op until 0x427F is implemented.
extern "C"
void D2Common_1FDC_UnlockFile2(uintptr_t hFile)
{
    (void)hFile;
}

// 1.13c D2Common+0x2039
// Ten-byte tail fragment of __fsopen (0x1FE7, its only caller): __unlock_file(*(FILE**)[EBP-0x1C]).
// The handle lives in __fsopen's frame slot, so the fragment is handed it directly. __unlock_file
// (0x4250) is a separate UNMAPPED row.
// ponytail: the release is a no-op until 0x4250 is implemented.
extern "C"
void D2Common_2039_UnlockFile(void* pFile)
{
    (void)pFile;
}

// 1.13c D2Common+0x2043
// Nineteen-byte __cdecl wrapper: _fsopen(name, mode, 0x40) with 0x40 = _SH_DENYNO, which is exactly
// what fopen does, and its FILE* result is returned to the caller (0x12C90, 0x5D770, 0x5E710 all
// assign it). Our stdio is the host CRT, so std::fopen is the same share-mode-default open.
// ponytail: the retail __fsopen's error-path unlock (0x1FE7/0x2039) is host CRT behaviour until
// 0x1FE7 is implemented.
extern "C"
void* D2Common_2043_FSopen(const char* pName, const char* pMode)
{
    return std::fopen(pName, pMode);
}

// 1.13c D2Common+0x219F
// Ten-byte tail fragment of _fwrite (0x215D, its only caller): __unlock_file(*(FILE**)[EBP+0x14]).
// Same shape as 0x2039 with the handle in a different frame slot; handed it directly.
// ponytail: the release is a no-op until 0x4250 is implemented.
extern "C"
void D2Common_219F_UnlockFile(void* pFile)
{
    (void)pFile;
}
