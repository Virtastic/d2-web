// Batch 53 implementations (alpha lane, unvectored). Offsets are 1.13c.
#include <cstdint>
typedef uintptr_t P;
static inline uint32_t ru(P b, uint32_t o) { return *(const uint32_t*)(b + o); }
static inline int32_t ri(P b, uint32_t o) { return *(const int32_t*)(b + o); }

// D2Common+0x83E20 lives in the game tree (src/core/d2moo/src/Retail/D2Common_83e20.cpp).

// stand-alone 32-bit layout twin of D2Common 0x68340 (the proven row runs DRLGROOMTILE_FindTilePresetUnit): search the static table (stride 0x1c at 0x6fddbf2c-based rows) in
// ranges [lo,hi] selected by key==param_1 (triplets at 0x6fddbd68, stride 12). tbl = range table, rows = row table.
int D2Common_68340(P ranges, P rows, P rangesEnd, int key, uint32_t packed, uint32_t third)
{
    for (P r = ranges; r < rangesEnd; r += 12) {
        if (ri(r, 0) != key) continue;
        int32_t hi = ri(r, 8);
        for (int32_t i = ri(r, 4); i <= hi; i++) {
            P row = rows + (P)i * 0x1c;
            if (((packed >> 20) & 0x3f) == ru(row, 0) && ((packed >> 8) & 0xff) == ru(row, 4) && third == ru(row, 8))
                return i;
        }
    }
    return -1;
}

// 1.13c D2Common+0x17B70 : eax=node, edi=head ptr. Bump node counter (+0x14); while a node hits 3, walk to parent (+0x20) and bump it.
// Returns (EAX) the node it stopped on, or 0 once the walk reached the head node.
P D2Common_17B70(P n, const P* head)
{
    if (ri(n, 0x14) < 4) { P p = ru(n, 0x18) + 1; *(uint32_t*)(n + 0x18) = (uint32_t)p; *(uint8_t*)(n + 0x1c) = (*(int8_t*)p + *(int8_t*)(n + 0x1c)) & 3; }
    int32_t c = ri(n, 0x14) + 1; *(int32_t*)(n + 0x14) = c;
    if (c != 3) return n;
    while (n != *head) {
        n = (P)ru(n, 0x20);
        P p = ru(n, 0x18) + 1; *(uint32_t*)(n + 0x18) = (uint32_t)p;
        c = ri(n, 0x14) + 1;
        *(uint8_t*)(n + 0x1c) = (*(int8_t*)p + *(int8_t*)(n + 0x1c)) & 3;
        *(int32_t*)(n + 0x14) = c;
        if (c != 3) return n;
    }
    return 0;
}

// 1.13c D2Common+0x17C80 : this={x,y}; esi=x, ebx=y, eax=list head, obj=param_1. 1 = (x,y) is the point itself, or in-bounds, open (flag 0x200 clear) and not in list.
int D2Common_17C80(P pt, P obj, int32_t x, int32_t y, P list)
{
    if (x == ri(pt, 0) && y == ri(pt, 4)) return 1;
    P s = ru(obj, 0x14);
    if (ri(s, 0x54) <= x && ri(s, 0x58) <= y && x < ri(s, 0x5c) + ri(s, 0x54) && y < ri(s, 0x60) + ri(s, 0x58) &&
        !(ru((P)ru(s, 0x2c), 4 * (ri((P)ru(s, 0x30), 4 * y) + x)) & 0x200)) {
        for (; list; list = ru(list, 0x20)) // end of list -> free cell
            if (ri(list, 0xc) == x && ri(list, 0x10) == y) return 0;
        return 1;
    }
    return 0;
}

extern uint32_t D2Common_6C5D0(uint32_t cur, uint32_t want);
extern uint32_t D2Common_Ordinal_10265(uint32_t);
extern void D2Common_Ordinal_10024(const void*, uint32_t);
extern const char D2Common_Dat_DDA728[];
extern void D2Common_exit(int);

// 1.13c D2Common+0x68800 moved to d2common_68800.cpp (pb8).
