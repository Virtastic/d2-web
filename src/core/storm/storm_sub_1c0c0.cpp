// 1.13c Storm+0x1C0C0: heap page allocate (free-list search, split, optional big-block VirtualAlloc).
// Retail passes the page in EAX; here it is the first arg. Words are 32-bit as in retail; pointer
// slots keep the low 32 bits (ponytail: wasm64 ceiling). Callees and Win32 are injected.
#include <cstdint>

struct StormHeapAllocEnv {
    uint32_t pageSize;                 // DAT_6FC43550 (0 = not yet queried)
    uint32_t addrTopDown;              // DAT_6FC43554
    uint32_t guardTail;                // DAT_6FC44D5C
    void (*queryPageSize)(StormHeapAllocEnv*);                          // 1A6C0
    void (*coalesce)(uint32_t* page);                                   // 1A770
    int (*commit)(uint32_t* page, uint32_t end);                        // 1A550
    void (*trim)(uint32_t* page, uint32_t* blk, uint32_t* need, int* slack);  // 1A970
    uint32_t* (*grow)(uint32_t owner, uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e);  // 1BDD0
    void* (*valloc)(void* addr, uint32_t n, uint32_t type);             // VirtualAlloc (prot RW)
    void (*onFail)();                                                   // 1ABF0
};

uint32_t* Storm_Sub1C0C0(StormHeapAllocEnv& e, uint32_t* pg, uint32_t size, uint8_t flags) {
    // 1A6F0
    int big = (!e.addrTopDown && size < 0xfe88) ? 0 : 1;
    int tail = (e.guardTail && !big) ? 1 : 0;
    uint32_t base = big ? 4 : size;
    uint32_t t = (tail ? 2u : 0u) + 8 + base;
    uint32_t need = ((0u - t) & 7) + t;
    int slack = (int)(need - t);

    uint32_t bin = need >> 5;
    if (bin > 7) bin = 8;
    if (pg[9] > 3 && pg[bin + 0xe] == 0) e.coalesce(pg);
    uint32_t reqd = need;
    uint32_t cur = pg[bin + 0xe];
    uint16_t* slot = (uint16_t*)(pg + bin + 0xe);
    uint16_t* best = nullptr;
    uint32_t bestDiff = 0x7fffffff;
    while (cur == 0 && ++bin < 9) { cur = *(uint32_t*)(slot + 2); slot += 2; }
    uint16_t* p = (uint16_t*)(uintptr_t)*(uint32_t*)slot;
    if (p) {
        uint32_t lim = 0x10;
        do {
            uint32_t d = *p - need;
            if (d < bestDiff) {
                best = slot;
                if (d < lim) break;
                lim += 4;
                bestDiff = d;
            }
            slot = p + 2;
            p = (uint16_t*)(uintptr_t)*(uint32_t*)slot;
        } while (p);
    }
    uint32_t wasTail = 0;
    uint32_t* h = pg;
    uint16_t* blk;
    if (!best) {
        uint32_t cap = pg[0xc];
        uint32_t endNeed = (pg[8] - (uint32_t)(uintptr_t)pg) + need;
        if (cap < endNeed) {
            if (cap < 0x10000000) cap *= 2;
            h = e.grow(pg[0x17], pg[1], pg[2], cap >> 3, cap >> 3, cap);
            if (!h) return nullptr;
            pg[4] = 0;
            endNeed = (h[8] - (uint32_t)(uintptr_t)h) + reqd;
        }
        if (h[0xb] < endNeed && !e.commit(h, endNeed)) return nullptr;
        blk = (uint16_t*)(uintptr_t)h[8];
        h[8] = need + (uint32_t)(uintptr_t)blk;
    } else {
        blk = (uint16_t*)(uintptr_t)*(uint32_t*)best;
        uint8_t f = ((uint8_t*)blk)[3];
        *(uint32_t*)best = *(uint32_t*)(blk + 2);
        wasTail = (f & 0x10) >> 4;
        if (pg[9] && (wasTail || ((uint32_t)*blk + (uint32_t)(uintptr_t)blk != pg[8] &&
                                  (((uint8_t*)blk)[*blk + 3] & 2)))) pg[9]--;
        e.trim(pg, (uint32_t*)blk, &need, &slack);
    }
    h[6] += size;
    h[5] += 1;
    *blk = (uint16_t)need;
    uint8_t fl = (uint8_t)((wasTail ? 0x10 : 0) | (big ? 4 : 0) | (tail ? 1 : 0) | flags);
    ((uint8_t*)blk)[2] = (uint8_t)slack;
    ((uint8_t*)blk)[3] = fl;
    *(uint32_t*)(blk + 2) = h[3];
    if (fl & 1) *(uint16_t*)((uint8_t*)blk + need - 2 - slack) = 0x12b1;
    if (!big) return (uint32_t*)(blk + 4);
    blk[4] = 0; blk[5] = 0;
    if (!e.pageSize) e.queryPageSize(&e);
    uint32_t sz = size + 0x10, off = 0;
    void* hint = nullptr;
    if (e.guardTail || e.addrTopDown) {
        off = e.pageSize - ((e.pageSize - 1) & sz);
        if (!e.addrTopDown) off &= e.pageSize - 4;
        else { off &= e.pageSize - 1; hint = e.valloc(nullptr, off + 4 + sz, 0x2000); }
    }
    uint8_t* m = (uint8_t*)e.valloc(hint, off + sz, 0x1000);
    if (!m) { e.onFail(); return nullptr; }
    uint32_t* q = (uint32_t*)(m + off);
    q[1] = (uint32_t)(uintptr_t)blk;
    q[0] = size;
    *(int16_t*)(q + 2) = (int16_t)((size + 0xffff) >> 16);
    ((uint8_t*)q)[10] = 0;
    ((uint8_t*)q)[11] = 0xc;
    q[3] = h[3];
    *(uint32_t*)(blk + 8 / 2) = (uint32_t)(uintptr_t)(q + 4);
    h[0xd] += size;
    return q + 4;
}
