// src/core/fog/fog_batch_tiny_1018b.cpp -- five tiny 1.13c Fog.dll rows from the cached decompiles
// in build-wasm/ghidra-fog/decomp/0x<RVA>.c (alpha lane, stage A). Base 0x6FF50000; every RVA below
// is retail's. All five are MSVC 2003 Release CRT glue: four ten-byte stdio tail fragments that hand
// the enclosing frame's FILE* to __unlock_file (+0x68A0), and the nine-byte tail of
// type_info::~type_info that unlocks CRT lock-table entry 0xE through +0x3C3F. Both callees are
// separate, still UNMAPPED Fog rows; the releases are no-ops until they are implemented, as in
// src/core/d2common/d2common_batch_tiny_1019i.cpp.
#include <cstddef>
#include <cstdint>

#include <Windows.h>   // LeaveCriticalSection: kernel state, a no-op under wasm

// +0x2E210 DAT_6ff7e210: the CRT lock table, 8-byte CRITICAL_SECTION stride (the decompile indexes it
// as dwords, hence `param_1 * 2`), as modelled in fog_batch_2000.cpp. Under the compat shim the entry
// address is not observable; the index is the whole rule.
static uint8_t g_fog_csTable[0x400];

// 1.13c Fog+0x272F
// Ten-byte tail fragment (caller +0x26DB, _fwprintf): __unlock_file(*(FILE**)[EBP-0x20]) -- the
// handle sits in the enclosing frame slot, so it is handed in as a parameter.
// ponytail: the release is a no-op until __unlock_file (+0x68A0) is implemented.
extern "C"
void fog_unlock_file_272F(void* pFile)
{
    (void)pFile;
}

// 1.13c Fog+0x28D8
// Ten-byte tail fragment (caller +0x2896, _fwrite): __unlock_file(*(FILE**)[EBP+0x14]) -- the 4th
// stack argument of the enclosing frame, the stream.
// ponytail: the release is a no-op until __unlock_file (+0x68A0) is implemented.
extern "C"
void fog_unlock_file_28D8(void* pFile)
{
    (void)pFile;
}

// 1.13c Fog+0x2A7B
// Ten-byte tail fragment (caller +0x2A44, _ftell): __unlock_file(*(FILE**)[EBP+8]) -- the 1st stack
// argument of the enclosing frame, the stream.
// ponytail: the release is a no-op until __unlock_file (+0x68A0) is implemented.
extern "C"
void fog_unlock_file_2A7B(void* pFile)
{
    (void)pFile;
}

// 1.13c Fog+0x2B53
// Ten-byte tail fragment (caller +0x2B14, _fseek): __unlock_file(*(FILE**)[EBP+8]) -- the 1st stack
// argument of the enclosing frame, the stream.
// ponytail: the release is a no-op until __unlock_file (+0x68A0) is implemented.
extern "C"
void fog_unlock_file_2B53(void* pFile)
{
    (void)pFile;
}

// 1.13c Fog+0x2B9A
// Nine-byte tail fragment (caller +0x2B5D, type_info::~type_info): _unlock(0xE), i.e.
// LeaveCriticalSection(&DAT_6ff7e210 + 0xE*8) through +0x3C3F. No arguments; the index is the rule.
extern "C"
void fog_unlock_e_2B9A(void)
{
    LeaveCriticalSection((LPCRITICAL_SECTION)(g_fog_csTable + (size_t)0xE * 8));
}
