// 1.13c D2Common implementation batch (rows 0x4DCF0, 0x3E960, 0x4C990, 0x2F410, 0x224C0, 0x67E50).
// Bodies follow the cached 1.13c decompile. Machine state the engine does not carry yet (retail
// .data tables, the internal no-arg helper at RVA 0x1330, memory/error exports) is reached through
// installable pointer seams, so the rows are named now and wired when the dependency lands.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>

// ---- environment seams -----------------------------------------------------
// The host installs these; a null seam leaves the helper inert instead of faulting.

// 0x3E960 reads a counted table: count at retail DAT_6fdf0ba0, base at DAT_6fdf0b9c.
static int32_t (*env_10465_TableCount)(void) = nullptr;
static const uint8_t* (*env_10465_TableBase)(void) = nullptr;

// 0x4C990's only callee: retail FUN_6fd51330 (RVA 0x1330), a no-argument accessor for the grid
// record. The record's pointer field at +0x20 is the layer, whose pointer field at +0x20 is the
// word-cell array.
static void* (*env_1330_GetGridRecord)(void) = nullptr;

// Grid record / layer prefix: three int32 fields then a pointer. The pointer lands at byte 0x20
// for both the retail 32-bit build and this 64-bit one, because the fields before it are int32.
struct D2GridHeaderStrc
{
    int32_t nField0;   // 0x00
    int32_t nField1;   // 0x04
    int32_t nField2;   // 0x08
    int32_t nPad[5];   // 0x0C
    void* pCells;      // 0x20
};
static_assert(offsetof(D2GridHeaderStrc, pCells) == 0x20, "grid header pointer at retail offset 0x20");

// 1.13c D2Common+0x4DCF0: percentage scaled by an index, (n+1)*100/(n+8), integer-truncated.
extern "C"
int32_t D2Common_10456_GetDiminishingPercent(int32_t nValue)
{
    return ((nValue + 1) * 100) / (nValue + 8);
}

// 1.13c D2Common+0x3E960: record nIndex of a counted table with a 0x34-byte stride, or 0 when the
// index is not below the record count.
extern "C"
int32_t D2Common_10465_GetCountedTableRecord(int32_t nIndex)
{
    const int32_t nCount = env_10465_TableCount ? env_10465_TableCount() : 0;
    const uint8_t* pBase = env_10465_TableBase ? env_10465_TableBase() : nullptr;
    if (nIndex < nCount && pBase != nullptr)
    {
        return (int32_t)((intptr_t)pBase + (intptr_t)nIndex * 0x34);
    }
    return 0;
}

// 1.13c D2Common+0x4C990: ORs nFlags into the 16-bit cell at (nX, nY) of the grid layer obtained
// through 0x1330. The first argument is not read by the body.
extern "C"
void D2Common_10468_SetGridCellFlag(uint32_t nUnused, int32_t nX, int32_t nY, uint16_t nFlags)
{
    (void)nUnused;
    if (env_1330_GetGridRecord == nullptr)
    {
        return;
    }
    D2GridHeaderStrc* pRecord = (D2GridHeaderStrc*)env_1330_GetGridRecord();
    D2GridHeaderStrc* pLayer = pRecord ? (D2GridHeaderStrc*)pRecord->pCells : nullptr;
    if (pLayer == nullptr || pLayer->pCells == nullptr)
    {
        return;
    }
    const int32_t nIndex = (nY - pLayer->nField1) * pLayer->nField2 - pLayer->nField0 + nX;
    uint16_t* pCell = (uint16_t*)((uint8_t*)pLayer->pCells + (intptr_t)nIndex * 2);
    *pCell |= nFlags;
}

// ---- environment seams, second half ----------------------------------------

// 0x2F410 allocates through retail ordinal 10045, passed (source file, line, 0); the third stack
// slot is a flag in retail, not a size.
static void* (*env_10045_Alloc)(const char* szFile, int32_t nLine, int32_t nFlags) = nullptr;

// 0x224C0's fatal path: ordinal 10265 maps a code to text, ordinal 10024 reports it against the
// retail context global DAT_6fdda728.
static uint32_t (*env_10265_GetErrorText)(uint32_t nCode) = nullptr;
static void (*env_10024_ReportError)(void* pContext, uint32_t nText) = nullptr;
static uint8_t g_D2Common_errorContext[8];

// 0x67E50 reads a counted table of 0x220-byte records: count at singleton+0xc5c, base at +0xc58
// in the retail 32-bit data-tables layout. Exposed as seams until the field is pinned for wasm64.
static int32_t (*env_10436_TableCount)(void) = nullptr;
static const uint8_t* (*env_10436_TableBase)(void) = nullptr;

// 1.13c D2Common+0x2F410: lazily installs the 32-byte aux block at unit +0x2C and zeroes it.
// ponytail: raw retail offsets; map onto the real D2UnitStrc fields when this row is verified.
extern "C"
void D2Common_10445_EnsureUnitAuxData(uint8_t* pUnit)
{
    if (*(void**)(pUnit + 0x2c) != nullptr)
    {
        return;
    }
    void* pAux = env_10045_Alloc ? env_10045_Alloc("Units.cpp", 0x151e, 0) : nullptr;
    *(void**)(pUnit + 0x2c) = pAux;
    if (pAux != nullptr)
    {
        memset(pAux, 0, 0x20);
    }
}

// 1.13c D2Common+0x224C0: returns the dword at unit data +0xC when the unit is an item (type 4)
// with unit data present. Otherwise it maps 0x10C8 (null unit), 0x10C9 (wrong type) or 0x10CB
// (missing unit data) to text, reports it, and terminates the process.
extern "C"
uint32_t D2Common_10438_GetUnitDataDwordOrFatal(int32_t* pUnit)
{
    uint32_t nCode;
    if (pUnit == nullptr)
    {
        nCode = 0x10c8;
    }
    else if (pUnit[0] == 4)
    {
        if (pUnit[5] != 0)
        {
            return *(const uint32_t*)((const uint8_t*)(intptr_t)pUnit[5] + 0xc);
        }
        nCode = 0x10cb;
    }
    else
    {
        nCode = 0x10c9;
    }

    const uint32_t nText = env_10265_GetErrorText ? env_10265_GetErrorText(nCode) : nCode;
    if (env_10024_ReportError != nullptr)
    {
        env_10024_ReportError(g_D2Common_errorContext, nText);
    }
    std::_Exit(-1);
}

// 1.13c D2Common+0x67E50: defaults *pOut to 0xFF, then for in-range record nIndex (stride 0x220)
// copies the byte at +0xE4 and returns 1 when it is not 0xFF. Index 0 and out-of-range yield 0.
extern "C"
uint32_t D2Common_10436_GetTableRecordByteField(int32_t nIndex, uint16_t* pOut)
{
    *pOut = 0xff;
    const int32_t nCount = env_10436_TableCount ? env_10436_TableCount() : 0;
    const uint8_t* pBase = env_10436_TableBase ? env_10436_TableBase() : nullptr;
    if (nIndex < nCount && nIndex > 0 && pBase != nullptr)
    {
        const uint8_t* pRecord = pBase + (intptr_t)nIndex * 0x220;
        const uint16_t nValue = pRecord[0xe4];
        *pOut = nValue;
        if (nValue != 0xff)
        {
            return 1;
        }
    }
    return 0;
}
