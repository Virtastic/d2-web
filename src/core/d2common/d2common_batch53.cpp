// Batch 53 implementations (alpha lane, unvectored). Offsets are 1.13c.
#include <cstdint>
typedef uintptr_t P;
static inline uint32_t ru(P b, uint32_t o) { return *(const uint32_t*)(b + o); }
static inline int32_t ri(P b, uint32_t o) { return *(const int32_t*)(b + o); }

extern int D2Common_83B30(int x, int y, P p3, P p2);
extern void D2Common_83990(int a, int b, int x, int y, P p1, P p3, P p2);

// seed at unit+0x1c4 (lo) / +0x1c8 (hi): 64-bit LCG, state = lo*0x6AC690C5 + hi.
static uint32_t seedRand(P s, uint32_t n)
{
    if ((int32_t)n < 1) return 0;
    uint64_t v = (uint64_t)ru(s, 0x1c4) * 0x6ac690c5ull + ru(s, 0x1c8);
    *(uint64_t*)(s + 0x1c4) = v;
    return (n & (n - 1)) == 0 ? (n - 1) & (uint32_t)v : (uint32_t)v % n;
}

// 1.13c D2Common+0x83E20
uint32_t D2Common_83E20(P p1, P p2, P p3)
{
    P u = (P)ru(p1, 0);
    int32_t t = ri(u, 0x1d0);
    bool small = t >= 2 && t <= 7;
    int32_t step = (ri(p1, 24) == 1 && (ru((P)ru(u, 0x14), 0) & 0xc)) ? -1 : 1;
    int32_t w = ri((P)ru(p1, 4), 8) - ri(p3, 8) * ri(p2, 0x4c) + step;
    int32_t h = ri((P)ru(p1, 4), 0xc) - ri(p3, 12) * ri(p2, 0x4c) + 1;
    uint32_t n = (uint32_t)(h * w);
    if (n == 0) return 0;
    uint32_t cell[514]; // [0]=count [1]=skip-centre flag, then (x,y) pairs from [2]
    cell[1] = (ri(p1, 24) == 1 && small && w <= 5 && h <= 5) ? 1 : 0;
    for (int32_t i = 0; i < (int32_t)n; i++) { cell[2 + i * 2] = i % w; cell[3 + i * 2] = i / w; }
    for (cell[0] = n; cell[0]; cell[0]--) { // Fisher-Yates-ish: two random picks swapped
        uint32_t a = seedRand(u, n), b = seedRand(u, n);
        uint32_t x = cell[2 + a * 2], y = cell[3 + a * 2];
        cell[2 + a * 2] = cell[2 + b * 2]; cell[3 + a * 2] = cell[3 + b * 2];
        cell[2 + b * 2] = x; cell[3 + b * 2] = y;
    }
    for (uint32_t i = 0; (int32_t)i < (int32_t)n; i++) {
        uint32_t cx = cell[2 + i * 2], cy = cell[3 + i * 2];
        if (cell[1] && cx == 2 && cy == 2) continue;
        if (!D2Common_83B30(cx, cy, p3, p2)) continue;
        uint32_t r = seedRand(u, ru(p3, 20)) ; // 0 when param_3[5] < 1
        D2Common_83990(r + 1, (ri(p3, 8) + 1) * (r + 1), cx, cy, p1, p3, p2);
        if (ri(p2, 0x44) == 0 || ri(p2, 0x44) == 1) return 1;
    }
    return 0;
}

// 1.13c D2Common+0x68340 : search the static table (stride 0x1c at 0x6fddbf2c-based rows) in
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
void D2Common_17B70(P n, const P* head)
{
    if (ri(n, 0x14) < 4) { P p = ru(n, 0x18) + 1; *(uint32_t*)(n + 0x18) = (uint32_t)p; *(uint8_t*)(n + 0x1c) = (*(int8_t*)p + *(int8_t*)(n + 0x1c)) & 3; }
    int32_t c = ri(n, 0x14) + 1; *(int32_t*)(n + 0x14) = c;
    if (c != 3) return;
    while (n != *head) {
        n = (P)ru(n, 0x20);
        P p = ru(n, 0x18) + 1; *(uint32_t*)(n + 0x18) = (uint32_t)p;
        c = ri(n, 0x14) + 1;
        *(uint8_t*)(n + 0x1c) = (*(int8_t*)p + *(int8_t*)(n + 0x1c)) & 3;
        *(int32_t*)(n + 0x14) = c;
        if (c != 3) return;
    }
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

// 1.13c D2Common+0x68800 : esi=unit, arg=param_1. Re-resolve unit->+0x10 record when its type byte (+0x2c) is not 'b' and not the wanted one.
void D2Common_68800(uint32_t ecxArg, int32_t param_1, P unit)
{
    uint32_t err = 0x273;
    if (unit) {
        err = 0x274;
        if (ru(unit, 0x10)) {
            uint8_t c1 = *(uint8_t*)((P)ru(unit, 0x10) + 0x2c);
            uint8_t c2 = (uint8_t)((((param_1 != 0xb) - 1u) & 6) + 0x6c);
            if (c1 != 'b' && c1 != c2) *(uint32_t*)(unit + 0x10) = D2Common_6C5D0(c1, c2);
            return;
        }
    }
    D2Common_Ordinal_10024(D2Common_Dat_DDA728, D2Common_Ordinal_10265(err));
    D2Common_exit(-1);
}
