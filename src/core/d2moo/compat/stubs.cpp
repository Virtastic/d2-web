// Compat: the non-table externals the vendored D2MOO DRLG links against, in wasm terms.
// Everything that touched units, paths, environment or Win32 is a no-op here: the DRLG is run
// headless for grid parity (M1), exactly the way the retail capture tool drives retail D2Common.
#include <D2Common.h>
#include <Archive.h>
#include <D2Seed.h>
#include <D2Environment.h>
#include <Units/Units.h>
#include <Units/UnitRoom.h>
#include <D2Monsters.h>
#include <D2DataTbls.h>
#include <DataTbls/FieldTbls.h>
#include <D2Collision.h>
#include <stdarg.h>
#include <stdio.h>
#include <vector>
#include "../../../assets/mpqvfs.h"
#include "../../prng.h"

// --- Fog ------------------------------------------------------------------------------------
void FOG_DisplayWarning(const char* m, const char* f, int l) { fprintf(stderr, "d2moo warning: %s (%s:%d)\n", m, f, l); }
// The last retail assert, kept so a caller can say WHICH one ended the engine: most are followed by
// exit(-1) (147 sites in D2Game alone), and stderr alone did not reach the test runner log.
static char g_lastAssert[512];
extern "C" __attribute__((used)) const char* d2_last_assert() { return g_lastAssert; }
void FOG_DisplayAssert(const char* m, const char* f, int l)  {
    snprintf(g_lastAssert, sizeof g_lastAssert, "%s (%s:%d)", m, f, l);
    fprintf(stderr, "d2moo ASSERT: %s\n", g_lastAssert); printf("d2moo ASSERT: %s\n", g_lastAssert); fflush(stdout);
}
void FOG_DisplayHalt(const char* m, const char* f, int l)    { fprintf(stderr, "d2moo HALT: %s (%s:%d)\n", m, f, l); abort(); }
void FOG_Trace(const char* fmt, ...) { va_list a; va_start(a, fmt); vfprintf(stderr, fmt, a); va_end(a); fputc('\n', stderr); }
void FOG_TraceF(const char*, const char* fmt, ...) { va_list a; va_start(a, fmt); vfprintf(stderr, fmt, a); va_end(a); fputc('\n', stderr); }
const char* FOG_csprintf(char* dst, const char* fmt, ...) { va_list a; va_start(a, fmt); vsprintf(dst, fmt, a); va_end(a); return dst; }

// --- Archive: DS1 reads go through our layered MPQ VFS ----------------------------------------
namespace d2 { extern MpqVfs* g_drlgVfs; }
// The existence-only VFS probe can fail only with file-not-found (Win32 2).
uint32_t ARCHIVE_GetLastError() { return 2; }
BOOL ARCHIVE_OpenFile(HD2ARCHIVE, const char* szFilePath, HSFILE* phFile, BOOL) { *phFile = nullptr; return d2::g_drlgVfs && d2::g_drlgVfs->has(szFilePath) ? TRUE : FALSE; }
void* ARCHIVE_AllocateBufferAndReadFile(HD2ARCHIVE, const char* szFilePath, size_t* pdwBytesWritten, const char* szSrcPath, int nLine) {
    std::vector<uint8_t> buf;
    if (!d2::g_drlgVfs || !d2::g_drlgVfs->read(szFilePath, buf)) {
        fprintf(stderr, "ARCHIVE read miss: %s (%s:%d)\n", szFilePath, szSrcPath, nLine);
        if (pdwBytesWritten) *pdwBytesWritten = 0;
        return nullptr;
    }
    // Retail allocates size + 800 (D2Win ARCHIVE_AllocateBufferAndReadFile) and some DS1s
    // declare one more substitution group than they contain (Act1/Outdoors/Trees.ds1: 14
    // declared, 13 present), so the parser reads into that slack. Zero slack reproduces the
    // captured 1.13c grids; 
    void* p = calloc(1, buf.size() + 800);
    memcpy(p, buf.data(), buf.size());
    if (pdwBytesWritten) *pdwBytesWritten = buf.size();
    return p;
}


// --- Environment / units / paths: not modelled ---------------------------------------------------

// --- Path direction lookup (D2Common 0x6FDAB750 1.10f / 0x6FDDC320 table 1.13c) -----------------
// (path direction table sub_6FDAB750 now comes from the vendored Path/Step.cpp)

// --- Path (D2Common Path/Path.cpp #10166/#10167): the room a unit's dynamic path is in ---------
#include <Path/Path.h>

// --- Storm string helpers (squall's storm/String.hpp declares them) ------------------------------
#include <Storm.h>
int32_t SStrCmpI(const char* a, const char* b, size_t n) { return strncasecmp(a, b, n); }
int32_t SStrCmp(const char* a, const char* b, size_t n) { return strncmp(a, b, n); }
size_t  SStrCopy(char* dest, const char* src, size_t destsize) { if (!destsize) return 0; size_t n = strlen(src); if (n >= destsize) n = destsize - 1; memcpy(dest, src, n); dest[n] = 0; return n; }
size_t  SStrLen(const char* s) { return strlen(s); }
int     SStrVPrintf(char* dst, size_t max, char* fmt, va_list args) { return vsnprintf(dst, max, fmt, args); }

// Fog #10137 / #10086: the CRC-16 over a name and the 14-bit-to-2-chars encoding the client
// preview uses. Nothing in the spawn path hashes; kept minimal until a scenario reads them.
uint16_t FOG_ComputeStringCRC16(const char* s) {
    uint16_t crc = 0;
    for (; s && *s; s++) { crc ^= (uint8_t)*s; for (int i = 0; i < 8; i++) crc = (crc & 1) ? (uint16_t)((crc >> 1) ^ 0xA001) : (uint16_t)(crc >> 1); }
    return crc;
}
void FOG_Encode14BitsToString(uint16_t* p2, uint32_t v) { p2[0] = (uint16_t)(0x20 + (v & 0x7F)); p2[1] = (uint16_t)(0x20 + ((v >> 7) & 0x7F)); }

// Fog #10083/#10084: 512-step unit circle. Fog ships these as float tables; until they are pulled
// out of the 1.13c DLL this computes them (identical for the integer-heavy callers seen so far).
#include <math.h>
float FOG_10083_Cos_LUT(int16_t i) { return (float)cos((double)(i & 511) * 6.283185307179586 / 512.0); }
float FOG_10084_Sin_LUT(int16_t i) { return (float)sin((double)(i & 511) * 6.283185307179586 / 512.0); }
