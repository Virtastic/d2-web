// 1.13c D2Common+0x44B20
// Sorted insert of an orth node into a room's or level's singly linked list (next at +0x14). The comparator is
// strict and takes (new, existing); it is a parameter so a probe can script it.
#include <cstdint>
#include <cstring>
#include "Drlg/D2DrlgDrlg.h"

static int32_t Dir44B20(const D2DrlgOrthStrc* orth)
{
    int32_t value; std::memcpy(&value, &orth->nDirection, sizeof(value)); return value;
}

// 1.13c D2Common+0x44870 (the comparator): same direction compares the boxes, else the direction decides.
extern "C" bool D2Common_44B20_Before(const D2DrlgOrthStrc* a, const D2DrlgOrthStrc* b)
{
    const int32_t da = Dir44B20(a), db = Dir44B20(b);
    if (da != db) return da < db;
    switch (da) {
    case 0: return a->pBox->nPosY < b->pBox->nPosY;
    case 1: return a->pBox->nPosX > b->pBox->nPosX;
    case 2: return a->pBox->nPosY > b->pBox->nPosY;
    case 3: return a->pBox->nPosX < b->pBox->nPosX;
    default: return false;
    }
}

extern "C" void D2Common_44B20(D2DrlgOrthStrc** head, D2DrlgOrthStrc* node, bool (*before)(const D2DrlgOrthStrc*, const D2DrlgOrthStrc*))
{
    if (!*head) { *head = node; return; }
    D2DrlgOrthStrc* prev = *head;
    D2DrlgOrthStrc* next = prev->pNext;
    if (!next) {
        if (before(node, prev)) { node->pNext = prev; *head = node; return; }
    } else {
        do {
            if (before(node, next)) break;
            prev = next;
            next = next->pNext;
        } while (next);
    }
    node->pNext = next;
    prev->pNext = node;
}
