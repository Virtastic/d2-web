// D2Launch CRT rows (l03stl). Retail Win32/CRT callees are explicit host seams; no weak symbols.
#include "../crt_freefls.h"
#include <atomic>
#include <cstdint>
#include <cstring>

struct L03ExitEnv {
    void (*lock)(int32_t);
    void (*unlock)(int32_t);
    void (*terminateProcess)(uint32_t code);                     // GetCurrentProcess()+TerminateProcess
    void (*crtExitProcess)(uint32_t code);
    void (**onexitBegin)(void); void (**onexitEnd)(void);        // DAT_6fa680d4 / DAT_6fa680d0 (end moves)
    void (**preBegin)(void); void (**preEnd)(void);              // initterm table 0x6FA60034
    void (**postBegin)(void); void (**postEnd)(void);            // initterm table 0x6FA6003C
};
static int32_t g_exiting;          // DAT_6fa64e24
static int32_t g_doexitCalled;     // DAT_6fa64e20
static uint8_t g_retcaller;        // DAT_6fa64e1c

// 1.13c D2Launch+0x1647 -- 14-byte tail of doexit: unlock(8) when retcaller (frame [ebp+0x10]) != 0.
extern "C" void d2launch_sub_1647(int32_t retcaller, const L03ExitEnv* e) { if (retcaller != 0) e->unlock(8); }

static void l03_initterm(void (**b)(void), void (**en)(void)) { for (; b < en; ++b) if (*b) (*b)(); }

// 1.13c D2Launch+0x1598 -- _doexit(code, quick, retcaller)
extern "C" void d2launch_doexit(uint32_t code, int32_t quick, int32_t retcaller, L03ExitEnv* e) {
    e->lock(8);
    if (g_exiting == 1) e->terminateProcess(code);
    g_doexitCalled = 1;
    g_retcaller = (uint8_t)retcaller;
    if (quick == 0) {
        if (e->onexitBegin != nullptr)
            while (--e->onexitEnd, e->onexitBegin <= e->onexitEnd)
                if (*e->onexitEnd) (*e->onexitEnd)();
        l03_initterm(e->preBegin, e->preEnd);
    }
    l03_initterm(e->postBegin, e->postEnd);
    d2launch_sub_1647(retcaller, e);
    if (retcaller != 0) return;
    g_exiting = 1;
    e->crtExitProcess(code);
}

// 1.13c D2Launch+0x165B -- _exit(code): doexit(code, 0, 0)
extern "C" void d2launch_exit(uint32_t code, L03ExitEnv* e) { d2launch_doexit(code, 0, 0, e); }

// 1.13c D2Launch+0x167D -- __cexit(): doexit(0, 0, 1)
extern "C" void d2launch_cexit(L03ExitEnv* e) { d2launch_doexit(0, 0, 1, e); }

struct L03FreeEnv {
    int32_t heapMode;                                 // DAT_6fa66f80 (3 = small-block heap)
    void (*lock)(int32_t); void (*unlock)(int32_t);   // unlock(4) is FUN_6fa4440b
    void* (*sbhFindBlock)(void* mem);
    void (*sbhFreeBlock)(void* hdr, void* mem);
    void (*heapFree)(void* mem);                      // HeapFree(process heap, 0, mem)
};
// 1.13c D2Launch+0x43B8 -- _free
extern "C" void d2launch_free(void* mem, const L03FreeEnv* e) {
    if (!mem) return;
    if (e->heapMode == 3) {
        e->lock(4);
        void* hdr = e->sbhFindBlock(mem);
        if (hdr) e->sbhFreeBlock(hdr, mem);
        e->unlock(4);
        if (hdr) return;
    }
    e->heapFree(mem);
}

