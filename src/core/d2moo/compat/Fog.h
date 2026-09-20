// Compat: the slice of D2MOO's Fog.h the DRLG needs. Allocation goes straight to malloc
// (pools are ignored), asserts abort, warnings print. Structs copied verbatim from D2MOO.
#pragma once
#include <D2Dll.h>
#include <D2BuildInformation.h>   // D2_VERSION_* / D2_VERSION_HAS_UBERS everywhere, like D2MOO's CMake does
#include <D2BasicTypes.h>
#include <StormHandles.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)
struct D2TxtLinkNodeStrc { char szText[32]; int nLinkIndex; D2TxtLinkNodeStrc* pPrevious; D2TxtLinkNodeStrc* pNext; };
struct D2TxtLinkTblStrc  { union { char szCode[4]; uint32_t dwCode; }; int nLinkIndex; };
struct D2TxtLinkStrc     { int32_t nRecords; int32_t nAllocatedCells; D2TxtLinkTblStrc* pTbl; D2TxtLinkNodeStrc* pFirstNode; };
struct D2BinFileStrc     { uint8_t* pDataBuffer; uint8_t* pData; int32_t nRecordCount; int32_t nCellCount; };
enum D2C_TxtFieldTypes : uint32_t {
    TXTFIELD_NONE, TXTFIELD_ASCII, TXTFIELD_DWORD, TXTFIELD_WORD, TXTFIELD_BYTE, TXTFIELD_UNKNOWN1, TXTFIELD_UNKNOWN2,
    TXTFIELD_BYTE2, TXTFIELD_DWORD2, TXTFIELD_RAW, TXTFIELD_ASCIITOCODE, TXTFIELD_UNKNOWN3, TXTFIELD_UNKNOWN4,
    TXTFIELD_CODETOBYTE, TXTFIELD_UNKNOWN5, TXTFIELD_CODETOWORD, TXTFIELD_UNKNOWN6, TXTFIELD_NAMETOINDEX,
    TXTFIELD_NAMETOINDEX2, TXTFIELD_NAMETODWORD, TXTFIELD_NAMETOWORD, TXTFIELD_NAMETOWORD2, TXTFIELD_KEYTOWORD,
    TXTFIELD_CUSTOMLINK, TXTFIELD_UNKNOWN7, TXTFIELD_CALCTODWORD, TXTFIELD_BIT
};
// pLinkField is initialised with function pointers of several shapes (MSVC's fn -> void*).
struct D2LinkFieldPtr { void* p; D2LinkFieldPtr(decltype(nullptr) = nullptr) : p(nullptr) {} D2LinkFieldPtr(int) : p(nullptr) {} D2LinkFieldPtr(long) : p(nullptr) {} template <class T> D2LinkFieldPtr(T* f) : p((void*)f) {} operator void*() const { return p; } };
struct D2BinFieldStrc { const char* szFieldName; D2C_TxtFieldTypes nFieldType; int32_t nFieldLength; int32_t nFieldOffset; D2LinkFieldPtr pLinkField; };
#pragma pack()

void        FOG_DisplayWarning(const char* szMsg, const char* szFile, int nLine);
void        FOG_DisplayAssert(const char* szMsg, const char* szFile, int nLine);
void        FOG_DisplayHalt(const char* szMsg, const char* szFile, int nLine);
void        FOG_Trace(const char* szFormat, ...);
void        FOG_TraceF(const char* szPrefix, const char* szFormat, ...);
const char* FOG_csprintf(char* szDest, const char* szFormat, ...);
inline void FOG_10050_EnterCriticalSection(void*, int) {}
inline int  FOG_IsExpansion() { return 1; }

// Fog's excel linkers (#10211-10217, #10255): code/name -> row index maps built by the table
// loaders and queried by every code-to-index fixup. compat/linker.cpp.
void* FOG_AllocLinker(const char* szFile, int nLine);
void  FOG_FreeLinker(void* pLinker);
int   FOG_10215(void* pLinker, int dwCode);                             // append a 4-char code, returns its index
int   FOG_10216_AddRecordToLinkingTable(void* pLinker, const char* szName); // append a name, returns its index
int   FOG_GetLinkIndex(void* pLinker, uint32_t dwCode, BOOL bLogError); // code -> index, -1 if absent
int   FOG_GetRowFromTxt(void* pLinker, char* szText, int nColumn);      // name -> index, -1 if absent
char* FOG_10255(void* pLinker, int nId, int a3);                        // index -> name/code
int   FOG_GetStringFromLinkIndex(void* pLinker, int nIndex, char* szString);
// D2BinFileStrc = the .bin payload (count + records); the .txt compile path is not taken.
D2BinFileStrc* FOG_CreateBinFile(void* pData, size_t dwSize);
int   FOG_GetRecordCountFromBinFile(D2BinFileStrc* pBinFile);
void  FOG_FreeBinFile(D2BinFileStrc* pBinFile);
void  FOG_10207(D2BinFileStrc* pBinFile, D2BinFieldStrc* pTbl, void* pTxt, int nRecordCount, size_t dwSize);

