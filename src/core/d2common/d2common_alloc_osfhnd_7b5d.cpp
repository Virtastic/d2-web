#include <cstdint>

// 1.13c D2Common+0x07B5D __alloc_osfhnd(): MSVC CRT. Under lock 0xb, scans the 64-entry osfile
// block table (each block = 32 records of 0x24 bytes: +0 handle, +4 flag byte, +5 pipech, +8 lockinit,
// +0xC CRITICAL_SECTION). Missing block: malloc 0x480, init every record (handle -1, flags 0, pipech 10,
// lockinit 0), nhandle += 32, take that block's first slot (returns idx<<5) and lock it. Otherwise
// first record with FOPEN clear gets its critical section lazily created under lock 10, is entered,
// re-checked, and its handle set to -1. Returns the fh, or -1.
// Env seam: CRT lock/heap/critsec primitives and globals are owner-supplied.
struct D2AllocOsfhndEnv {
    uint8_t* blocks[0x40];                 // DAT_6fdf1720
    uint32_t nhandle;                      // DAT_6fdf1714
    int (*mtinitlocknum)(int);
    void (*lock)(int);
    void (*unlock)(int);                   // FUN_6fd57cd0 (lock 0xb) / FUN_6fd57c36 (lock 10)
    void* (*malloc_)(uint32_t);
    int (*lockFhandle)(int fh);
    int (*initCritSec)(void* cs, uint32_t spin);
    void (*enterCS)(void* cs);
    void (*leaveCS)(void* cs);
};

int d2common_alloc_osfhnd_7b5d(D2AllocOsfhndEnv& e)
{
    int fh = -1;
    if (!e.mtinitlocknum(0xb)) return -1;
    e.lock(0xb);
    for (int i = 0; i < 0x40; i++) {
        uint8_t* b = e.blocks[i];
        if (!b) {
            b = (uint8_t*)e.malloc_(0x480);
            if (b) {
                e.blocks[i] = b;
                e.nhandle += 0x20;
                for (uint8_t* p = b; p < b + 0x480; p += 0x24) {
                    *(uint32_t*)p = 0xffffffffu;
                    p[4] = 0;
                    p[5] = 10;
                    *(uint32_t*)(p + 8) = 0;
                }
                fh = i << 5;
                if (!e.lockFhandle(fh)) fh = -1;
            }
            break;
        }
        for (uint8_t* p = b; p < b + 0x480; p += 0x24) {
            if (p[4] & 1) continue;
            if (*(uint32_t*)(p + 8) == 0) {
                e.lock(10);
                if (*(uint32_t*)(p + 8) == 0) {
                    if (!e.initCritSec(p + 0xc, 4000)) { e.unlock(10); e.unlock(0xb); return -1; }
                    (*(uint32_t*)(p + 8))++;
                }
                e.unlock(10);
            }
            e.enterCS(p + 0xc);
            if (!(p[4] & 1)) {
                *(uint32_t*)p = 0xffffffffu;
                fh = (int)((p - b) / 0x24) + i * 0x20;
                break;
            }
            e.leaveCS(p + 0xc);
        }
        if (fh != -1) break;
    }
    e.unlock(0xb);
    return fh;
}
