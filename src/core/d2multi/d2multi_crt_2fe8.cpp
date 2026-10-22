// src/core/d2multi/d2multi_crt_2fe8.cpp -- D2Multi.dll MSVC CRT lock table / cookie / __allmul, from the
// decompiles. Win32 calls go through the d2multi_crt2fe8_env seam. Not ported: __mtinit (FLS globals),
// __mtinitlocknum (SEH), __onexit_lk (register arg).
#include <cstdint>

struct d2multi_crt2fe8_env {
    int  (*initCritSec)(void* cs, uint32_t spin);  // ___crtInitCritSecAndSpinCount
    void (*deleteCritSec)(void* cs);
    void (*enterCritSec)(void* cs);
    void (*leaveCritSec)(void* cs);
    void (*free_)(void* p);
    void (*exitProcess)(uint32_t code);
    void (*reportFailureHook)(int);                // FUN_5ef9
    void (*amsgExit)(int);
    int  (*mtinitlocknum)(int lock);
    void* lockStorage;                             // 0x18-byte CRITICAL_SECTION slots
    uint32_t cookie;                               // DAT_6f9e91f0
};
struct d2multi_crt2fe8_slot { void* cs; int32_t kind; };  // lock table entry (8 bytes)

// 1.13c D2Multi+0x30E0  __allmul
int64_t d2multi_allmul(uint32_t a, int32_t b, uint32_t c, int32_t d) {
    uint64_t lo = (uint64_t)a * c;
    if (b == 0 && d == 0) return (int64_t)lo;
    uint32_t hi = (uint32_t)(lo >> 32) + (uint32_t)b * c + a * (uint32_t)d;
    return (int64_t)(((uint64_t)hi << 32) | (uint32_t)lo);
}

// 1.13c D2Multi+0x3114  report_failure
void d2multi_report_failure(d2multi_crt2fe8_env* e) {
    e->reportFailureHook(1);
    e->exitProcess(3);
}

// 1.13c D2Multi+0x3145  __security_check_cookie
void d2multi_security_check_cookie(d2multi_crt2fe8_env* e, uint32_t stackCookie) {
    if (stackCookie == e->cookie) return;
    d2multi_report_failure(e);
}

// 1.13c D2Multi+0x3153  __mtinitlocks (36 entries)
int d2multi_mtinitlocks(d2multi_crt2fe8_env* e, d2multi_crt2fe8_slot* tbl) {
    uint8_t* next = (uint8_t*)e->lockStorage;
    for (int i = 0; i < 0x24; i++) {
        if (tbl[i].kind == 1) {
            tbl[i].cs = next;
            next += 0x18;
            if (e->initCritSec(tbl[i].cs, 4000) == 0) { tbl[i].cs = nullptr; return 0; }
        }
    }
    return 1;
}

// 1.13c D2Multi+0x319C  __mtdeletelocks
void d2multi_mtdeletelocks(d2multi_crt2fe8_env* e, d2multi_crt2fe8_slot* tbl) {
    for (int i = 0; i < 0x24; i++)
        if (tbl[i].cs && tbl[i].kind != 1) {
            e->deleteCritSec(tbl[i].cs);
            e->free_(tbl[i].cs);
            tbl[i].cs = nullptr;
        }
    for (int i = 0; i < 0x24; i++)
        if (tbl[i].cs && tbl[i].kind == 1) e->deleteCritSec(tbl[i].cs);
}

// 1.13c D2Multi+0x31F1  (unlock)
void d2multi_unlock(d2multi_crt2fe8_env* e, d2multi_crt2fe8_slot* tbl, int n) {
    e->leaveCritSec(tbl[n].cs);
}

// 1.13c D2Multi+0x329D
void d2multi_unlock10(d2multi_crt2fe8_env* e, d2multi_crt2fe8_slot* tbl) {
    d2multi_unlock(e, tbl, 10);
}

// 1.13c D2Multi+0x32A6  __lock
void d2multi_lock(d2multi_crt2fe8_env* e, d2multi_crt2fe8_slot* tbl, int n) {
    if (tbl[n].cs == nullptr && e->mtinitlocknum(n) == 0) e->amsgExit(0x11);
    e->enterCritSec(tbl[n].cs);
}
