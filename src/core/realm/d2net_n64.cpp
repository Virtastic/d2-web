// D2Net.dll MSVC CRT lock/exit layer. DLL globals live in CrtState, Win32/CRT calls are host seams.
#include <cstdint>
#include <cstddef>

struct CrtHost {
    void (*enter_cs)(void* cs);
    void (*leave_cs)(void* cs);
    int  (*init_cs)(void* cs, unsigned spin);   // ___crtInitCritSecAndSpinCount
    void* (*alloc)(size_t);
    void (*release)(void*);
    void (*terminate_process)(unsigned code);   // GetCurrentProcess+TerminateProcess
    void (*exit_process)(unsigned code);        // ___crtExitProcess
    void (*fatal_msg)(int rtc);                 // FUN_6fbf292c
    void (*msg_banner)();                       // __FF_MSGBANNER
    int* (*errno_ptr)();                        // FUN_6fbf2b78
    void* (*onexit)(void* fn);                  // __onexit (0x19B9)
};
typedef void (*CrtFn)();
struct CrtState {
    void* locks[48];            // DAT_6fbfa038, one CS pointer per lock number
    int exiting;                // DAT_6fbfade0
    int exit_started;           // DAT_6fbfade4
    unsigned char cexit_flag;   // DAT_6fbfaddc
    int app_type, app_type2;    // DAT_6fbfadf4 / DAT_6fbfadf8
    CrtFn *atexit_begin, *atexit_end;   // DAT_6fbfb7b0 / DAT_6fbfb7ac
    CrtFn *term_a_b, *term_a_e, *term_b_b, *term_b_e;  // tables at DAT_6fbfa020 / DAT_6fbfa028
    void (*exit_fn)(CrtState*, const CrtHost*, unsigned); // PTR___exit_6fbfa030
};

// 1.13c D2NET+0x0137F
static void n64_initterm(CrtFn* cur, CrtFn* end) {
    for (; cur < end; ++cur) if (*cur) (*cur)();
}

// 1.13c D2NET+0x0182B
static void n64_unlock(CrtState* s, const CrtHost* h, int n) { h->leave_cs(s->locks[n]); }

// 1.13c D2NET+0x018D7
static void n64_unlock10(CrtState* s, const CrtHost* h) { n64_unlock(s, h, 10); }

// 1.13c D2NET+0x014B0
static void n64_doexit_unlock(CrtState* s, const CrtHost* h, int param_3) {
    if (param_3 != 0) n64_unlock(s, h, 8);   // original reads doexit's 3rd arg from the caller frame
}

static void n64_lock(CrtState* s, const CrtHost* h, int n);
static void n64_doexit(CrtState* s, const CrtHost* h, unsigned code, int p2, int p3);

// 1.13c D2NET+0x0175A
static void n64_amsg_exit(CrtState* s, const CrtHost* h, int rtc) {
    if (s->app_type == 1 || (s->app_type == 0 && s->app_type2 == 1)) h->msg_banner();
    h->fatal_msg(rtc);
    s->exit_fn(s, h, 0xff);
}

// 1.13c D2NET+0x01840
static int n64_mtinitlocknum(CrtState* s, const CrtHost* h, int n) {
    void** slot = &s->locks[n];
    if (*slot) return 1;
    void* mem = h->alloc(0x18);
    if (!mem) { *h->errno_ptr() = 12; return 0; }
    n64_lock(s, h, 10);
    if (*slot == nullptr) {
        if (!h->init_cs(mem, 4000)) {
            h->release(mem);
            *h->errno_ptr() = 12;
            n64_unlock10(s, h);   // __local_unwind2 runs the SEH handler
            return 0;
        }
        *slot = mem;
    } else h->release(mem);
    n64_unlock10(s, h);
    return 1;
}

// 1.13c D2NET+0x018E0
static void n64_lock(CrtState* s, const CrtHost* h, int n) {
    if (!s->locks[n] && !n64_mtinitlocknum(s, h, n)) n64_amsg_exit(s, h, 0x11);
    h->enter_cs(s->locks[n]);
}

// 1.13c D2NET+0x01401
static void n64_doexit(CrtState* s, const CrtHost* h, unsigned code, int p2, int p3) {
    n64_lock(s, h, 8);
    if (s->exit_started == 1) h->terminate_process(code);
    s->exiting = 1;
    s->cexit_flag = (unsigned char)p3;
    if (p2 == 0) {
        if (s->atexit_begin)
            while (--s->atexit_end, s->atexit_begin <= s->atexit_end)
                if (*s->atexit_end) (*s->atexit_end)();
        n64_initterm(s->term_a_b, s->term_a_e);
    }
    n64_initterm(s->term_b_b, s->term_b_e);
    n64_doexit_unlock(s, h, p3);
    if (p3) return;
    s->exit_started = 1;
    h->exit_process(code);
}

// 1.13c D2NET+0x014C4
void n64_exit(CrtState* s, const CrtHost* h, int code) { n64_doexit(s, h, code, 0, 0); }

// 1.13c D2NET+0x014D5
void n64___exit(CrtState* s, const CrtHost* h, unsigned code) { n64_doexit(s, h, code, 1, 0); }

// 1.13c D2NET+0x014E6
void n64_cexit(CrtState* s, const CrtHost* h) { n64_doexit(s, h, 0, 0, 1); }

// 1.13c D2NET+0x019F1
int n64_atexit(const CrtHost* h, void* fn) { return (h->onexit(fn) != nullptr) - 1; }
