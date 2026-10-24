// D2Net.dll MSVC 2003 CRT start/exit rows (decompile build-wasm/ghidra-d2net/decomp/0x<RVA>.c). The Win32 and CRT
// services retail imports or reaches by address are host seams (function pointers / state in N82Crt), retail argument order.
#include <cstdint>

struct N82Crt {
    // globals, named at their retail address (base 0x6FBF0000)
    int exiting;                 // 0xADE4
    int exitFlag;                // 0xADE0
    uint8_t quick;               // 0xADDC
    int initCount;               // 0xADE8
    int (*userEntry)(void*, int, void*);  // 0xB7A4 (null = none)
    void (*preInit)(int);        // 0xB7B8
    void (**atexitBegin)();      // 0xB7B0
    void (**atexitEnd)();        // 0xB7AC (decremented by the walk)
    // seams
    void (*lock)(int);           // 0x18E0 __lock
    void (*unlock)(int);         // 0x182B
    void (*terminateSelf)(unsigned);   // GetCurrentProcess + TerminateProcess
    void* (*getModuleHandle)(const char*);       // GetModuleHandleA
    void* (*getProcAddress)(void*, const char*); // GetProcAddress
    void (*exitProcess)(unsigned);               // ExitProcess
    void (**termA)(); void (**termAEnd)();       // initterm(0xA020..)
    void (**termB)(); void (**termBEnd)();       // initterm(0xA028..)
    int (**initA)(); int (**initAEnd)();         // 0xA00C..0xA018
    void (**initB)(); void (**initBEnd)();       // 0xA000..0xA008
    int (*atexit)(void (*)());                   // 0x5596; the 0x1A47 handler is passed as atexitHandler
    void (*atexitHandler)();
    int (*crtInit1)();           // GetVersionEx..heap_init..mtinit..ioinit..setargv..setenvp, returns >=0 ok
    int (*mtinit)(); void (*ioterm)(); void (*mtterm)(); void (*heapTerm)();
    void (*freeptd)(void*);
    int (*initThread)();         // 0x1F61 calloc+TlsSetValue+__initptd, 1 ok
    int (*dllMain)(void*, int);  // 0x6AA0
};

// 1.13c D2NET+0x0133D
void n82_crtExitProcess(N82Crt* c, int code) {
    void* h = c->getModuleHandle("mscoree.dll");
    if (h) {
        void* (*p)(void*, const char*) = c->getProcAddress;
        auto f = reinterpret_cast<void (*)(int)>(p(h, "CorExitProcess"));
        if (f) f(code);
    }
    c->exitProcess((unsigned)code);
}

// 1.13c D2NET+0x0136D
void n82_lock8(N82Crt* c) { c->lock(8); }

// 1.13c D2NET+0x01376
void n82_unlock8(N82Crt* c) { c->unlock(8); }

// helper for the rows below (D2Net 0x137F)
void n82_initterm(void (**b)(), void (**e)()) {
    for (; b < e; ++b)
        if (*b) (*b)();
}

// 1.13c D2NET+0x01397
int n82_cinit(N82Crt* c, int arg) {
    if (c->preInit) c->preInit(arg);
    int rc = 0;
    for (int (**p)() = c->initA; p < c->initAEnd; ++p) {
        if (rc != 0) return rc;
        if (*p) rc = (*p)();
    }
    if (rc == 0) {
        c->atexit(c->atexitHandler);
        for (void (**p)() = c->initB; p < c->initBEnd; ++p)
            if (*p) (*p)();
    }
    return rc;
}

// (n64 row) D2Net 0x14B0 (doexit tail fragment: param_3 is the caller's frame slot)
void n82_doexit_unlock(N82Crt* c, int retain) {
    if (retain != 0) c->unlock(8);
}

// (n64 row) D2Net 0x1401
void n82_doexit(N82Crt* c, unsigned code, int quick, int retain) {
    c->lock(8);
    if (c->exiting == 1) c->terminateSelf(code);
    c->exitFlag = 1;
    c->quick = (uint8_t)retain;
    if (quick == 0) {
        if (c->atexitBegin) {
            while (--c->atexitEnd, c->atexitBegin <= c->atexitEnd)
                if (*c->atexitEnd) (*c->atexitEnd)();
        }
        n82_initterm(c->termA, c->termAEnd);
    }
    n82_initterm(c->termB, c->termBEnd);
    n82_doexit_unlock(c, retain);
    if (retain != 0) return;
    c->exiting = 1;
    n82_crtExitProcess(c, (int)code);
}

// (n64 row) D2Net 0x14C4
void n82_exit(N82Crt* c, unsigned code) { n82_doexit(c, code, 0, 0); }

// (n64 row) D2Net 0x14D5
void n82__exit(N82Crt* c, unsigned code) { n82_doexit(c, code, 1, 0); }

// (n64 row) D2Net 0x14E6
void n82_cexit(N82Crt* c) { n82_doexit(c, 0, 0, 1); }

// 1.13c D2NET+0x014F5
int n82_CRT_INIT(N82Crt* c, void* inst, int reason) {
    if (reason == 1) {
        if (c->crtInit1() >= 0 && n82_cinit(c, 0) == 0) { c->initCount++; return 1; }
        c->ioterm(); c->mtterm(); c->heapTerm();
    } else if (reason == 0) {
        if (c->initCount > 0) {
            c->initCount--;
            if (c->exitFlag == 0) n82_cexit(c);
            c->ioterm(); c->mtterm(); c->heapTerm();
            return 1;
        }
    } else if (reason == 2) {
        return c->initThread();
    } else {
        if (reason == 3) c->freeptd(nullptr);
        return 1;
    }
    return 0;
}

// 1.13c D2NET+0x01676
int n82_entry(N82Crt* c, void* inst, int reason, void* reserved) {
    int rc = 1;
    if (reason == 0 && c->initCount == 0) return 0;
    if (reason == 1 || reason == 2) {
        if (c->userEntry) rc = c->userEntry(inst, reason, reserved);
        if (rc == 0 || n82_CRT_INIT(c, inst, reason) == 0) return 0;
    }
    rc = c->dllMain(inst, reason);
    if (reason == 1 && rc == 0) n82_CRT_INIT(c, inst, 0);
    if (reason == 0 || reason == 3) {
        if (n82_CRT_INIT(c, inst, reason) == 0) rc = 0;
        if (rc != 0 && c->userEntry) rc = c->userEntry(inst, reason, reserved);
    }
    return rc;
}

// (n64 row) D2Net 0x175A
void n82_amsg_exit(N82Crt* c, int msg, int banner, void (*msgbanner)(), void (*writeMsg)(int), void (*exitFn)(unsigned)) {
    if (banner) msgbanner();
    writeMsg(msg);
    exitFn(0xff);
}
