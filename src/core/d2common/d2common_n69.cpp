// src/core/d2common/d2common_n69.cpp -- D2Common 1.13c rows written from the decompile.
// DLL globals are passed as state; Fog allocator / CRT doexit are host function-pointer seams.
#include <cstdint>
#include <cstddef>
#include <cstring>

struct D2N69Host {
    void (*pFree)(void* p, const char* file, int line, int zero);          // Fog Ordinal_10046
    void* (*pAlloc)(int size, int zero);                                   // Fog Ordinal_10042
    int (*pDoExit)(int code, int quick, int retcaller);                    // CRT doexit @0x1A4A
};
struct D2N69OnExit { void* (*pLock)(void); void* (*pOnExitLk)(void*); void (*pUnlock)(void); };

// 1.13c D2COMMON+0x01B1E
int D2N69_Exit(const D2N69Host* h, int code) { return h->pDoExit(code, 1, 0); }

// 1.13c D2COMMON+0x01B0D
int D2N69__Exit(const D2N69Host* h, int code) { return h->pDoExit(code, 0, 0); }

// (D2COMMON+0x02FD2; marker lives in d2common_onexit_2fd2.cpp)
void* D2N69_OnExit(const D2N69OnExit* s, void* func) {
    s->pLock();
    void* r = s->pOnExitLk(func);
    s->pUnlock();
    return r;
}

// 1.13c D2COMMON+0x12000
void D2N69_UNITFINDS_FreeUnitFindData(const D2N69Host* h, uint8_t* p) {
    if (p && *(void**)(p + 4)) {
        h->pFree(*(void**)(p + 4), "..\\Source\\D2Common\\UNITS\\UnitFinds.cpp", 0xff, 0);
        *(void**)(p + 4) = nullptr;
    }
}

// 1.13c D2COMMON+0x18260
void D2N69_CHAT_FreeHoverMsg(const D2N69Host* h, void* unused, void* pMsg) {
    (void)unused;
    if (pMsg) h->pFree(pMsg, "..\\Source\\D2Common\\Chat\\Chat.cpp", 0x44, 0);
}

// 1.13c D2COMMON+0x1A6D0
void D2N69_MONSTERS_SetMonsterNameInMonsterData(const D2N69Host* h, int32_t* pUnit, const uint16_t* wszName) {
    if (pUnit && pUnit[0] == 1 && pUnit[5] && wszName) {
        size_t n = 0; while (wszName[n]) ++n;
        uint16_t* d = (uint16_t*)h->pAlloc(0xc3, 0);
        *(uint16_t**)((uint8_t*)(intptr_t)pUnit[5] + 0x2c) = d;
        memcpy(d, wszName, (n + 1) * 2);
    }
}

// 1.13c D2COMMON+0x70F40
void D2N69_Ordinal10352(void) {}

// 1.13c D2COMMON+0x71280
void* D2N69_Ordinal10041(void* pGlobal_6fdefbf0) { return pGlobal_6fdefbf0; }
