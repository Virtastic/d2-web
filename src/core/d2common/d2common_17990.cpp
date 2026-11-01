// 1.13c D2Common+0x17990: the difficulty-3 DRLG generator. Registers two level-link tables, finds (or allocates)
// level 0x68 in the DRLG level list (head +0x47C, next +0x1AC, id +0x1D0 in retail's layout; here the seam
// returns it), ORs the global flag word into the dword its +0x14 pointer addresses, then runs the outplace
// pass over 0x67/0x6A with ECX = that global. Callees are host seams.
#include <cstdint>

struct D2Common17990Host {
    void (*pRegister)(const void* pTable, const void* pFnA, const void* pFnB);   // 0x16E10
    uint32_t* (*pFindLevelFlags)(void* pDrlg, int32_t nLevelId);                  // level 0x68's +0x14 dword pointer
    void (*pOutplace)(void* pDrlg, int32_t nLevelA, int32_t nLevelB);             // 0x17760
};

// 1.13c D2Common+0x17990
extern "C" void D2Common_17990_Run(void* pDrlg, const void* pT0, const void* pFn0, const void* pT1, const void* pFn1,
                                   uint32_t nGlobal146C, const D2Common17990Host* h)
{
    h->pRegister(pT0, pFn0, nullptr);
    h->pRegister(pT1, pFn1, nullptr);
    uint32_t* pFlags = h->pFindLevelFlags(pDrlg, 0x68);
    *pFlags |= nGlobal146C;
    h->pOutplace(pDrlg, 0x67, 0x6A);
}
