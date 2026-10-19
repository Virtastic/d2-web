// src/core/fog/fog_batch_tiny_1018.cpp -- five 1.13c Fog.dll alpha-lane rows reimplemented from the
// cached Ghidra decompiles (build-wasm/ghidra-fog/decomp/0x<RVA>.c). Base 0x6FF50000.
//
// All five are VC7.1 Release CRT fragments the linker pulled into Fog.dll, all in the file-close and
// onexit paths: the call/ret wrapper for __unlock(8) (+0x251F), _atexit (+0x2525), the two ten-byte
// __unlock_file tails of __fsopen (+0x2589) and _fread (+0x26D1), and the nineteen-byte fopen
// wrapper (+0x2593, _fsopen with _SH_DENYNO = 0x40).
//
// Prior art: src/core/d2common/d2common_batch_1b2f.cpp implements the same four CRT fragments and
// src/core/realm/mcp_tiny_rowex.cpp / bnclient_batch_5906.cpp the same _atexit. The callees that are
// separate, still-UNMAPPED Fog rows (+0x24ED __onexit, +0x2537 __fsopen, +0x68A0 __unlock_file) are
// seams and are noted at their call sites.
#include <cstdint>
#include <cstdio>

extern "C" void d2_fn_fog_1a5b(uint32_t* r);  // 1.13c Fog+0x1A5B __unlock(8), fog_batch_1a52.cpp

// 1.13c Fog+0x0251F
// Six-byte `call 0x1A5B; ret` (its only caller is __onexit 0x24ED): release exit lock 8 through the
// wrapper immediately above, which is the same rule.
extern "C"
void fog_sub_251F(void)
{
    d2_fn_fog_1a5b(nullptr);
}

// 1.13c Fog+0x02525
// _atexit: register the function through __onexit and return (registered != 0) - 1, i.e. 0 on success
// and -1 when __onexit answered null. The onexit table is module state; __onexit (0x24ED) is a
// separate UNMAPPED row, so the callee is the seam.
extern "C"
int32_t fog_sub_2525(void* func, void* (*onexit_)(void*))
{
    return (onexit_(func) != nullptr) - 1;
}

// 1.13c Fog+0x02589
// Ten-byte tail fragment of __fsopen (0x2537, its only caller): __unlock_file(*(FILE**)[EBP-0x1C]).
// The handle lives in __fsopen's frame slot, so the fragment is handed it directly. __unlock_file
// (0x68A0) is a separate UNMAPPED row.
// ponytail: the release is a no-op until 0x68A0 is implemented.
extern "C"
void fog_sub_2589(void* pFile)
{
    (void)pFile;
}

// 1.13c Fog+0x02593
// Nineteen-byte __cdecl wrapper: _fsopen(name, mode, 0x40), 0x40 = _SH_DENYNO, which is exactly
// fopen; the FILE* result is returned to the caller (0x14540, 0x14D10, 0x183A0, 0x18580, 0x18C40 all
// assign it). Our stdio is the host CRT, so std::fopen is the same share-mode-default open.
// ponytail: the retail __fsopen's error-path unlock (0x2589) is host CRT behaviour until 0x2537 is
// implemented.
extern "C"
void* fog_sub_2593(const char* pName, const char* pMode)
{
    return std::fopen(pName, pMode);
}

// 1.13c Fog+0x026D1
// Ten-byte tail fragment of _fread (0x268F, its only caller): __unlock_file(*(FILE**)[EBP+0x14]).
// Same shape as 0x2589 with the handle in a different frame slot; handed it directly.
// ponytail: the release is a no-op until 0x68A0 is implemented.
extern "C"
void fog_sub_26D1(void* pFile)
{
    (void)pFile;
}