struct L03FileEnv {
    uint32_t nhandle;                                 // DAT_6fa66f84
    uint8_t* (*osfile)(int32_t fh);                   // &ioinfo[fh>>5][fh&31].osfile (flag bit0 = open)
    intptr_t (*getOsfhandle)(int32_t fh);
    int (*closeHandle)(intptr_t h);
    uint32_t (*getLastError)(void);
    void (*freeOsfhnd)(int32_t fh);
    void (*dosmaperr)(uint32_t err);
    void (*lockFhandle)(int32_t fh); void (*unlockFhandle)(int32_t fh);
    int32_t* (*perrno)(void); uint32_t* (*pdoserrno)(void);
};
// 1.13c D2Launch+0x4429 -- __close_lk
extern "C" int32_t d2launch_close_lk(uint32_t fh, const L03FileEnv* e) {
    uint32_t err = 0;
    if (e->getOsfhandle(fh) != -1) {
        bool shared = (fh == 1 || fh == 2) && e->getOsfhandle(1) == e->getOsfhandle(2);
        if (!shared && !e->closeHandle(e->getOsfhandle(fh))) err = e->getLastError();
    }
    e->freeOsfhnd(fh);
    *e->osfile(fh) = 0;
    if (err == 0) return 0;
    e->dosmaperr(err);
    return -1;
}
// 1.13c D2Launch+0x44AC -- __close
extern "C" int32_t d2launch_close(int32_t fh, const L03FileEnv* e) {
    if ((uint32_t)fh < e->nhandle && (*e->osfile(fh) & 1)) {
        int32_t r;
        e->lockFhandle(fh);
        if (!(*e->osfile(fh) & 1)) { *e->perrno() = 9; r = -1; }
        else r = d2launch_close_lk(fh, e);
        e->unlockFhandle(fh);
        return r;
    }
    *e->perrno() = 9;
    *e->pdoserrno() = 0;
    return -1;
}

// 1.13c D2Launch+0x4785 -- __freefls@4
extern "C" void d2launch_freefls(CrtTidData* t, const CrtFreeflsEnv* env) { crt_freefls_body(t, *env); }

// ---- stack / SEH validation ------------------------------------------------------------------
struct L03Mbi { uintptr_t baseAddress, allocationBase; uint32_t protect, state, type; uintptr_t regionSize; };
struct L03StkEnv {
    size_t (*virtualQuery)(uintptr_t addr, L03Mbi* out);
    uintptr_t pageSize;                                   // GetSystemInfo.dwPageSize
    int (*virtualAlloc)(uintptr_t addr, uintptr_t size, uint32_t type, uint32_t prot);
    int (*virtualProtect)(uintptr_t addr, uintptr_t size, uint32_t prot, uint32_t* old);
    int32_t osVersionFlag;                                // DAT_6fa64de4 (1 = Win9x)
    uintptr_t sp;                                         // address of the probe local (&uStack_60)
};
// 1.13c D2Launch+0x5CF0 -- __resetstkoflw: re-arm the stack guard page; returns the VirtualProtect result.
extern "C" int32_t d2launch_resetstkoflw(const L03StkEnv* e) {
    L03Mbi m;
    if (e->virtualQuery(e->sp, &m) == 0) return 0;
    const bool nt = e->osVersionFlag != 1;
    const uintptr_t ps = e->pageSize;
    const uintptr_t cur = (e->sp & ~(ps - 1)) - ps;
    const uintptr_t lo = (nt ? 2 : 0x11) * ps + m.allocationBase;
    if (lo > cur) return 0;
    uintptr_t target = cur;
    if (nt) {
        target = m.allocationBase;
        do {
            if (e->virtualQuery(target, &m) == 0) return 0;
            target += m.regionSize;
        } while ((m.state & 0x1000) == 0);
        target = m.baseAddress;
        if ((m.protect >> 8) & 1) return 1;
        if (cur < m.baseAddress) return 0;
        if (m.baseAddress < lo) target = lo;
        e->virtualAlloc(target, ps, 0x1000, 4);
    }
    uint32_t old;
    return e->virtualProtect(target, ps, nt ? 0x104 : 1, &old);
}