// Zeroed on purpose: retail pools hand out fresh (zero) pages far more often than not, and
// D2MOO marks several allocations D2_ALLOC that the original code reads before writing.
inline void* FOG_AllocPool(void*, size_t n, const char*, int, int) { return calloc(1, n ? n : 1); }
inline void* FOG_ReallocPool(void*, void* p, size_t n, const char*, int, int) { return realloc(p, n ? n : 1); }
inline void  FOG_FreePool(void*, void* p, const char*, int, int) { free(p); }
inline void* FOG_Alloc(size_t n, const char*, int, int) { return calloc(1, n ? n : 1); }
inline void  FOG_Free(void* p, const char*, int, int) { free(p); }

#define D2_ALLOC(size) FOG_Alloc((size), __FILE__, __LINE__, 0)
#define D2_CALLOC(size) memset(FOG_Alloc(size, __FILE__, __LINE__, 0), 0x00, size)
#define D2_ALLOC_STRC(type) (type*)FOG_Alloc(sizeof(type), __FILE__, __LINE__, 0)
#define D2_CALLOC_STRC(type) (type*)memset(FOG_Alloc(sizeof(type), __FILE__, __LINE__, 0), 0x00, sizeof(type))
#define D2_FREE(ptr) FOG_Free(ptr, __FILE__, __LINE__, 0)
#define D2_ALLOC_POOL(pMemPool, size) FOG_AllocPool(pMemPool, size, __FILE__, __LINE__, 0)
#define D2_CALLOC_POOL(pMemPool, size) memset(FOG_AllocPool(pMemPool, size, __FILE__, __LINE__, 0),0x00,size)
#define D2_REALLOC_POOL(pMemPool, pMem, size) FOG_ReallocPool(pMemPool, pMem, size, __FILE__, __LINE__, 0)
#define D2_ALLOC_STRC_POOL(pMemPool, type) (type*)FOG_AllocPool(pMemPool, sizeof(type), __FILE__, __LINE__, 0)
#define D2_CALLOC_STRC_POOL(pMemPool, type) (type*)memset(FOG_AllocPool(pMemPool, sizeof(type), __FILE__, __LINE__, 0), 0x00, sizeof(type))
#define D2_FREE_POOL(pMemPool, ptr) FOG_FreePool(pMemPool, ptr, __FILE__, __LINE__, 0)

// Always-on asserts: a silent divergence is worse than an abort in a parity harness.
#define D2_DISPLAY_ASSERT_THEN_BREAK(msg) (FOG_DisplayAssert(msg, __FILE__, __LINE__), abort())
#define D2_ASSERT(expr) (void)( (!!(expr)) || (D2_DISPLAY_ASSERT_THEN_BREAK(#expr), 0))
#define D2_ASSERTM(expr,msg) (void)( (!!(expr)) || (D2_DISPLAY_ASSERT_THEN_BREAK(msg), 0))
#define D2_CHECK(expr) (void)( (!!(expr)) || (FOG_DisplayWarning(#expr, __FILE__, __LINE__), 0))
#define D2_VERIFY(expr) ((!!(expr)) || (D2_DISPLAY_ASSERT_THEN_BREAK(#expr), false))
#define D2_VERIFYM(expr,msg) ((!!(expr)) || (D2_DISPLAY_ASSERT_THEN_BREAK(msg), false))
#define D2_UNREACHABLE D2_ASSERT(false)
#define D2_LOCK(pCriticalSection) D2_ASSERT(pCriticalSection); FOG_10050_EnterCriticalSection(pCriticalSection, __LINE__);
extern "C" int d2_trace_level;   // d2-web: level id that D2_MONS_TRACE prints for (set by the monspawn driver)
#define D2_UNLOCK(pCriticalSection) D2_ASSERT(pCriticalSection); (void)0;
inline uint32_t FOG_LeadingZeroesCount(uint32_t v) { return v ? 31u - (uint32_t)__builtin_clz(v) : (uint32_t)-1; }   // Fog #10252: bsr -- index of the highest set bit, -1 for 0 (callers loop while >= 0)
float FOG_10083_Cos_LUT(int16_t i);   // Fog #10083/#10084: 512-entry unit-circle tables (stubs.cpp; TODO extract the exact floats from Fog.dll)
float FOG_10084_Sin_LUT(int16_t i);
inline int32_t FOG_10055_GetSyncTime() { return 0; }   // Fog #10055: server sync timer, no clock here
uint16_t FOG_ComputeStringCRC16(const char* s);            // Fog #10137, stubs.cpp
void FOG_Encode14BitsToString(uint16_t* p2, uint32_t v);   // Fog #10086, stubs.cpp
