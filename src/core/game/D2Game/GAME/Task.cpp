#include "GAME/Task.h"

// 1.13c D2Game+0x1E10: append pLink at the tail of the circular list whose sentinel is pHead
// (1.10f has only the insert-after and push-front forms). Null arguments are fatal (Fog 0x77 / 0x78).
void __fastcall TASK_LinkList_InsertTail(D2LinkStrc* pHead, D2LinkStrc* pLink)
{
    D2_ASSERT(pHead);
    D2_ASSERT(pLink);

    pLink->pNext = pHead;
    pLink->pPrev = pHead->pPrev;
    pHead->pPrev->pNext = pLink;
    pHead->pPrev = pLink;
}

// 1.13c D2Game+0x1E50: unlink pLink from its list; a null link is fatal (Fog 0x87).
void __fastcall TASK_LinkList_Remove(D2LinkStrc* pLink)
{
    D2_ASSERT(pLink);

    pLink->pNext->pPrev = pLink->pPrev;
    pLink->pPrev->pNext = pLink->pNext;
}
