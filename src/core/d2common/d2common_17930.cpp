// 1.13c D2Common+0x17930: the last-act (difficulty-4 table) DRLG generator. Registers three level-link tables,
// links the level pairs 0x6F/0x70, 0x6E/0x6F and 0x6D/0x6E and runs the outplace pass over 0x6F/0x70.
// The callees (0x16E10 table registration, 0x16CF0 level link, 0x17760 outplace pass) are host seams; the
// table and handler pointers are retail image addresses the real caller supplies.
#include <cstdint>

struct D2Common17930Host {
    void (*pRegister)(const void* pTable, const void* pFnA, const void* pFnB);   // 0x16E10
    void (*pLink)(void* pDrlg, int32_t nLevelA, int32_t nLevelB);                 // 0x16CF0
    void (*pOutplace)(void* pDrlg, int32_t nLevelA, int32_t nLevelB);             // 0x17760
};
struct D2Common17930Tables { const void *pT0, *pT1, *pT2, *pFn2; };

// 1.13c D2Common+0x17930
extern "C" void D2Common_17930_Run(void* pDrlg, const D2Common17930Tables* t, const D2Common17930Host* h)
{
    h->pRegister(t->pT0, nullptr, nullptr);
    h->pRegister(t->pT1, nullptr, nullptr);
    h->pLink(pDrlg, 0x6F, 0x70);
    h->pOutplace(pDrlg, 0x6F, 0x70);
    h->pLink(pDrlg, 0x6E, 0x6F);
    h->pLink(pDrlg, 0x6D, 0x6E);
    h->pRegister(t->pT2, t->pFn2, nullptr);
}
