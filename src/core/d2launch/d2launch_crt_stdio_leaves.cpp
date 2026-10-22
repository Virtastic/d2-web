// src/core/d2launch/d2launch_crt_stdio_leaves.cpp -- MSVC 7.1 CRT stdio lock / open leaves of
// 1.13c D2Launch.dll (batch, implementation + markers only). Win32 / CRT callees are host seams.
// __SEH_prolog (0x36DC) / __SEH_epilog (0x3717) rewrite the caller's stack frame and the x86
// exception chain; they have no C equivalent and are not mapped.
#include <cstdint>
#include <cstddef>

struct LaunchFile { char* ptr; int32_t cnt; char* base; uint32_t flag; int32_t file; int32_t charbuf; int32_t bufsiz; char* tmpfname; };

struct LaunchStdioEnv {
    void (*lock)(int32_t n);                 // __lock (0x352E)
    void (*unlock)(int32_t n);               // 0x3479
    void (*enterCs)(void* cs);               // EnterCriticalSection
    void (*leaveCs)(void* cs);               // LeaveCriticalSection
    int32_t (*sopen)(const char* name, uint32_t oflag, int32_t shflag, int32_t pmode);  // __sopen (0x6851)
    uintptr_t iobBegin;                      // &_iob[0] (0x6FA60170), range check is (iobBegin-1, iobEnd)
    uintptr_t iobEnd;                        // 0x6FA603D1
    uint32_t* commodeFlag;                   // DAT_6fa6510c
    int32_t* openCount;                      // DAT_6fa64f90
};

// 1.13c D2Launch+0x03651 -- __RTC_Initialize (the initializer table is empty: begin == end)
extern "C" void d2launch_rtc_initialize(void) {}

// 1.13c D2Launch+0x038EE -- __lock_file
extern "C" void d2launch_lock_file(LaunchFile* f, const LaunchStdioEnv& e) {
    uintptr_t p = (uintptr_t)f;
    if (p >= e.iobBegin && p < e.iobEnd) { e.lock((int32_t)((p - e.iobBegin) >> 5) + 0x10); return; }
    e.enterCs((uint8_t*)f + 0x20);
}

// 1.13c D2Launch+0x0391D -- __lock_file2
extern "C" void d2launch_lock_file2(int32_t idx, void* f, const LaunchStdioEnv& e) {
    if (idx < 0x14) { e.lock(idx + 0x10); return; }
    e.enterCs((uint8_t*)f + 0x20);
}

// 1.13c D2Launch+0x03940 -- __unlock_file
extern "C" void d2launch_unlock_file(LaunchFile* f, const LaunchStdioEnv& e) {
    uintptr_t p = (uintptr_t)f;
    if (p >= e.iobBegin && p < e.iobEnd) { e.unlock((int32_t)((p - e.iobBegin) >> 5) + 0x10); return; }
    e.leaveCs((uint8_t*)f + 0x20);
}

// 1.13c D2Launch+0x0396F -- __unlock_file2
extern "C" void d2launch_unlock_file2(int32_t idx, void* f, const LaunchStdioEnv& e) {
    if (idx < 0x14) { e.unlock(idx + 0x10); return; }
    e.leaveCs((uint8_t*)f + 0x20);
}

// 1.13c D2Launch+0x03992 -- __openfile
extern "C" LaunchFile* d2launch_openfile(const char* name, const char* mode, int32_t shflag, LaunchFile* f, const LaunchStdioEnv& e) {
    bool bVar4 = false, bVar3 = false, bVar2;
    uint32_t oflag, fl;
    char c = *mode;
    if (c == 'a') { oflag = 0x109; fl = *e.commodeFlag | 2; }
    else if (c == 'r') { oflag = 0; fl = *e.commodeFlag | 1; }
    else if (c == 'w') { oflag = 0x301; fl = *e.commodeFlag | 2; }
    else return nullptr;
    bVar2 = true;
    for (;;) {
        ++mode; c = *mode;
        if (c == '\0' || !bVar2) break;
        if (c == 'T') { if (!(oflag & 0x1000)) { oflag |= 0x1000; continue; } }
        else if (c == '+') { if (!(oflag & 2)) { oflag = (oflag & ~1u) | 2; fl = (fl & ~3u) | 0x80; continue; } }
        else if (c == 'D') { if (!(oflag & 0x40)) { oflag |= 0x40; continue; } }
        else if (c == 'R') { if (!bVar3) { bVar3 = true; oflag |= 0x10; continue; } }
        else if (c == 'S') { if (!bVar3) { bVar3 = true; oflag |= 0x20; continue; } }
        else if (c == 'b') { if (!(oflag & 0xc000)) { oflag |= 0x8000; continue; } }
        else if (c == 'c') { if (!bVar4) { bVar4 = true; fl |= 0x4000; continue; } }
        else if (c == 'n') { if (!bVar4) { bVar4 = true; fl &= 0xffffbfffu; continue; } }
        else if (c == 't') { if (!(oflag & 0xc000)) { oflag |= 0x4000; continue; } }
        bVar2 = false;
    }
    int32_t fd = e.sopen(name, oflag, shflag, 0x1a4);
    if (fd < 0) return nullptr;
    ++*e.openCount;
    f->flag = fl; f->cnt = 0; f->ptr = nullptr; f->base = nullptr; f->tmpfname = nullptr; f->file = fd;
    return f;
}

// _tiddata: tid +0, thandle +4, terrno +8, tdoserrno +0xC
extern "C" int32_t* d2launch_errno_ptr(uint8_t* ptd) { return (int32_t*)(ptd + 8); }        // 1.13c D2Launch+0x03AFA
extern "C" uint32_t* d2launch_doserrno_ptr(uint8_t* ptd) { return (uint32_t*)(ptd + 0xC); } // 1.13c D2Launch+0x03B03
