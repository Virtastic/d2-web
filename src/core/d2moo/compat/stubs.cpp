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
void FOG_DisplayAssert(const char* m, const char* f, int l)  { fprintf(stderr, "d2moo ASSERT: %s (%s:%d)\n", m, f, l); }
void FOG_DisplayHalt(const char* m, const char* f, int l)    { fprintf(stderr, "d2moo HALT: %s (%s:%d)\n", m, f, l); abort(); }
void FOG_Trace(const char* fmt, ...) { va_list a; va_start(a, fmt); vfprintf(stderr, fmt, a); va_end(a); }
const char* FOG_csprintf(char* dst, const char* fmt, ...) { va_list a; va_start(a, fmt); vsprintf(dst, fmt, a); va_end(a); return dst; }

// --- Archive: DS1 reads go through our layered MPQ VFS ----------------------------------------
namespace d2 { extern MpqVfs* g_drlgVfs; }
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
D2DrlgEnvironmentStrc* ENVIRONMENT_AllocDrlgEnvironment(void*) { return (D2DrlgEnvironmentStrc*)calloc(1, sizeof(D2DrlgEnvironmentStrc)); }
void ENVIRONMENT_FreeDrlgEnvironment(void*, D2DrlgEnvironmentStrc* p) { free(p); }
int  MONSTERS_ValidateMonsterId(int nMonsterId) { return (nMonsterId >= 0 && nMonsterId < sgptDataTables->nMonStatsTxtRecordCount) ? nMonsterId : -1; }

// --- Path direction lookup (D2Common 0x6FDAB750 1.10f / 0x6FDDC320 table 1.13c) -----------------
// Used by the wilderness road placer. Copied from D2MOO Path/PathMisc.cpp.
static int pathDirIndex(int nX1, int nY1, int nX2, int nY2) {
    int nDiffX = nX2 - nX1, nDiffY = nY2 - nY1;
    int nAbsDiffX = nDiffX < 0 ? -nDiffX : nDiffX, nAbsDiffY = nDiffY < 0 ? -nDiffY : nDiffY;
    if (nAbsDiffX < 2 * nAbsDiffY) {
        if (nAbsDiffY >= 2 * nAbsDiffX) {
            if (nDiffX < 0) {
                if (nDiffY < -1) return 5;
                else if (nDiffY > 1) nDiffY = 2;
                return nDiffY + 7;
            }
            nDiffX &= 1;
        }
    } else {
        if (nDiffY >= 0) nDiffY &= 1; else nDiffY = -1;
    }
    if (nDiffX < -1) nDiffX = -2; else if (nDiffX > 1) nDiffX = 2;
    if (nDiffY < -1) return 5 * nDiffX + 10;
    else if (nDiffY > 1) nDiffY = 2;
    return nDiffY + 5 * nDiffX + 12;
}
int sub_6FDAB750(int nX1, int nY1, int nX2, int nY2) {
    static const int8_t k[25] = { 5,4,4,4,3, 6,5,4,3,2, 6,6,6,2,2, 6,7,0,1,2, 7,0,0,0,1 };
    return k[pathDirIndex(nX1, nY1, nX2, nY2)];
}

// --- Path (D2Common Path/Path.cpp #10166/#10167): the room a unit's dynamic path is in ---------
#include <Path/Path.h>
D2ActiveRoomStrc* PATH_GetRoom(D2DynamicPathStrc* p) { return p->pRoom; }
void PATH_SetRoom(D2DynamicPathStrc* p, D2ActiveRoomStrc* pRoom) { p->pRoom = pRoom; }
