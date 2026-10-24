#include "d2common_crt.h"
D2CommonCrtState g_d2common_crt = {};
D2CommonWin32 g_d2common_win32 = {};
static uint8_t g_ioinfo[2048 * 0x24];
extern "C" uint8_t* d2common_ioinfo_entry(int32_t fh) { return g_ioinfo + (uint32_t)fh * 0x24; }

#include <cstdlib>

struct D2CommonAmsgEnv { int32_t errorMode, appType; void (*msgBanner)(); void (*writeMsg)(int32_t); void (*exitFn)(int32_t); };
void d2common_amsg_exit_2d73(const D2CommonAmsgEnv& e, int32_t msg);
struct D2CommonMtLockEnv {
    void* (*malloc_)(uint32_t n); void (*free_)(void* p); int32_t* (*errno_ptr)();
    void (*lock10)(); void (*unlock10)(); int32_t (*init_cs_spin)(void* cs, uint32_t spin);
};
extern "C" int32_t D2Common_2E59_MtInitLockNum(void** slot, const D2CommonMtLockEnv* env);

static void seam_enter(void* cs) { if (g_d2common_win32.EnterCriticalSection) g_d2common_win32.EnterCriticalSection(cs); }
static void seam_leave(void* cs) { if (g_d2common_win32.LeaveCriticalSection) g_d2common_win32.LeaveCriticalSection(cs); }
static void* mt_malloc(uint32_t n) { return std::malloc(n); }
static void mt_free(void* p) { std::free(p); }
static void mt_lock10() { d2common_lock_2ef9(10); }
static int32_t mt_initcs(void* cs, uint32_t) {
    if (g_d2common_win32.InitializeCriticalSection) g_d2common_win32.InitializeCriticalSection(cs);
    return 1;
}
static void mt_exit(int32_t c) { std::_Exit(c); }

// 1.13c D2Common+0x2EF9: __lock(n). Entry n of the lock table (DAT_6fde9048) empty: create it through
// __mtinitlocknum (0x2E59), fatal __amsg_exit(0x11) on failure; then EnterCriticalSection.
extern "C" void d2common_lock_2ef9(int32_t n)
{
    void** slot = &g_d2common_crt.locks[n];
    if (*slot == nullptr) {
        D2CommonMtLockEnv e = {mt_malloc, mt_free, d2common_errno_44bc, mt_lock10, d2common_unlock_7b32, mt_initcs};
        if (D2Common_2E59_MtInitLockNum(slot, &e) == 0) {
            D2CommonAmsgEnv a = {g_d2common_crt.errorMode, g_d2common_crt.appType, d2common_ff_msgbanner_5e04,
                                 d2common_nmsg_write_5c8c, mt_exit};
            d2common_amsg_exit_2d73(a, 0x11);
        }
    }
    seam_enter(*slot);
}

// 1.13c D2Common+0x7B32: __unlock(10) (9-byte thunk over 0x2E44): LeaveCriticalSection on lock-table entry 10.
extern "C" void d2common_unlock_7b32(void)
{
    seam_leave(g_d2common_crt.locks[10]);
}

// 1.13c D2Common+0x7A9B: __lock_fhandle(fh). Record = ioinfo entry; lockinitflag at +8, critical section
// at +0xC. Flag 0: under __lock(10) init the section if still unset (init failure: unlock, return 0),
// bump the flag, __unlock(10). Then EnterCriticalSection; return 1.
extern "C" int32_t d2common_fhandle_lock_7a9b(int32_t fh)
{
    uint8_t* const pio = d2common_ioinfo_entry(fh);
    int32_t* const flag = reinterpret_cast<int32_t*>(pio + 8);
    void* const cs = pio + 0xC;
    if (*flag == 0) {
        d2common_lock_2ef9(10);
        if (*flag == 0) {
            if (mt_initcs(cs, 4000) == 0) {
                d2common_unlock_7b32();
                return 0;
            }
            ++*flag;
        }
        d2common_unlock_7b32();
    }
    seam_enter(cs);
    return 1;
}

// 1.13c D2Common+0x7B3B: __unlock_fhandle(fh): LeaveCriticalSection on the ioinfo record's section (+0xC).
extern "C" void d2common_fhandle_unlock_7b3b(int32_t fh)
{
    seam_leave(d2common_ioinfo_entry(fh) + 0xC);
}

// 1.13c D2Common+0x04D01: __close(fh). fh < _nfile with FOPEN set: __lock_fhandle, re-check FOPEN (clear:
// errno = EBADF(9), result -1; set: __close_lk), then the 0x4D78 finally -> __unlock_fhandle. Otherwise
// errno = 9, _doserrno = 0, -1.
extern "C" int32_t d2common_close_4d01(int32_t fh)
{
    if (static_cast<uint32_t>(fh) < g_d2common_crt.nhandle && (d2common_ioinfo_entry(fh)[4] & 1) != 0) {
        int32_t r;
        d2common_fhandle_lock_7a9b(fh);
        if ((d2common_ioinfo_entry(fh)[4] & 1) == 0) {
            *d2common_errno_44bc() = 9;
            r = -1;
        } else {
            r = d2common_close_lk_4c7e(static_cast<uint32_t>(fh));
        }
        d2common_fhandle_unlock_7b3b(fh);
        return r;
    }
    *d2common_errno_44bc() = 9;
    g_d2common_crt.tdoserrno = 0;
    return -1;
}
