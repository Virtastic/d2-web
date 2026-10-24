// D2Common 1.13c CRT leaves as real functions over module state (single thread: one _tiddata).
// Win32 calls are the only seams (function pointers set by the host).
#pragma once
#include <cstdint>

struct D2CommonCrtState {
    int32_t terrno;        // _tiddata._terrno
    uint32_t tdoserrno;    // _tiddata._tdoserrno
    uint32_t nhandle;      // DAT_6fdf1714 (_nfile)
    int32_t errorMode;     // 0x6fdef5f4
    int32_t appType;       // 0x6fdef5f8
    void (*pfnmsg)();      // 0x6fdef88c
    void* locks[48];       // DAT_6fde9048 lock table: critical-section pointer per index
};
extern D2CommonCrtState g_d2common_crt;

// Win32 platform boundary.
struct D2CommonWin32 {
    uint32_t (*SetFilePointer)(intptr_t h, int32_t off, uint32_t origin);
    uint32_t (*GetLastError)();
    int32_t (*CloseHandle)(intptr_t h);               // __close_lk
    int32_t (*SetStdHandle)(uint32_t n, uint32_t h);  // __free_osfhnd
    void (*WriteStdout)(const char*, uint32_t);
    bool (*GetModuleFileName)(char* buf, uint32_t size);
    void (*MessageBox)(const char* text, const char* caption, uint32_t flags);
    void (*EnterCriticalSection)(void* cs);
    void (*LeaveCriticalSection)(void* cs);
    void (*InitializeCriticalSection)(void* cs);
    int32_t (*WideCharToMultiByte)(uint32_t cp, uint32_t flags, const uint16_t* wch, int32_t wchlen, char* out,
                                   int32_t outlen, const char* defchar, int32_t* used_default);
};
extern D2CommonWin32 g_d2common_win32;

extern "C" {
void d2common_lock_2ef9(int32_t n);          // __lock
void d2common_unlock_7b32(void);             // __unlock(10)
int32_t d2common_fhandle_lock_7a9b(int32_t fh);   // __lock_fhandle
void d2common_fhandle_unlock_7b3b(int32_t fh);    // __unlock_fhandle
int32_t d2common_close_4d01(int32_t fh);          // __close
int32_t* d2common_errno_44bc(void);
void d2common_dosmaperr_44ce(uint32_t err);
uint8_t* d2common_ioinfo_entry(int32_t fh);  // 0x24-byte record, flags at +4
int32_t d2common_get_osfhandle_7a5a(int32_t fh);
int32_t d2common_free_osfhnd_79db(int32_t fh);
int32_t d2common_close_lk_4c7e(uint32_t fh);
void d2common_nmsg_write_5c8c(int32_t msg);
void d2common_ff_msgbanner_5e04(void);
uint32_t d2common_lseek_lk_7255(uint32_t fh, int32_t off, uint32_t origin);
int32_t d2common_wctomb_mt_73e2(uint32_t locale, char* mbch, uint16_t wch);
}
