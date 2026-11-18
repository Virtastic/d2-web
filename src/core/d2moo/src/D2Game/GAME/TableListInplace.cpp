// D2Game+0x24B0 and +0x2740: the game-table list initialisers, split out of Game.cpp so their FN probes link on their own.
#include <GAME/Game.h>

//D2Game.0x6FC3BB50
// 1.13c D2Game+0x24B0 (the list initialisation itself: the retail routine has no null check, this one skips a null list)
// Should be __thiscall
void __fastcall D2GameDataTable_TSExplicitList_InplaceNew(void* pMemory)
{
    if (TSExplicitList<D2GameStrc, 0xDDDDDDDD>* pNode = (TSExplicitList<D2GameStrc, 0xDDDDDDDD>*)pMemory)
    {
        pNode->m_terminator.m_prevlink = 0;
        pNode->m_terminator.m_next = 0;
        pNode->m_terminator.m_prevlink = &pNode->m_terminator;
        pNode->m_linkoffset = 0xDDDDDDDD;
        pNode->m_terminator.m_next = (struct D2GameStrc*)~(uintptr_t)&pNode->m_terminator;
    }
}

//D2Game.0x6FC3BB80
// 1.13c D2Game+0x2740: copies the source's link offset, re-initialises the terminator as an empty list
// Should be __thiscall, nUnused is present due to using __fastcall
void __fastcall D2GameDataTable_TSExplicitList_InplaceNew_WithList(void* pMemory, int32_t nUnused, TSExplicitList<D2GameStrc, 0xDDDDDDDD>* pList)
{
    if (TSExplicitList<D2GameStrc, 0xDDDDDDDD>* pNode = (TSExplicitList<D2GameStrc, 0xDDDDDDDD>*)pMemory)
    {
        pNode->m_terminator.m_prevlink = 0;
        pNode->m_terminator.m_next = 0;
        pNode->m_terminator.m_prevlink = &pNode->m_terminator;
        pNode->m_linkoffset = pList->m_linkoffset;
        pNode->m_terminator.m_next = (struct D2GameStrc*)~(uintptr_t)&pNode->m_terminator;
    }
}