struct L03Eh3Cache { uint32_t count; uint32_t page[16]; std::atomic<int32_t> lock; };   // 6fa650c0 / c8 / 108
static L03Eh3Cache g_eh3;
static uint32_t rd32(uintptr_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(a), 4); return v; }
static uint16_t rd16(uintptr_t a) { uint16_t v; std::memcpy(&v, reinterpret_cast<const void*>(a), 2); return v; }
static void eh3_front(int n, uint32_t page) {          // rotate page into slot 0, shifting 0..n down
    for (int i = 0; i <= n; ++i) { uint32_t t = g_eh3.page[i]; g_eh3.page[i] = page; page = t; }
}
// 1.13c D2Launch+0x062A6 -- __ValidateEH3RN: 1 = handler plausible, 0 = reject, -1 = unknown (query failed).
// reg = EH3 registration record; stackLimit/stackBase = TIB +8 / +4.
extern "C" int32_t d2launch_validate_eh3rn(uintptr_t reg, uintptr_t stackLimit, uintptr_t stackBase,
                                           const L03StkEnv* e) {
    const uintptr_t scope = rd32(reg + 8);
    if ((scope & 3) || (stackLimit <= scope && scope < stackBase)) return 0;
    const uint32_t tryLevel = rd32(reg + 0xc);
    if (tryLevel == 0xffffffffu) return 1;
    uint32_t i = 0, filters = 0;
    do {
        uint32_t prev = rd32(scope + i * 12);
        if (prev != 0xffffffffu && i <= prev) return 0;
        if (rd32(scope + i * 12 + 4) != 0) ++filters;
        ++i;
    } while (i <= tryLevel);
    if (filters != 0) { uintptr_t saved = rd32(reg - 8); if (saved < stackLimit || reg <= saved) return 0; }
    uint32_t page = (uint32_t)(scope & 0xfffff000u);
    for (int n = 0; n < (int)g_eh3.count; ++n) {
        if (g_eh3.page[n] != page) continue;
        if (n < 1) return 1;
        if (g_eh3.lock.exchange(1) != 0) return 1;
        int k = n;
        if (g_eh3.page[n] != page) {
            k = (int)g_eh3.count - 1;
            while (k >= 0 && g_eh3.page[k] != page) --k;
            if (k < 0) { if (g_eh3.count < 16) ++g_eh3.count; k = (int)g_eh3.count - 1; }
            else if (k == 0) { g_eh3.lock.store(0); return 1; }
        }
        eh3_front(k, page);
        g_eh3.lock.store(0);
        return 1;
    }
    L03Mbi m;
    if (e->virtualQuery(scope, &m) == 0 || m.type != 0x1000000) return -1;
    if (m.protect & 0xcc) {
        const uintptr_t b = m.allocationBase;
        if (rd16(b) != 0x5a4d) return -1;
        const uintptr_t pe = b + rd32(b + 0x3c);
        if (rd32(pe) != 0x4550 || rd16(pe + 0x18) != 0x10b) return -1;
        const uint32_t opt = rd16(pe + 0x14);
        if (rd16(pe + 6) == 0) return -1;
        const uint32_t va = rd32(pe + opt + 0x24), vs = rd32(pe + opt + 0x20);
        const uint32_t off = (uint32_t)(scope - b);
        if (va <= off && off < vs + va && (*reinterpret_cast<const uint8_t*>(pe + opt + 0x3f) & 0x80)) return 0;
    }
    const int32_t n0 = (int32_t)g_eh3.count;
    if (g_eh3.lock.exchange(1) != 0) return 1;
    int j = n0;
    while (j > 0 && g_eh3.page[j - 1] != page) --j;
    if (j == 0) {
        eh3_front(n0 < 15 ? n0 : 15, page);
        if (n0 < 16) g_eh3.count = n0 + 1;
    }
    g_eh3.lock.store(0);
    return 1;
}
