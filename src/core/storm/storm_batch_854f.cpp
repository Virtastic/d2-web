// Storm CRT leaves (VC7.1 Release): malloc, SEH, chkstk, exit/init, lock table. Same CRT as the D2Net copy
// (src/core/realm/d2net_crt_thread.cpp). Retail globals and callees are seams; 32-bit links are uint32_t.
#include <cstdint>

struct SehFrame { SehFrame* prev; };

// 1.13c STORM+0x854F _malloc: __nh_malloc(size, DAT_6fc427c4 new-handler mode).
void* storm_malloc(uint32_t size, int32_t nh_mode, void* (*nh_malloc)(uint32_t, int32_t)) { return nh_malloc(size, nh_mode); }

// 1.13c STORM+0x8A1C __SEH_prolog: link a new frame into the exception-list head.
void storm_seh_prolog(SehFrame** head, SehFrame* frame) { frame->prev = *head; *head = frame; }

// 1.13c STORM+0x8A57 __SEH_epilog: restore the saved head (ebp[-4]).
void storm_seh_epilog(SehFrame** head, SehFrame* frame) { *head = frame->prev; }

// 1.13c STORM+0x8B80 __chkstk: touch one dword per 4 KiB page below sp for `size` bytes (guard-page probe).
void storm_chkstk(volatile uint8_t* sp, uint32_t size) {
    for (; size >= 0x1000; size -= 0x1000) { sp -= 0x1000; *(volatile uint32_t*)sp = *(volatile uint32_t*)sp; }
    sp -= size; *(volatile uint32_t*)sp = *(volatile uint32_t*)sp;
}

// 1.13c STORM+0x8D6F ___crtExitProcess: CorExitProcess from mscoree.dll if loaded, then ExitProcess.
void storm_crt_exit_process(int32_t code, void* (*get_module)(const char*), void* (*get_proc)(void*, const char*),
                            void (*exit_process)(int32_t)) {
    void* m = get_module("mscoree.dll");
    if (m) { auto f = (void (*)(int32_t))get_proc(m, "CorExitProcess"); if (f) f(code); }
    exit_process(code);
}

// 1.13c STORM+0x8DB1 __initterm: call each non-null entry in [begin, end).
void storm_initterm(void (**begin)(), void (**end)()) {
    for (; begin < end; ++begin) if (*begin) (*begin)();
}

// 1.13c STORM+0x8DC9 __cinit: fpmath hook, then the C initializers (stop on nonzero), atexit(retail 0x98F4
// handler), then the C++ initializers (DAT_6fc40000..78).
int32_t storm_cinit(int32_t arg, void (*fpmath)(int32_t), int32_t (**cinit_b)(), int32_t (**cinit_e)(),
                    int32_t (*atexit_)(void (*)()), void (*atexit_fn)(), void (**cxx_b)(), void (**cxx_e)()) {
    if (fpmath) fpmath(arg);
    int32_t r = 0;
    do {
        if (r) return r;
        if (*cinit_b) r = (*cinit_b)();
        ++cinit_b;
    } while (cinit_b < cinit_e);
    if (r == 0) { atexit_(atexit_fn); storm_initterm(cxx_b, cxx_e); r = 0; }
    return r;
}

// 1.13c STORM+0x8EF6 __exit: doexit(code, quick=1, retcaller=0).
void storm_exit(int32_t code, void (*doexit)(int32_t, int32_t, int32_t)) { doexit(code, 1, 0); }

// 1.13c STORM+0x8F07 __cexit: doexit(0, quick=0, retcaller=1).
void storm_cexit(void (*doexit)(int32_t, int32_t, int32_t)) { doexit(0, 0, 1); }

// Lock table: 0x24 entries of {cs ptr, preallocated flag}; static CS storage in 0x18-byte slots.
struct StormLocks {
    uint32_t* table;          // DAT_6fc402c0
    uint8_t* cs_storage;      // DAT_6fc424e0
    uint32_t count;           // 0x24
};

// 1.13c STORM+0x8F8C __mtinitlocks: give each flagged entry a static CS slot and init it (spin 4000).
int32_t storm_mtinitlocks(const StormLocks* l, int32_t (*init_cs)(void*, uint32_t)) {
    uint8_t* slot = l->cs_storage;
    for (uint32_t i = 0; i < l->count; ++i) {
        if (l->table[i * 2 + 1] == 1) {
            l->table[i * 2] = (uint32_t)(uintptr_t)slot;
            slot += 0x18;
            if (!init_cs((void*)(uintptr_t)l->table[i * 2], 4000)) { l->table[i * 2] = 0; return 0; }
        }
    }
    return 1;
}

// 1.13c STORM+0x8FD5 __mtdeletelocks: delete+free the heap CSs, then delete the static ones.
void storm_mtdeletelocks(const StormLocks* l, void (*delete_cs)(void*), void (*free_)(void*)) {
    for (uint32_t i = 0; i < l->count; ++i) {
        uint32_t* s = l->table + i * 2;
        if (s[0] && s[1] != 1) { delete_cs((void*)(uintptr_t)s[0]); free_((void*)(uintptr_t)s[0]); s[0] = 0; }
    }
    for (uint32_t i = 0; i < l->count; ++i) {
        uint32_t* s = l->table + i * 2;
        if (s[0] && s[1] == 1) delete_cs((void*)(uintptr_t)s[0]);
    }
}

// 1.13c STORM+0x903F __mtinitlocknum: lazily malloc(0x18) a CS for lock n under lock(10); errno=12 (ENOMEM) on failure.
int32_t storm_mtinitlocknum(uint32_t* entry, void* (*malloc_)(uint32_t), void (*free_)(void*),
                            int32_t (*init_cs)(void*, uint32_t), void (*lock)(int32_t), void (*unlock10)(),
                            int32_t* errno_p) {
    if (*entry) return 1;
    void* m = malloc_(0x18);
    if (!m) { *errno_p = 12; return 0; }
    lock(10);
    if (*entry == 0) {
        if (!init_cs(m, 4000)) { free_(m); *errno_p = 12; unlock10(); return 0; }
        *entry = (uint32_t)(uintptr_t)m;
    } else free_(m);
    unlock10();
    return 1;
}
