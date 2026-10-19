// src/core/d2gfx/d2gfx_freefls.cpp -- 1.13c D2Gfx CRT __freefls@4, reimplemented from the cached
// decompile (ghidra-d2gfx/decomp/0x296F.c). Alpha batch lane (OBJECTIVE v8.3): implementation + marker
// only, no spec, no vector. Host model follows d2gfx_getptd.cpp and d2gfx_batch_535e.cpp: callees whose
// own rows are UNMAPPED are seams on a caller-supplied struct in retail argument order, and the module
// globals the port does not image are caller-supplied values named by their retail RVA.
//
// The same body exists in D2Client at 0x07368 (src/core/d2client/d2client_batch_7368.cpp); this is the
// D2Gfx copy, whose module globals sit at different RVAs and whose two unlocks are in-image wrappers
// (0x2AA1 __unlock(0xD), 0x2AAD __unlock(0xC)).
#include <cstdint>
#include <cstddef>

// 0x1A71 __lock, 0x2AA1/0x2AAD __unlock(0xD)/__unlock(0xC), 0x2BD4 _free, 0x51CD ___freetlocinfo.
// 0x1D6B0 DAT_6fa9d6b0 (the CRT default mbc-info block), 0x1074C PTR_DAT_6fa9074c (the process default
// locale) and 0x106F8 DAT_6fa906f8 (the image's static locale block) are module state the caller holds.
struct D2GfxFreeflsHost {
    void     (*lock)(int32_t locknum);
    void     (*unlock)(int32_t locknum);
    void     (*free_)(void* p);
    void     (*freetlocinfo)(uint32_t locinfo);
    uint32_t default_mbc;
    uint32_t default_tlocinfo;
    uint32_t static_tlocinfo;
};

// The per-thread block __getptd (0x28FE) allocates: six owned pointers, the initaddr at +0x54, the
// mbc-info refcount pointer at +0x60 and the thread locale pointer at +0x64. Retail pointers stay
// 32-bit even in the wasm64 build.
struct D2GfxFreeflsPtd {
    uint8_t  head[0x24];
    uint32_t p24;      // 0x24
    uint8_t  pad28[4];
    uint32_t p2c;      // 0x2C
    uint8_t  pad30[4];
    uint32_t p34;      // 0x34
    uint8_t  pad38[4];
    uint32_t p3c;      // 0x3C
    uint8_t  pad40[4];
    uint32_t p44;      // 0x44
    uint32_t p48;      // 0x48
    uint8_t  gap[0x54 - 0x4C];
    uint32_t p54;      // 0x54
    uint8_t  pad58[8];
    uint32_t p60;      // 0x60
    uint32_t p64;      // 0x64
};
static_assert(offsetof(D2GfxFreeflsPtd, p24) == 0x24, "ptd +0x24");
static_assert(offsetof(D2GfxFreeflsPtd, p2c) == 0x2C, "ptd +0x2C");
static_assert(offsetof(D2GfxFreeflsPtd, p34) == 0x34, "ptd +0x34");
static_assert(offsetof(D2GfxFreeflsPtd, p3c) == 0x3C, "ptd +0x3C");
static_assert(offsetof(D2GfxFreeflsPtd, p44) == 0x44, "ptd +0x44");
static_assert(offsetof(D2GfxFreeflsPtd, p48) == 0x48, "ptd +0x48");
static_assert(offsetof(D2GfxFreeflsPtd, p54) == 0x54, "ptd +0x54");
static_assert(offsetof(D2GfxFreeflsPtd, p60) == 0x60, "ptd +0x60");
static_assert(offsetof(D2GfxFreeflsPtd, p64) == 0x64, "ptd +0x64");

// 1.13c D2Gfx+0x0296F
// __freefls@4: free the six pointers at +0x24..+0x48 and the one at +0x54 unless it is the CRT initaddr
// (RVA 0x101A0, the value __getptd writes into that field); under lock 0xD drop the +0x60 mbc-info
// refcount and free it at zero when it is not the CRT default; unlock 0xD; under lock 0xC drop the
// +0x64 locale refcount and those of its four tables at +0x2C/+0x34/+0x30/+0x40 and lc_time's +0xB4
// count, deep-freeing it at zero when it is neither the process default nor the static block; unlock
// 0xC; then free the block itself. A NULL block is a no-op.
extern "C"
void d2gfx_freefls(uint8_t* ptd, const D2GfxFreeflsHost* host) {
    if (ptd == nullptr) return;
    D2GfxFreeflsPtd* p = (D2GfxFreeflsPtd*)ptd;

    if (p->p24) host->free_((void*)(uintptr_t)p->p24);
    if (p->p2c) host->free_((void*)(uintptr_t)p->p2c);
    if (p->p34) host->free_((void*)(uintptr_t)p->p34);
    if (p->p3c) host->free_((void*)(uintptr_t)p->p3c);
    if (p->p44) host->free_((void*)(uintptr_t)p->p44);
    if (p->p48) host->free_((void*)(uintptr_t)p->p48);
    if (p->p54 != 0x101A0u) host->free_((void*)(uintptr_t)p->p54);

    host->lock(0xD);
    uint32_t* mbc = (uint32_t*)(uintptr_t)p->p60;
    if (mbc != nullptr && ((*mbc -= 1), *mbc == 0) && mbc != (uint32_t*)(uintptr_t)host->default_mbc)
        host->free_(mbc);
    host->unlock(0xD);

    host->lock(0xC);
    uint32_t* loc = (uint32_t*)(uintptr_t)p->p64;
    if (loc != nullptr) {
        *loc -= 1;
        if (loc[0xB]) *(uint32_t*)(uintptr_t)loc[0xB] -= 1;
        if (loc[0xD]) *(uint32_t*)(uintptr_t)loc[0xD] -= 1;
        if (loc[0xC]) *(uint32_t*)(uintptr_t)loc[0xC] -= 1;
        if (loc[0x10]) *(uint32_t*)(uintptr_t)loc[0x10] -= 1;
        *(uint32_t*)(uintptr_t)(loc[0x13] + 0xB4) -= 1;
        if (loc != (uint32_t*)(uintptr_t)host->default_tlocinfo &&
            loc != (uint32_t*)(uintptr_t)host->static_tlocinfo && *loc == 0)
            host->freetlocinfo((uint32_t)(uintptr_t)loc);
    }
    host->unlock(0xC);

    host->free_(ptd);
}
