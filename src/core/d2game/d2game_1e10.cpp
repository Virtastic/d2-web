// src/core/d2game/d2game_1e10.cpp -- 1.13c D2Game.dll row 0x01E10 (sub_1E10).
#include <cstdint>

struct D2GameListNode { D2GameListNode* next; D2GameListNode* prev; };

// 1.13c D2Game+0x01E10: append node (EAX) at the tail of a circular list whose sentinel is the arg.
// A null sentinel or null node is a fatal error in retail (Fog 10265/10024 then exit(-1)); trap here.
extern "C" void d2game_list_insert_tail_1e10(D2GameListNode* sentinel, D2GameListNode* node)
{
    if (!sentinel || !node) __builtin_trap();
    node->next = sentinel;
    D2GameListNode* last = sentinel->prev;
    node->prev = last;
    last->next = node;
    sentinel->prev = node;
}
