#include <test_seam.h>
#include "GAME/Game113c.h"

// 1.13c D2Game+0x26F0
// SEH wrapper (thiscall: ECX = list, stack = node): a non-null node is built by 0x2740 (the InplaceNew_WithList body). The SEH frame is the only other thing it does.
void __fastcall D2GameDataTable_TSExplicitList_InplaceNew_Guarded(TSExplicitList<D2GameStrc, 0xDDDDDDDD>* pList, void* pNode)
{
    if (pNode)
    {
        D2_CALL(D2GameDataTable_TSExplicitList_InplaceNew_WithList, pNode, 0, pList);
    }
}
